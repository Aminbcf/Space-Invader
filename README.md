# Space Invaders MVC

A modern C implementation of the classic Space Invaders arcade game using strict Model-View-Controller architecture.

## Features

### Game Features
- **Menu System**: Full-featured menu with difficulty selection and settings
- **Three Difficulty Levels**:
  - **Easy**: 3 levels, no boss fight
  - **Normal**: 4 levels with boss fight
  - **Hard**: 1.5x faster enemies, more bullets, 2x score multiplier
- **Score Bonuses**: Difficulty-based multipliers (Easy 1x, Normal 1.5x, Hard 2x)
- **Progressive Levels**: 4 levels with increasing difficulty
- **Boss Fight**: Epic level 4 boss battle with special music
- **Audio System**: Multiple music tracks and sound effects
  - Menu/gameplay music
  - Boss battle music
  - Victory music
  - Shooting, explosion, damage, and game over sounds
- **High Score Tracking**: Persistent high score save system

### Technical Features
- **Strict MVC Architecture**: Clean separation of concerns
- **Dual Interface**: SDL3 (graphics) and ncurses (terminal)
- **Shared Model**: Same game logic for both interfaces
- **Memory Safe**: Valgrind-verified, zero leaks
- **Unit Tested**: 11 passing unit tests
- **Modular Design**: Easy to extend and maintain

## Quick Start

### Installation

```bash
# Install dependencies (auto-detects your package manager)
make install-deps

# Or manually for Arch Linux:
sudo pacman -S sdl3 sdl3_ttf sdl3_image ncurses check valgrind doxygen
```

### Compilation & Running

```bash
# Compile and run SDL version
make run-sdl

# Compile and run terminal version
make run-ncurses

# Run tests
make test

# Build everything
make all
```

## Controls

### SDL Version
- **Movement**: Arrow keys or A/D
- **Shoot**: Space
- **Menu Navigation**: Arrow keys, Enter to select
- **Volume Adjust**: Left/Right arrows (in settings)
- **Pause**: P
- **Return to Menu**: ESC
- **Quit**: ESC (from main menu)

### ncurses Version
- **Movement**: A (left), D (right)
- **Shoot**: Space
- **Pause**: P
- **Quit**: Q

## Architecture

```
src/
├── core/           # Model (game logic)
│   ├── model.c/h
│   └── game_state.c/h
├── views/          # View (SDL + ncurses)
│   ├── view_sdl.c/h
│   └── view_ncurses.c/h
├── controller/     # Controller (input handling)
│   ├── controller.c/h
│   └── input_handler.c/h
└── utils/          # Platform abstraction
    ├── platform.h
    └── font_manager.c/h
```

## Difficulty System

| Feature | Easy | Normal | Hard |
|---------|------|--------|------|
| **Levels** | 3 | 4 | 4 |
| **Boss Fight** | ❌ | ✅ | ✅ |
| **Enemy Speed** | 1.0x | 1.0x | 1.5x |
| **Bullet Frequency** | Normal | Normal | 1.5x |
| **Player Speed** | Normal | Normal | 1.3x |
| **Score Multiplier** | 1.0x | 1.5x | 2.0x |

### Score Examples (Hard Difficulty)
- Invader kill: 30 → **60 points** (2x)
- Saucer: 200-300 → **400-600 points** (2x)
- Boss defeat: 5,000 → **10,000 points** (2x)

## Audio System

- **Menu/Gameplay**: `music_game.wav` - Energetic E minor, 130 BPM
- **Boss Fight**: `music_boss.wav` - Intense battle music
- **Victory**: `music_victory.wav` - Triumphant fanfare
- **Sound Effects**: Shooting, explosions, damage, game over

Music automatically transitions between game states and loops continuously.

## Development

### Testing
```bash
make test              # Run unit tests
make valgrind-sdl      # Memory leak check (SDL)
make valgrind-ncurses  # Memory leak check (ncurses)
make fullcheck         # Complete validation
```

### Documentation
```bash
make doc  # Generate Doxygen documentation
```

### Code Style
```bash
make check-style  # Verify code style
make format       # Auto-format code
```

## Building

### Makefile Targets

**Compilation**:
- `make all` - Build SDL, ncurses, and tools
- `make sdl` - Build SDL version only
- `make ncurses` - Build ncurses version only
- `make rebuild` - Clean and rebuild everything

**Running**:
- `make run-sdl` - Compile and launch SDL version
- `make run-ncurses` - Compile and launch ncurses version
- `make run-tests` - Compile and run tests

**Testing**:
- `make test` - Run unit tests  
- `make valgrind-sdl` - Memory check SDL
- `make valgrind-ncurses` - Memory check ncurses
- `make check-memory` - Complete memory analysis

**Documentation**:
- `make doc` - Generate documentation
- `make help` - Show all available targets

**Utilities**:
- `make clean` - Remove build files
- `make install` - Install to system
- `make dist` - Create distribution package

## Requirements

- **C Compiler**: GCC 7.0+ (C11 support)
- **SDL3**: libsdl3-dev, libsdl3-ttf-dev, libsdl3-image-dev
- **ncurses**: libncurses-dev
- **Check**: Unit testing framework
- **Make**: Build system
- **Optional**: Valgrind, Doxygen, clang-format

## Project Structure

```
.
├── src/                # Source code
│   ├── core/          # Game model
│   ├── views/         # SDL + ncurses views
│   ├── controller/    # Input handling
│   ├── utils/         # Utilities
│   ├── assets/        # Audio files, images
│   └── main_*.c       # Entry points
├── tests/             # Unit tests
├── bin/               # Compiled executables
├── build/             # Object files
├── docs/              # Generated documentation
├── Makefile           # Build system
└── README.md          # This file
```

## Menu System

### Main Menu
- **START**: Opens difficulty selection
- **SETTINGS**: Configure volume and view controls
- **QUIT**: Exit game

### Settings
- **CONTROLS**: View all key bindings
- **MUSIC VOLUME**: Adjust volume (0-100%) with arrow keys
- **BACK**: Return to main menu

### Difficulty Selection
Choose between Easy, Normal, or Hard before starting the game.

## Return to Menu

After winning or losing, press **SPACE** to return to the main menu (not restart). From there you can:
- Choose a different difficulty
- Adjust settings
- View your high score
- Quit the game

## Memory Safety

All code is Valgrind-checked with zero memory leaks:
```bash
make valgrind-report  # Generate complete memory report
```

## License

Academic project - see project documentation for details.

## Author

**Amine Boucif**  
Space Invaders MVC - C Implementation

---

**Built with**: C11, SDL3, ncurses, MVC architecture  
**Tested with**: Valgrind, Check framework  
**Documented with**: Doxygen
