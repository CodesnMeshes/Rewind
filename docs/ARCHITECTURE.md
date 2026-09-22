# Architecture

## Why this architecture?

### What we ruled out, and why

- **Writing an emulation core from scratch** — unnecessary. Real, community-maintained
  cores already exist for almost every system (e.g. FCEUmm for the NES, Snes9x for the
  SNES) via the libretro ecosystem.
- **Shelling out to RetroArch as an external process** — this worked, but wasn't clean:
  two separate windows, an external dependency, and a disjointed experience. It also
  ruled out true zero-config.
- **Existing C# bindings (LibRetriX)** — either abandoned (last updated 2018) or tied to
  a specific framework (Unity, UWP).

### What we chose

**[lrcpp](https://github.com/leiradel/lrcpp)** — a small, MIT-licensed, actively
maintained C++ library built specifically for writing libretro frontends. Rewind wraps
it in a thin C# layer.

## UI framework & Linux strategy

The desktop/mobile UI is built with **[.NET MAUI](https://dotnet.microsoft.com/apps/maui)**,
which reaches Windows, macOS, Android, and iOS — but has no Linux target and none is
planned by Microsoft.

If/when Linux support is worth doing, the plan is **not** to try to stretch MAUI onto it.
Instead:

- All application logic, state, and view-models stay in a UI-agnostic shared project (no
  MAUI types leak into it).
- MAUI is the View layer for Windows/macOS/mobile.
- A separate **[Avalonia](https://avaloniaui.net/)**-based View layer — which does support
  Linux natively — would be added as a second UI head, binding to the same shared
  view-models.

This was chosen over adopting [Uno Platform](https://platform.uno/) as a MAUI replacement,
and over Uno's `.NET MAUI Embedding` (which only re-exposes MAUI *controls* inside an Uno
host on the platforms MAUI already reaches — it does not bring MAUI itself to Linux).
Keeping MAUI as the primary framework and adding Avalonia only where Linux is actually
needed keeps the primary platforms on the more mainstream, better-supported toolkit
without giving up a real path to Linux later.

## Core concepts (for reference)

### What is a "core"?

A compiled binary (a `.dll` on Windows) that implements roughly twenty functions with a
fixed name and signature (`retro_init`, `retro_run`, `retro_load_game`, …). A core has no
idea what application is hosting it — it only knows this contract. That's exactly what
lets any core run inside any compatible frontend.

### The environment function — the heart of it all

Instead of dozens of separate functions, a core exposes a **single** function called
`environment`. Every question it has — pixel format, filesystem paths, rumble support,
etc. — goes through this one door, tagged with a number that identifies the question.
The frontend's job is to answer these questions correctly. This is where most of the
real difficulty of writing a frontend lives: there are a lot of possible questions, and
getting the answers right matters.

### How does a single frame run?

When `run()` is called (all of this happens before it returns):

1. The core asks the frontend to poll input right now (`input_poll`).
2. It starts simulating a frame.
3. Whenever it needs to know if a button is pressed, it asks (`input_state`).
4. Once a frame is ready, it hands it to the frontend (`video_refresh`).
5. It hands over audio the same way (`audio_sample` / `audio_sample_batch`).
6. `run()` returns.

The frontend is the active caller; the core is passive and responds to callbacks that
were registered ahead of time.

### The main pieces of lrcpp

| Piece | Role |
|---|---|
| `Core` (struct) | Just a table of function pointers into the core; we jump into it via `GetProcAddress` after loading `core.dll`. |
| `Frontend` (class) | The lifecycle manager: `setCore`, `loadGame`, `run`, `reset`, `serialize`/`unserialize` (save states), `cheatSet`, `getMemoryData`. |
| Components (Video, Audio, Input, Logger, Config, …) | Base classes we derive from to implement the behavior we want (drawing frames, playing audio, reading input). |
| `CoreFsm` | An internal state machine that prevents calling functions in the wrong order (e.g. `run()` before `loadGame()`). |

## Project progress

- ✅ **Initial validation** — the official lrcpp SDL2 sample was built with MSVC and
  successfully ran a real NES core (`FCEUmm`) against a legal test ROM (`nestest.nes`):
  256×240 video, audio, and input all worked correctly.
- 🔨 **In progress** — writing a thin C# shell (`native/RewindCore.cpp`) that, instead of
  drawing directly to the screen like the SDL2 sample did, keeps the latest video/audio
  frame in memory and exposes it to C# through a handful of simple `extern "C"`
  functions.

## Console scope

| Phase | Systems | Notes |
|---|---|---|
| Phase 1 (starting point) | NES, SNES, Game Boy, Genesis, Atari 2600 | Software-rendered, no GPU required. |
| Phase 2 (later) | PS1, N64, PSP | Needs accelerated graphics rendering (OpenGL). |
| Out of scope | Original Xbox, GameCube, Saturn, Dreamcast | Too heavy, or not realistically part of the libretro ecosystem. |

## Folder structure

```
Rewind/
├── src/
│   ├── engine/          ← our C++ code (the bridge to libretro)
│   └── app/             ← the C# application (later)
├── third-party/
│   ├── lrcpp/           ← external library (the core engine), vendored as a submodule
│   └── sdl2/            ← only for early prototyping (not committed, see CONTRIBUTING.md)
├── test-assets/
│   ├── cores/           ← test cores (fceumm_libretro.dll, and more later)
│   └── roms/            ← legal test ROMs
├── tools/
│   └── build.bat        ← build script for the SDL2 prototype
└── build/               ← compiled output (generated)
```

## Tools used

- **lrcpp** — the libretro integration engine (MIT).
- **SDL2** — used only for the early prototype; the final app uses .NET MAUI (Avalonia
  for a Linux UI head, if/when that's built — see "UI framework & Linux strategy" above).
- **.NET / C#** — the UI and application-logic layer.
