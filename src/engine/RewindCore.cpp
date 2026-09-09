// RewindCore — the thin bridge between lrcpp (C++) and anything else (C#, Python, ...).
//
// Design: this file implements the four lrcpp Components we actually need
// (Video, Audio, Input, Logger), but instead of drawing to a window like the
// SDL2 example does, each one just stores its latest data in memory. Then a
// small set of plain C functions (the "Facade") exposes that memory to
// whoever loaded this DLL.
//
// Nothing outside this file needs to know lrcpp, Frontend, or Components
// exist. That's the whole point.

#include <lrcpp/Frontend.h>
#include <lrcpp/Components.h>
#include "DynLib.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>

// ---------------------------------------------------------------------------
// Components: minimal, memory-only implementations
// ---------------------------------------------------------------------------

namespace {

class MemoryVideo : public lrcpp::Video {
public:
    bool setPixelFormat(retro_pixel_format format) override {
        pixelFormat = format;
        return true; // we accept whatever format the core wants to use
    }

    // The core calls this right after loading a game to say "here is the
    // real resolution/aspect ratio/frame rate for THIS game" (it can differ
    // from game to game, even on the same core). We must answer "true" here
    // or lrcpp's Frontend::loadGame() will treat the whole load as failed -
    // this tripped us up during testing before this override existed.
    bool setSystemAvInfo(retro_system_av_info const* info) override {
        (void)info; // we don't need anything from it yet; refresh() gives us width/height per frame anyway
        return true;
    }

    void refresh(void const* data, unsigned width, unsigned height, size_t pitch) override {
        if (data == nullptr) {
            return; // core is asking us to duplicate the previous frame; we already have it
        }

        this->width = width;
        this->height = height;
        this->pitch = pitch;

        size_t const size = pitch * height;
        buffer.resize(size);
        std::memcpy(buffer.data(), data, size);
    }

    std::vector<uint8_t> buffer;
    unsigned width = 0;
    unsigned height = 0;
    size_t pitch = 0;
    retro_pixel_format pixelFormat = RETRO_PIXEL_FORMAT_0RGB1555;
};

class MemoryAudio : public lrcpp::Audio {
public:
    size_t sampleBatch(int16_t const* data, size_t frames) override {
        size_t const oldSize = buffer.size();
        buffer.resize(oldSize + frames * 2); // stereo: 2 int16_t per frame
        std::memcpy(buffer.data() + oldSize, data, frames * 2 * sizeof(int16_t));
        return frames;
    }

    void sample(int16_t left, int16_t right) {
        buffer.push_back(left);
        buffer.push_back(right);
    }

    std::vector<int16_t> buffer;
};

class MemoryInput : public lrcpp::Input {
public:
    static unsigned const MAX_PORTS = 4;
    static unsigned const MAX_BUTTONS = 16; // RETRO_DEVICE_ID_JOYPAD_* goes up to 15

    int16_t state(unsigned port, unsigned device, unsigned index, unsigned id) override {
        (void)index;

        if (device != RETRO_DEVICE_JOYPAD || port >= MAX_PORTS || id >= MAX_BUTTONS) {
            return 0;
        }

        return buttons[port][id] ? 1 : 0;
    }

    void poll() override {
        // Nothing to do: buttons[] is updated directly by Rewind_SetButton,
        // there is no external device to poll here.
    }

    bool buttons[MAX_PORTS][MAX_BUTTONS] = {};
};

class SimpleLogger : public lrcpp::Logger {
public:
    void vprintf(retro_log_level level, char const* format, va_list args) override {
        (void)level;
        char message[512];
        vsnprintf(message, sizeof(message), format, args);
        lastMessage = message;
        fprintf(stderr, "[core] %s", message); // TEMP: so we can see what the core is complaining about
    }

    std::string lastMessage;
};

// ---------------------------------------------------------------------------
// Everything one loaded game needs, bundled together behind one opaque handle
// ---------------------------------------------------------------------------

struct RewindContext {
    DynLib dynlib;
    lrcpp::Core core{};
    lrcpp::Frontend frontend;

    MemoryVideo video;
    MemoryAudio audio;
    MemoryInput input;
    SimpleLogger logger;

