# Makefile for Space Invaders MVC with SDL2, SDL2_ttf and NCURSES
# Simple version - no advanced features

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -g  # Added -g for valgrind debugging
LDFLAGS = 
LIBS = 

# Directories
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
CONTROLLER_DIR = $(SRC_DIR)/controller
VIEWS_DIR = $(SRC_DIR)/views
UTILS_DIR = $(SRC_DIR)/utils
OBJ_DIR = build
BIN_DIR = .

# Include paths
INCLUDES = -I./src -I$(CORE_DIR) -I$(CONTROLLER_DIR) -I$(VIEWS_DIR) -I$(UTILS_DIR)
CFLAGS += $(INCLUDES)

# Detect operating system
UNAME_S := $(shell uname -s 2>/dev/null || echo Linux)

# Platform-specific configuration
ifeq ($(UNAME_S),Linux)
    SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf 2>/dev/null || echo -I/usr/include/SDL2)
    SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf 2>/dev/null || echo -lSDL2 -lSDL2_image -lSDL2_ttf) -lm
    NCURSES_LIBS = -lncurses -lm
    PLATFORM = Linux
endif

ifeq ($(UNAME_S),Darwin)
    SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null)
    SDL_LIBS = $(shell sdl2-config --libs 2>/dev/null) -lSDL2_image -lSDL2_ttf -lm
    NCURSES_LIBS = -lncurses -lm
    PLATFORM = macOS
endif

# Source files (ONLY the ones that exist)
CORE_SRC = $(CORE_DIR)/model.c $(CORE_DIR)/game_state.c
CONTROLLER_SRC = $(CONTROLLER_DIR)/controller.c $(CONTROLLER_DIR)/input_handler.c

