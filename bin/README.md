# Space Invaders MVC - High-Performance Game Engine

Space Invaders MVC is a robust, modular implementation of the classic arcade title, engineered in ISO C11 with a focus on strict Model-View-Controller (MVC) architectural principles. The project features a dual-rendering system supporting both high-performance graphical output (SDL3) and terminal-based ASCII rendering (Ncurses), while maintaining a hardware-independent core logic.

## Technical Specifications

### Architecture
- **Strict MVC Separation**: The core logic (Model) is entirely decoupled from rendering (View) and input processing (Controller).
- **Logical Resolution**: The game engine operates on a fixed logical grid of 600x600 units, ensuring consistent physics and collision detection regardless of display scale.
- **Polymorphic Rendering**: Abstract view interface allowing seamless switching between SDL3 and Ncurses.
- **Memory Management**: Zero-leak policy enforced through rigorous Valgrind analysis and customized suppression profiles for system-level drivers.

### Core Features
- **Deterministic Physics**: Frame-rate independent movement and collision systems.
- **Advanced AI Patterns**: Tiered enemy behaviors including "Bullet Hell" mechanics in high-difficulty modes.
- **Dynamic Audio Engine**: Real-time procedural audio synthesis and mixing via the `miniaudio` library.
- **Expansion Support**: Modular entity system for easy addition of new projectile types, power-ups, and boss mechanics.

## System Prerequisites

### Core Development Tools
- **Compiler**: GCC 11.0+ or Clang 13.0+ (requires C11 standard support).
- **Build System**: GNU Make 4.0+.
- **Analysis Tools**: Valgrind (for memory profiling), Doxygen (for documentation).

### Libraries
| Library | Minimum Version | Purpose |
|---------|-----------------|---------|
| SDL3 | 3.0.0+ | Graphical Rendering, Event Handling |
| SDL3_image | 3.0.0+ | Asset Loading (PNG, JPG) |
| SDL3_ttf | 3.0.0+ | TrueType Font Rendering |
| Ncurses | 6.0+ | Terminal Graphics |
| Check | 0.15.0+ | Unit Testing Framework |
| Miniaudio | Internal | Audio Backend (Header-only) |

## Installation and Dependency Management

### Linux Distribution Packages

#### Debian / Ubuntu / Mint
```bash
sudo apt update
sudo apt install build-essential libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev \
                 libncurses5-dev check valgrind doxygen graphviz pkg-config
```

#### Arch Linux
```bash
sudo pacman -S base-devel sdl3 sdl3_image sdl3_ttf ncurses check \
               valgrind doxygen graphviz pkg-config
```

#### Fedora
```bash
sudo dnf install gcc make SDL3-devel SDL3_image-devel SDL3_ttf-devel \
                 ncurses-devel check-devel valgrind doxygen graphviz pkg-config
```

## Build System Integration

The project utilizes a comprehensive Makefile to manage the build lifecycle.

### Standard Build Targets
| Target | Description |
|--------|-------------|
| `make all` | Compiles SDL and Ncurses versions, and all auxiliary tools. |
| `make sdl` | Builds the graphical version (`bin/space_invaders_sdl`). |
| `make ncurses` | Builds the terminal version (`bin/space_invaders_ncurses`). |
| `make tools` | Compiles specialized asset generation and testing tools. |
| `make clean` | Removes all build artifacts, binaries, and temporary files. |

### Execution Targets
| Target | Description |
|--------|-------------|
| `make run-sdl` | Compiles and executes the SDL3 version. |
| `make run-ncurses` | Compiles and executes the Ncurses version. |
| `make test` | Executes the unit test suite via the Check framework. |

### Advanced Verification
| Target | Description |
|--------|-------------|
| `make fullcheck` | Performs a comprehensive audit: Clean build -> Tests -> Memory Check -> Style Check. |
| `make valgrind-report` | Generates detailed memory leak reports in the `reports/` directory. |
| `make doc` | Generates Doxygen documentation in `docs/html/`. |
| `make debug` | Compiles with AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan). |
| `make release` | Compiles with full optimizations (`-O3`) and link-time optimization (`-flto`). |

## Project Structure

```text
.
├── src/                # Implementation Source Code
│   ├── core/           # Model: Physics, AI, State Management
│   ├── views/          # View: SDL3 and Ncurses renderers
│   ├── controller/     # Controller: Input handling and Command mapping
│   ├── utils/          # Cross-platform utilities and Font management
│   └── main_*.c        # Executable entry points
├── tests/              # Unit tests and Mock environments
├── tools/              # Static asset generators and development scripts
├── bin/                # Compiled binaries and runtime assets
├── assets/             # Raw media resources
└── docs/               # Generated technical documentation
```

## Operational Manual

### Controls Configuration
The game supports runtime keybinding modification via the Settings menu.

| Action | Primary (SDL) | Secondary (SDL) | Ncurses Default |
|--------|---------------|-----------------|-----------------|
| Move Left | Left Arrow | A | Left Arrow |
| Move Right | Right Arrow | D | Right Arrow |
| Fire | Space | Left Shift | Space |
| Pause | P | P | P |
| Exit/Back | ESC | ESC | Q |

### Entity Difficulty Scaling
- **Level 1-3**: Standard movement and firing cooldowns.
- **Level 4+ (Boss Phase)**: Activation of the Mothership. This entity features high health, phase-based attack patterns, and tracks player proximity.
- **Hard/Rogue Mode**: Activates "Bullet Hell" protocols where invaders ignore standard column firing restrictions.

## Troubleshooting and FAQ

**1. SDL3 Header Not Found**
Ensure `PKG_CONFIG_PATH` includes the directory containing `sdl3.pc`. On some systems, SDL3 must be compiled from source if not available in the default repositories.

**2. Missing Assets at Runtime**
Execute binaries from the project root or use `make prepare-assets` to ensure the `bin/` directory is synchronized with the latest resources.

**3. Memory Report Errors**
System-level leaks (e.g., within NVIDIA or Mesa drivers) are filtered via `valgrind.supp`. Only report leaks originating from the `src/` directory.

---
© 2026 Academic Research Project. Developed under the MIT license.
Original Space Invaders concept owned by Taito Corporation.