    bool coreLoaded = false;
    bool gameLoaded = false;
};

// The core .dll only exposes plain C functions like "retro_init", "retro_run",
// etc. dynlib.getSymbol(name) is basically Windows' GetProcAddress: give it a
// function's name as a string, get back its address as a generic pointer.
//
// The macro below does that once per function lrcpp::Core needs, and casts
// the generic pointer to the *exact* function pointer type that field expects
// (decltype(core.member) means "whatever type that field already is").
// Writing this by hand 24 times would be repetitive and error-prone, so a
// macro generates all 24 short blocks from one line each below.
bool loadCoreFunctions(DynLib& dynlib, lrcpp::Core& core) {
#define LOAD_CORE_FUNC(member, name) \
    core.member = reinterpret_cast<decltype(core.member)>(dynlib.getSymbol("retro_" #name)); \
    if (core.member == nullptr) { \
        return false; /* the core .dll doesn't have this function - it's not a valid libretro core */ \
    }

    LOAD_CORE_FUNC(init, init)
    LOAD_CORE_FUNC(deinit, deinit)
    LOAD_CORE_FUNC(apiVersion, api_version)
    LOAD_CORE_FUNC(getSystemInfo, get_system_info)
    LOAD_CORE_FUNC(getSystemAvInfo, get_system_av_info)
    LOAD_CORE_FUNC(setEnvironment, set_environment)
    LOAD_CORE_FUNC(setVideoRefresh, set_video_refresh)
    LOAD_CORE_FUNC(setAudioSample, set_audio_sample)
    LOAD_CORE_FUNC(setAudioSampleBatch, set_audio_sample_batch)
    LOAD_CORE_FUNC(setInputPoll, set_input_poll)
    LOAD_CORE_FUNC(setInputState, set_input_state)
    LOAD_CORE_FUNC(setControllerPortDevice, set_controller_port_device)
    LOAD_CORE_FUNC(reset, reset)
    LOAD_CORE_FUNC(run, run)
    LOAD_CORE_FUNC(serializeSize, serialize_size)
    LOAD_CORE_FUNC(serialize, serialize)
    LOAD_CORE_FUNC(unserialize, unserialize)
    LOAD_CORE_FUNC(cheatReset, cheat_reset)
    LOAD_CORE_FUNC(cheatSet, cheat_set)
    LOAD_CORE_FUNC(loadGame, load_game)
    LOAD_CORE_FUNC(loadGameSpecial, load_game_special)
    LOAD_CORE_FUNC(unloadGame, unload_game)
    LOAD_CORE_FUNC(getRegion, get_region)
    LOAD_CORE_FUNC(getMemoryData, get_memory_data)
    LOAD_CORE_FUNC(getMemorySize, get_memory_size)

#undef LOAD_CORE_FUNC
    return true;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// The Facade: plain C functions, callable from C#, Python, or anything else
// ---------------------------------------------------------------------------

// extern "C" turns off C++'s "name mangling" (C++ normally renames functions
// internally to support overloading, which makes them impossible to find by
// a plain string name from another language). Without this block, C# would
// not be able to find any of the functions below by name.
extern "C" {

// __declspec(dllexport) is the Windows way of saying "make this function
// visible from outside this .dll" - without it, the function would be
// compiled in but hidden, and C# could not see it at all.
#define REWIND_API __declspec(dllexport)

// Creates one "game session" and returns an opaque handle (just a raw
// address) to it. The caller (C#) doesn't know or care what's behind that
// address - it just passes it back into every other function below so we
// know *which* session it's talking about. This matters because lrcpp lets
// you run several cores at once, each with its own Frontend.
REWIND_API void* Rewind_Create() {
    RewindContext* ctx = new RewindContext();

    ctx->frontend.setLogger(&ctx->logger);
    ctx->frontend.setVideo(&ctx->video);
    ctx->frontend.setAudio(&ctx->audio);
    ctx->frontend.setInput(&ctx->input);

    return ctx;
}

// Every function below starts the same way: turn the raw "handle" (void*)
// that the caller gave us back into a real RewindContext* we can use. This
// is safe as long as the caller only ever passes back a handle that
// Rewind_Create() gave them earlier.
REWIND_API void Rewind_Destroy(void* handle) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);
    if (ctx == nullptr) return;

    if (ctx->gameLoaded) ctx->frontend.unloadGame();
    if (ctx->coreLoaded) ctx->frontend.unset();

    delete ctx;
}

REWIND_API bool Rewind_LoadCore(void* handle, char const* corePath) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);

    if (!ctx->dynlib.load(corePath)) {
        return false;
    }

    if (!loadCoreFunctions(ctx->dynlib, ctx->core)) {
        return false;
    }

    if (!ctx->frontend.setCore(&ctx->core)) {
        return false;
    }

    ctx->coreLoaded = true;
    return true;
}

REWIND_API bool Rewind_LoadGame(void* handle, char const* gamePath) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);

    // TODO: for cores where need_fullpath is false, we'll need to read the
    // file into memory ourselves and use the (path, data, size) overload
    // instead. FCEUmm (our first target) needs a real path, so this is
    // enough for now.
    if (!ctx->frontend.loadGame(gamePath)) {
        return false;
    }

    ctx->gameLoaded = true;
    return true;
}

REWIND_API bool Rewind_RunFrame(void* handle) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);
    ctx->audio.buffer.clear(); // keep only this frame's audio around
    return ctx->frontend.run();
}

REWIND_API bool Rewind_GetVideoFrame(void* handle, uint8_t const** data, int* width, int* height, int* pitch, int* pixelFormat) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);
    MemoryVideo const& v = ctx->video;

    if (v.buffer.empty()) {
        return false;
    }

    *data = v.buffer.data();
    *width = static_cast<int>(v.width);
    *height = static_cast<int>(v.height);
    *pitch = static_cast<int>(v.pitch);
    *pixelFormat = static_cast<int>(v.pixelFormat);
    return true;
}

REWIND_API bool Rewind_GetAudioFrames(void* handle, int16_t const** data, int* frameCount) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);
    MemoryAudio const& a = ctx->audio;

    *data = a.buffer.data();
    *frameCount = static_cast<int>(a.buffer.size() / 2); // stereo
    return true;
}

REWIND_API void Rewind_SetButton(void* handle, int port, int buttonId, bool pressed) {
    RewindContext* ctx = static_cast<RewindContext*>(handle);
    if (port < 0 || port >= (int)MemoryInput::MAX_PORTS) return;
    if (buttonId < 0 || buttonId >= (int)MemoryInput::MAX_BUTTONS) return;
    ctx->input.buttons[port][buttonId] = pressed;
}

} // extern "C"