# Utils - only include files that actually exist
UTILS_SRC = $(wildcard $(UTILS_DIR)/*.c)
# Filter out any platform_*.c files and standalone tools with main()
UTILS_SRC := $(filter-out $(UTILS_DIR)/platform_%.c, $(UTILS_SRC))
UTILS_SRC := $(filter-out $(UTILS_DIR)/generate_spaceship.c, $(UTILS_SRC))
UTILS_SRC := $(filter-out $(UTILS_DIR)/create_assets.c, $(UTILS_SRC))
UTILS_SRC := $(filter-out $(UTILS_DIR)/create_saucer.c, $(UTILS_SRC))

# Separate font_manager for SDL only
FONT_MANAGER_SRC = $(UTILS_DIR)/font_manager.c
# Remove font_manager from UTILS_SRC since we'll handle it separately
UTILS_SRC := $(filter-out $(FONT_MANAGER_SRC), $(UTILS_SRC))

# Views and mains
MAIN_SDL_SRC = $(SRC_DIR)/main_sdl.c
MAIN_NCURSES_SRC = $(SRC_DIR)/main_ncurses.c

SDL_VIEWS_SRC = $(VIEWS_DIR)/view_sdl.c
NCURSES_VIEWS_SRC = $(VIEWS_DIR)/view_ncurses.c

# Combined sources
SDL_SOURCES = $(CORE_SRC) $(CONTROLLER_SRC) $(SDL_VIEWS_SRC) $(UTILS_SRC) $(FONT_MANAGER_SRC) $(MAIN_SDL_SRC)
NCURSES_SOURCES = $(CORE_SRC) $(CONTROLLER_SRC) $(NCURSES_VIEWS_SRC) $(UTILS_SRC) $(MAIN_NCURSES_SRC)

# Object files
SDL_OBJECTS = $(SDL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/sdl/%.o)
NCURSES_OBJECTS = $(NCURSES_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/ncurses/%.o)

# Target executables
SDL_TARGET = $(BIN_DIR)/space_invaders_sdl
NCURSES_TARGET = $(BIN_DIR)/space_invaders_ncurses

# Standalone tools (with their own main functions)
TOOLS_SRC = $(wildcard $(UTILS_DIR)/generate_spaceship.c $(UTILS_DIR)/create_assets.c $(UTILS_DIR)/create_saucer.c)
TOOLS_TARGETS = $(TOOLS_SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%)

# Default target
.DEFAULT_GOAL := help

# Help target
.PHONY: help
help:
	@echo "Space Invaders MVC - Build System"
	@echo "Platform: $(PLATFORM)"
	@echo "SDL2 with TTF support enabled"
	@echo ""
	@echo "Targets:"
	@echo "  make all                - Build both versions"
	@echo "  make sdl                - Build SDL version (with TTF)"
	@echo "  make ncurses            - Build ncurses version"
	@echo "  make tools              - Build utility tools"
	@echo "  make run-sdl            - Build and run SDL"
	@echo "  make run-ncurses        - Build and run ncurses"
	@echo "  make clean              - Remove build files"
	@echo "  make valgrind-sdl       - Run valgrind on SDL version"
	@echo "  make valgrind-ncurses   - Run valgrind on ncurses version"
	@echo "  make install-deps       - Install dependencies"
	@echo "  make info               - Show build information"

# Build all
.PHONY: all
all: $(SDL_TARGET) $(NCURSES_TARGET)
	@echo "Build complete"

# Build SDL
.PHONY: sdl
sdl: $(SDL_TARGET)

# Build ncurses
.PHONY: ncurses
ncurses: $(NCURSES_TARGET)

# Build utility tools
.PHONY: tools
tools: $(TOOLS_TARGETS)
	@echo "Utility tools built"

# SDL executable
$(SDL_TARGET): CFLAGS += $(SDL_CFLAGS) -DUSE_SDL_VIEW
$(SDL_TARGET): $(SDL_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking SDL (with TTF)..."
	$(CC) $(SDL_OBJECTS) -o $@ $(LDFLAGS) $(SDL_LIBS)
	@echo "Built: $@"

# ncurses executable
$(NCURSES_TARGET): CFLAGS += -DUSE_NCURSES_VIEW
$(NCURSES_TARGET): $(NCURSES_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking ncurses..."
	$(CC) $(NCURSES_OBJECTS) -o $@ $(LDFLAGS) $(NCURSES_LIBS)
	@echo "Built: $@"

# Utility tools (standalone programs)
$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	@echo "Building tool: $<"
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $< -o $@ $(LDFLAGS) $(SDL_LIBS)

# Compile SDL objects
$(OBJ_DIR)/sdl/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling (SDL): $<"
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -DUSE_SDL_VIEW -c $< -o $@

# Compile ncurses objects
$(OBJ_DIR)/ncurses/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling (ncurses): $<"
	$(CC) $(CFLAGS) -DUSE_NCURSES_VIEW -c $< -o $@

# Run targets
.PHONY: run-sdl
run-sdl: $(SDL_TARGET)
	@echo "Running SDL..."
	@echo "Note: Make sure fonts/venite-adoremus-font contains font files"
	./$(SDL_TARGET)

.PHONY: run-ncurses
run-ncurses: $(NCURSES_TARGET)
	@echo "Running ncurses..."
	TERM=xterm-256color ./$(NCURSES_TARGET)

# Clean
.PHONY: clean
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR)
	rm -f $(SDL_TARGET) $(NCURSES_TARGET)
	rm -f $(TOOLS_TARGETS)
	rm -f valgrind-sdl-report.txt valgrind-ncurses-report.txt
	@echo "Clean complete"

# Valgrind targets
.PHONY: valgrind-sdl
valgrind-sdl: $(SDL_TARGET)
	@echo "Running valgrind on SDL..."
	@echo "Note: Run the game and press ESC quickly to exit"
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --error-exitcode=0 \
	         --log-file=valgrind-sdl-report.txt \
	         ./$(SDL_TARGET)
	@echo ""
	@echo "Valgrind report saved to valgrind-sdl-report.txt"
	@echo "Memory leak summary:"
	@grep "ERROR SUMMARY" valgrind-sdl-report.txt || echo "No error summary found"

.PHONY: valgrind-ncurses
valgrind-ncurses: $(NCURSES_TARGET)
	@echo "Running valgrind on ncurses..."
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --error-exitcode=0 \
	         --log-file=valgrind-ncurses-report.txt \
	         TERM=xterm-256color ./$(NCURSES_TARGET)
	@echo "ncurses valgrind report: valgrind-ncurses-report.txt"

.PHONY: valgrind
valgrind: valgrind-sdl valgrind-ncurses
	@echo "Both valgrind reports generated"

# Install dependencies
.PHONY: install-deps-ubuntu
install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libncurses5-dev valgrind

.PHONY: install-deps-arch
install-deps-arch:
	sudo pacman -S --needed base-devel sdl2 sdl2_image sdl2_ttf ncurses valgrind

.PHONY: install-deps-macos
install-deps-macos:
	brew install sdl2 sdl2_image sdl2_ttf ncurses

.PHONY: install-deps
install-deps:
ifeq ($(PLATFORM),Linux)
	@if [ -f /etc/debian_version ]; then \
		$(MAKE) install-deps-ubuntu; \
	elif [ -f /etc/arch-release ]; then \
		$(MAKE) install-deps-arch; \
	else \
		echo "Unknown Linux distribution. Please install manually:"; \
		echo "  - build-essential"; \
		echo "  - libsdl2-dev"; \
		echo "  - libsdl2-image-dev"; \
		echo "  - libsdl2-ttf-dev"; \
		echo "  - libncurses5-dev"; \
		echo "  - valgrind"; \
	fi
else ifeq ($(PLATFORM),macOS)
	$(MAKE) install-deps-macos
else
	@echo "Unknown platform: $(PLATFORM)"
	@echo "Please install dependencies manually."
endif

# Assets and fonts preparation
.PHONY: prepare-assets
prepare-assets:
	@echo "Checking assets and fonts..."
	@if [ ! -d "assets" ]; then \
		echo "Creating assets directory..."; \
		mkdir -p assets; \
		echo "Note: PNG assets are no longer required for title/game over screens"; \
	fi
	@if [ ! -d "fonts/venite-adoremus-font" ]; then \
		echo "WARNING: Custom fonts directory does not exist"; \
		echo "The game will use fallback text rendering"; \
		echo "Please place your font files in fonts/venite-adoremus-font/"; \
	fi
	@echo ""
	@echo "Required font files (at least one):"
	@echo "  - VeniteAdoremus-rgRBA.ttf"
	@echo "  - VeniteAdoremusStraight-Yzo6v.ttf"
	@echo "  - Aa VeniteAdoremus-rgRBA.ttf"
	@echo "  - Aa VeniteAdoremusStraight-Yzo6v.ttf"

.PHONY: info
info:
	@echo "Configuration:"
	@echo "  Platform: $(PLATFORM)"
	@echo "  CC: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  SDL libs: $(SDL_LIBS)"
	@echo "  ncurses libs: $(NCURSES_LIBS)"
	@echo "  Custom fonts enabled for SDL: YES"
	@echo ""
	@echo "Sources:"
	@echo "  Core: $(CORE_SRC)"
	@echo "  Controller: $(CONTROLLER_SRC)"
	@echo "  Utils (common): $(UTILS_SRC)"
	@echo "  Font Manager (SDL only): $(FONT_MANAGER_SRC)"
	@echo "  Tools (standalone): $(TOOLS_SRC)"
	@echo ""
	@echo "SDL objects: $(words $(SDL_OBJECTS)) files"
	@echo "ncurses objects: $(words $(NCURSES_OBJECTS)) files"

.PHONY: all clean help sdl ncurses tools run-sdl run-ncurses valgrind-sdl valgrind-ncurses valgrind install-deps info prepare-assets