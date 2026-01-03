# Video Script: Space Invaders MVC Technical Showcase
**Target Duration:** 10 Minutes
**Audience:** Technical Reviewers / Developers
**Tone:** Professional / Academic

---

## 1. Introduction (0:00 - 1:00)
**[Visual: Title Screen - Terminal & SDL side-by-side]**

> "Welcome to a technical deep dive into Space Invaders MVC. This project is more than just a game; it's a demonstration of high-performance C11 programming, strict architectural separation, and extreme portability."

**[Visual: Project Tree - Highlighting src/core, src/views, src/controller]**

> "Today we'll explore how we built a cross-platform engine that moves from a high-speed 3D warp-field in SDL3 to a low-latency ASCII interface in the terminal, all powered by the exact same logical core."

---

## 2. Gameplay & Difficulty Mechanics (1:00 - 3:30)
**[Visual: SDL Gameplay - Level 1]**

> "The core loop runs on a hardware-independent 600x600 logical grid. This ensures that whether you're playing at 144 FPS or over a slow SSH connection, the physics, collision detection, and enemy AI remain consistent."

**[Visual: Switching to HARD mode]**

> "Our 'Bullet Hell' difficulty mode showcases the engine's entity management. In Hard mode, invaders ignore standard column firing limits, requiring the model to process hundreds of active projectiles with zero performance drop."

**[Visual: LEVEL 4 BOSS - The Mothership]**

> "At Level 4, the Mothership appears—a multi-phase boss that tracks player positioning and utilizes procedural attack patterns. Notice the smooth transitions and color-swapped health phases, managed through a modular boss-state machine."

---

## 3. Architecture: The 'Two Mains' & MVC (3:30 - 5:30)
**[Visual: Code - main_sdl.c and main_ncurses.c side-by-side]**

> "A crucial design choice was the implementation of 'The Two Mains'. Unlike monolithic engines, the project uses separate entry points: `main_sdl.c` and `main_ncurses.c`."

**[Visual: Code - view_base.h showing function pointers or interface]**

> "Both mains share the same Controller and Model instances, but initialize different View providers. This illustrates true polymorphism in C. We don't just 'toggle' graphics; we swap the entire rendering engine while the game state remains untouched in its own memory space."

**[Visual: Sequence Diagram overlay showing the update loop]**

> "Input flows through the Controller, commands are executed on the Model, and the View reads the state for the next frame. No circular dependencies, no shared global state."

---

## 4. Technical Innovation: Audio & Miniaudio (5:30 - 7:30)
**[Visual: Code - platform_sdl.c or audio loading code]**

> "One of our most significant technical pivots was our approach to audio. Initially, we considered SDL_mixer, but encountered a common industrial hurdle: version mismatch and dependency hell between SDL3 and its auxiliary libraries."

**[Visual: Text overlay - why miniaudio?]**

> "Because we couldn't find a stable, compatible SDL_mixer version for all targeted environments, we transitioned to `miniaudio.h`. This single-header library allows us to bypass external dependencies while giving us lower-level control over sound buffers and real-time volume modulation, which you see used in our settings menu."

---

## 5. Portability: The 3rdParty Strategy (7:30 - 9:00)
**[Visual: Explorer - 3rdParty directory]**

> "To ensure this project works 'out of the box' on any Linux machine, we implemented a self-contained dependency strategy. The `3rdParty` directory contains the full source for SDL3, SDL_image, and SDL_ttf."

**[Visual: Makefile - scrolling through the check-sdl-deps and LOCAL_SDL_DIR sections]**

> "The Makefile is intelligent. It first checks for system-installed libraries. If they are missing or incompatible, it automatically triggers a local build within the project directory. This isolation means the project remains functional whether the host system has SDL installed or not."

---

## 6. Verification: Tests & Memory Safety (9:00 - 9:50)
**[Visual: Terminal - `make test` running]**

> "Quality is enforced through the Check unit testing framework. Every critical logical component—from projectile collision to difficulty scaling—is verified before every commit."

**[Visual: Terminal - `make valgrind-report` output]**

> "Finally, we maintain a strict 'Zero Leak' policy. By using a custom suppression file for system-level driver leaks, we can confirm that 100% of our game logic memory is safely managed, ensuring long-term stability without crashes."

---

## 7. Conclusion (9:50 - 10:00)
**[Visual: Final Montage of SDL and Ncurses Gameplay]**

> "Space Invaders MVC is a testament to the power of clean architecture and proactive technical choices. From embedded dependencies to procedural audio, it’s built for performance, portability, and professional standards. Thank you for watching."

---

**[End Screen: Link to GitHub / Repository]**
