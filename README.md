# Rewind

Rewind is an all-in-one, cross-platform retro game emulator. Instead of juggling a separate emulator for every console, Rewind uses the libretro API to load emulator "cores" for many different systems inside a single, unified frontend — one app, one interface, any console.

## Built With

Rewind's emulation core is built on the [libretro](https://www.libretro.com/) API — the same interface used by RetroArch — which lets it load and run libretro emulator "cores" for a wide range of retro consoles without reimplementing per-system emulation from scratch.

To work with the libretro API from C++, Rewind uses [lrcpp](https://github.com/leiradel/lrcpp), a C++ wrapper around the libretro C API that also provides a composable class for loading and driving libretro cores.

The desktop UI is built with [Avalonia](https://avaloniaui.net/), giving Rewind a native cross-platform interface across Windows, macOS, and Linux.
