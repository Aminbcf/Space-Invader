# Space Invaders MVC

A modern, robust C implementation of the classic arcade game *Space Invaders*, built with a strict **Model-View-Controller (MVC)** architecture. This project demonstrates high-quality C programming practices, including memory safety, modular design, and dual-interface support (Graphical & Terminal).

## 🚀 Features

### Core Architectural Features
- **Strict MVC Pattern**: Complete separation between Logic (Model), Input (Controller), and Rendering (View).
- **Dual Interface**:
    - **SDL3**: High-performance graphical view with sprites, particles, and 3D warp effects.
    - **Ncurses**: ASCII-art terminal view playable over SSH.
- **Shared Model**: Both interfaces run on the exact same game logic engine.
- **Memory Safety**: Verified "Zero Leaks" with Valgrind.

### Gameplay Features
- **Advanced Boss Fight**: Level 4 features the "Dreadnought" boss with multi-phase attacks and changing visual states.
- **Dynamic Difficulty**:
    - **Easy**: 3 Levels, Slower enemies.
    - **Normal**: 4 Levels, Standard speed, Boss fight.
    - **Hard**: Faster enemies (1.5x), Aggressive firing, Double Score Points.
- **Procedural Audio**: Real-time synthesized sound effects and music (managed via `miniaudio`).
- **High Scores**: Persistent score tracking.
- **Customizable Controls**: Full keybinding remapping for both SDL and Terminal views.

---

## 🛠️ Installation

### Prerequisites

| Component | Requirement |
|-----------|-------------|
| Compiler | GCC or Clang (C11 support) |
| Build System | GNU Make |
| Graphics | SDL3, SDL3_image, SDL3_ttf |
| Terminal | Ncurses |
| Audio | Miniaudio (Included) |

### Linux Dependencies

**Debian / Ubuntu:**
```bash
sudo apt update
sudo apt install build-essential libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libncurses5-dev valgrind doxygen
```
*Note: If SDL3 is not yet in your repo, you may need to build it from source or use a PPA.*

**Arch Linux:**
```bash
sudo pacman -S base-devel sdl3 sdl3_image sdl3_ttf ncurses valgrind doxygen
```

**Fedora:**
```bash
sudo dnf install gcc make SDL3-devel SDL3_image-devel SDL3_ttf-devel ncurses-devel valgrind doxygen
```

### Building the Project

The `Makefile` handles the entire build process.

```bash
# Clone the repository
git clone https://github.com/your-repo/space-invaders-mvc.git
cd space-invaders-mvc

# Build everything (SDL + Ncurses + Tools)
make all

# Build only SDL version
make sdl

# Build only Terminal version
make ncurses
```

---

## 🎮 Gameplay Manual

### Controls

| Action | SDL Key | Ncurses Key |
|--------|---------|-------------|
| **Move Left** | Left Arrow / A | Left Arrow / A |
| **Move Right** | Right Arrow / D | Right Arrow / D |
| **Shoot** | Space | Space |
| **Pause** | P | P |
| **Confirm** | Enter | Enter |
| **Back/Quit** | ESC | Q |

> **Note**: You can customize these keys in **Settings > Controls**.

### Enemies & Scoring

| Enemy Type | Visual (SDL) | Visual (Ncurses) | Points (Normal) | Points (Hard - 2x) |
|------------|--------------|------------------|-----------------|--------------------|
| **Squid** | Small, Top Row | `^` | 30 | 60 |
| **Crab** | Medium, Middle | `M` | 20 | 40 |
| **Octopus** | Large, Bottom | `W` | 10 | 20 |
| **Saucer** | Red Flying Ship | `<=>` | 100 - 300 | 200 - 600 |
| **Boss** | Dreadnought | `[BOSS]` | 5000 | 10,000 |

### The Boss (Dreadnought)
Appears at Level 4 (Normal/Hard).
- **Phase 1**: Green shield, slow tracking.
- **Phase 2**: Purple shield, rapid fire, aggressive tracking.
- **Strategy**: Keep moving. The boss anticipates your position. Destroying it triggers the Victory screen.

---

## 🏗️ Architecture Deep Dive

### Project Structure
```
.
├── src/
│   ├── core/           # MODEL: Game Logic (Physics, AI, State)
│   ├── views/          # VIEW: Rendering code (SDL & Ncurses)
│   ├── controller/     # CONTROLLER: Input mapping & Event handling
│   ├── assets/         # Raw assets (images, sounds)
│   └── main_*.c        # Entry points
├── bin/                # Executables & Runtime assets
├── build/              # Computed object files
└── tests/              # Unit tests (Check framework)
```

### The MVC Loop
1.  **Input**: User presses a key.
2.  **Controller**: `input_handler.c` captures the raw keycode and maps it to a semantic `Command` (e.g., `CMD_SHOOT`).
3.  **Model**: `model.c` processes the command, updates game state (entity positions, collisions). **The Model knows nothing about the View.**
4.  **View**: `view_sdl.c` or `view_ncurses.c` reads the updated Model state and renders the frame.

### Key Files
-   `src/core/model.h`: The "Single Source of Truth". Defines `GameModel`, `Player`, `Invader` structs.
-   `src/views/view_base.h`: Defines the polymorphic `View` interface (`render`, `cleanup`).
-   `src/controller/controller.c`: The bridge that executes commands on the model.

---

## 🔧 Troubleshooting

**1. "Error initialization SDL" / "Window closes immediately"**
*   Check that `bin/assets/` exists and contains images/sounds.
*   Run `make install-deps` (if script available) or verify SDL3 installation.

**2. "Assets not found"**
*   Ensure you run the game from the project root, or that the executables in `bin/` can find the `assets/` folder relative to them.
*   The Makefile copies assets automatically. Try `make clean && make all`.

**3. Audio Latency / Glitches**
*   This uses `miniaudio.h`. If you are on Linux with PulseAudio/PipeWire, it should work out of the box.
*   If using WSL, audio configurations can be tricky.

---

## 📜 License

This project is an academic reimplementation for educational purposes.
Original Space Invaders concept © Taito Corporation.
Code licensed under MIT.
