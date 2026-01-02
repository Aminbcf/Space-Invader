# Space Invaders MVC

A modern, robust C implementation of the classic arcade game *Space Invaders*, built with a strict **Model-View-Controller (MVC)** architecture. This project demonstrates high-quality C programming practices, including memory safety, modular design, and dual-interface support (Graphical & Terminal).

## 🚀 Features

### Core Architectural Features
- **Strict MVC Pattern**: Complete separation between Logic (Model), Input (Controller), and Rendering (View).
- **Dual Interface**:
- **Shared Model**: Both interfaces run on the exact same game logic engine.
- **Memory Safety**: Verified "Zero Leaks" with Valgrind.

### Gameplay Features
- **MOTHERSHIP Boss Fight**: Level 4 features the colossal "MOTHERSHIP" boss with adaptive attack patterns and a visual health bar.
- **Bullet Hell Difficulty**: 
    - **HARD/ROGUE**: Enemies use advanced firing patterns, launching projectiles from all columns simultaneously ("Bullet Hell").
    - **Dynamic Shot Types**: Invaders fire different projectiles (ZigZag, Orbs, Lasers) based on their row rank.
- **Improved HUD & Grid**: A dedicated 600px game grid with a visual vertical separator and safe HUD zones.
- **Dual Interface**:
    - **SDL3**: High-performance graphical view with radial 3D warp stars, particles, and smooth animations.
    - **Ncurses**: ASCII-art terminal view playable over SSH.
- **Customizable Controls**: Full keybinding remapping for both SDL and Terminal views (supports 2-player mode).
- **Procedural Audio**: Real-time synthesized sound effects and music (managed via `miniaudio`).
- **High Scores**: Persistent score tracking.

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

| Action | Player 1 (SDL) | Player 2 (SDL) | Ncurses |
|--------|----------------|----------------|---------|
| **Move Left** | Left Arrow | A | Left Arrow |
| **Move Right**| Right Arrow | D | Right Arrow |
| **Shoot**     | Space | Left Shift | Space |
| **Pause**     | P | P | P |
| **Menu/Back** | ESC | ESC | Q |

> **Note**: Remix your controls in **Settings > Controls**.

### Enemies & Scoring

| Enemy Type | Visual (SDL) | Visual (Ncurses) | Basis Points |
|------------|--------------|------------------|--------------|
| **Squid**  | Small (Top)  | `^` | 30 |
| **Crab**   | Medium (Mid) | `M` | 20 |
| **Octopus**| Large (Bot)  | `W` | 10 |
| **Saucer** | Flying Ship  | `<=>` | 100 - 300 |
| **MOTHERSHIP**| Dreadnought | `[BOSS]` | 5000 |

### The MOTHERSHIP
Appears at Level 4 (Normal+ Difficulty).
- **Targeting**: The Mothership tracks player positions and launches heavy barrage attacks.
- **Bullet Hell**: On HARD, the Mothership is supported by continuous invader fire from the entire grid.
- **Weak Point**: Its center core. Watch the HP bar above the grid!

---

## 🏗️ Architecture Deep Dive

### The 600x600 Game Grid
The game uses a logical coordinate system restricted to a **600px wide area**.
- **0px - 600px**: Active Game Area (Enemies, Player, Saucer).
- **600px - 800px**: HUD/Sidebar (Scores, Lives, Power-up Timers).
- **Vertical Line**: A gray separator clearly demarcates the "Grid" from the "Interface".

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
