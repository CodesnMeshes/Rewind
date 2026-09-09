# Contributing to Rewind

Thanks for your interest in contributing! Rewind is very early-stage, so the most
valuable contributions right now are:

- Bug reports and reproduction steps
- Feedback on the architecture (see [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md))
- Testing the native prototype on your own machine/toolchain
- Small, focused pull requests

## Getting the code

Rewind vendors [lrcpp](https://github.com/leiradel/lrcpp) as a git submodule, so clone
with `--recursive`:

```bash
git clone --recursive https://github.com/codesbygom/rewind.git
```

Already cloned without it?

```bash
git submodule update --init --recursive
```

## Getting SDL2 (prototype only)

The current native prototype uses SDL2 for a quick video/audio/input test harness. SDL2
isn't vendored in this repo — download the Windows development libraries from
[libsdl.org](https://www.libsdl.org/) (the version currently used is 2.30.9) and extract
them into `third-party/sdl2/`, so you end up with `third-party/sdl2/SDL2-2.30.9/`.

SDL2 will go away once the native engine no longer needs a display of its own — see
[`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

## Getting a libretro core to test with

You need a compiled libretro core (a `.dll` on Windows) to actually load and run a game.
Prebuilt cores are available from the
[libretro buildbot](https://buildbot.libretro.com/nightly/). `FCEUmm` (NES) is the core
this project has been validated against.

Place the core under `test-assets/cores/`. Test ROMs (legally distributable ones, like
homebrew test ROMs) go under `test-assets/roms/`.

## Building

```bash
tools\build.bat
```

This builds the SDL2 prototype (`native/RewindCore.cpp` + the sample harness). The build
process will change as the C# app comes online — check back here or watch the repo for
updates.

## Code style

- C++: match the style already in `src/engine/` — favor clarity over cleverness.
- C#: standard .NET conventions.
- Keep commits focused and write descriptive commit messages.

## Reporting bugs / requesting features

Please use the issue templates when opening a new issue — they help make sure we get the
information needed to act on it.

## License

By contributing, you agree that your contributions will be licensed under the project's
[GPL-3.0 license](LICENSE).
