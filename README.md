<div align="center">

# Rewind

**One emulator. Every console.**

Rewind is a cross-platform, all-in-one retro game emulator front-end. Instead of juggling
a separate emulator for every console, Rewind loads [libretro](https://www.libretro.com/)
cores — the same emulation cores used by RetroArch — inside a single, unified,
zero-config interface.

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Status](https://img.shields.io/badge/status-pre--alpha-orange.svg)](#project-status)
[![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20macOS-lightgrey.svg)](#roadmap)
[![Support this project](https://img.shields.io/badge/%E2%9D%A4-Support%20this%20project-e25555.svg)](https://gomworks.github.io/donate)
[![Releases](https://img.shields.io/badge/releases-none%20yet-lightgrey.svg)](#releases)

[Features](#features) •
[Status](#project-status) •
[Roadmap](#roadmap) •
[Releases](#releases) •
[Building](#building-from-source) •
[Architecture](docs/ARCHITECTURE.md) •
[Contributing](CONTRIBUTING.md)

</div>

---

## Why Rewind?

Most retro emulation front-ends fall into one of two camps: single-console emulators
that only do one system well, or big all-in-one suites like RetroArch that come with a
steep learning curve and a lot of configuration. Rewind aims for something closer to
[Lemuroid](https://github.com/Swordfish90/Lemuroid) — drop in a ROM, hit play, done —
but as a native desktop *and* mobile app.

## Features

- 🎮 **One frontend, many consoles** — powered by [libretro](https://www.libretro.com/)
  cores, the same emulation backends used by RetroArch.
- 🖥️ **Native, not Electron** — the UI is built with [.NET MAUI](https://dotnet.microsoft.com/apps/maui),
  giving Rewind a genuinely native look and feel on every platform it targets.
- 🧩 **No core soup** — Rewind talks to libretro cores directly through
  [lrcpp](https://github.com/leiradel/lrcpp) instead of shelling out to a separate
  emulator process.
- 🎯 **Zero-config first** — sensible defaults, minimal setup, so you can go from "add
  ROM" to "playing" in seconds.

## Project status

Rewind is in **early, pre-alpha development**. The libretro↔C++ integration has been
validated end-to-end (loading a real NES core, running a test ROM, video/audio/input all
working), and the native engine is currently being wrapped for consumption from C#. There
is no installable build yet — see [Roadmap](#roadmap) and follow the repo for progress.

For a deep dive into *why* the project is built the way it is (and the alternatives that
were ruled out along the way), see [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

## Roadmap

| Phase | Target | Systems | Status |
|---|---|---|---|
| 1 | Desktop (Windows, macOS) | NES, SNES, Game Boy, Genesis, Atari 2600 | 🔨 In progress |
| 2 | Desktop | PS1, N64, PSP (GPU-accelerated cores) | ⏳ Planned |
| 3 | Mobile (Android, iOS) | Phase 1 systems | ⏳ Planned |

Systems like the original Xbox, GameCube, Saturn, or Dreamcast are out of scope — they're
either too heavy for a lightweight frontend or not realistically supported by the
libretro ecosystem.

> **Linux?** Not on the roadmap above since [.NET MAUI](https://dotnet.microsoft.com/apps/maui)
> (the primary UI framework) doesn't target it. If Linux support gets built, it'll be a
> separate Avalonia-based UI head sharing the same core — see
> [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md#ui-framework--linux-strategy).

## Built with

- **[libretro](https://www.libretro.com/)** — the emulation core API/ecosystem.
- **[lrcpp](https://github.com/leiradel/lrcpp)** — a C++ wrapper around the libretro C
  API, vendored here as a git submodule.
- **[.NET MAUI](https://dotnet.microsoft.com/apps/maui)** — the cross-platform, native UI framework for
  the desktop and mobile app.
- **C# / .NET** — application logic and UI layer.

## Building from source

> Rewind doesn't have a stable build yet — these steps build the current native
> prototype and will change as the project matures.

```bash
git clone --recursive https://github.com/GOMWorks/Rewind.git
cd Rewind
```

If you already cloned without `--recursive`, fetch the submodule with:

```bash
git submodule update --init --recursive
```

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for toolchain requirements (SDL2, a libretro
core to test against, etc.) and how to run the current prototype.

## Releases

🚧 **No release yet.** Rewind is still pre-alpha (see [Project status](#project-status))
— there is nothing installable to download yet. Watch or star the repo to get notified
the moment the first build goes up on the
[Releases page](https://github.com/GOMWorks/Rewind/releases).

## Support this project

Rewind is free, open-source, and built in spare time. If it's useful to you, consider
[supporting its development](https://gomworks.github.io/donate) ❤️

## License

Rewind is licensed under the [GNU General Public License v3.0](LICENSE), in line with
most of the libretro/RetroArch ecosystem it builds on.
