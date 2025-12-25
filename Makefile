# Makefile for Space Invaders MVC with SDL2 and NCURSES
# Simple version - no advanced features

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
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
    SDL_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null || echo -I/usr/include/SDL2)
    SDL_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null || echo -lSDL2) -lm
    NCURSES_LIBS = -lncurses -lm
    PLATFORM = Linux
endif

ifeq ($(UNAME_S),Darwin)
    SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null)
    SDL_LIBS = $(shell sdl2-config --libs 2>/dev/null) -lm
    NCURSES_LIBS = -lncurses -lm
    PLATFORM = macOS
endif

# Source files (ONLY the ones that exist)
CORE_SRC = $(CORE_DIR)/model.c $(CORE_DIR)/game_state.c
CONTROLLER_SRC = $(CONTROLLER_DIR)/controller.c $(CONTROLLER_DIR)/input_handler.c

# Utils - only include files that actually exist
UTILS_SRC = $(wildcard $(UTILS_DIR)/*.c)
# Filter out any platform_*.c files
UTILS_SRC := $(filter-out $(UTILS_DIR)/platform_%.c, $(UTILS_SRC))

MAIN_SDL_SRC = $(SRC_DIR)/main_sdl.c
MAIN_NCURSES_SRC = $(SRC_DIR)/main_ncurses.c

SDL_VIEWS_SRC = $(VIEWS_DIR)/view_sdl.c
NCURSES_VIEWS_SRC = $(VIEWS_DIR)/view_ncurses.c

# Combined sources
SDL_SOURCES = $(CORE_SRC) $(CONTROLLER_SRC) $(SDL_VIEWS_SRC) $(UTILS_SRC) $(MAIN_SDL_SRC)
NCURSES_SOURCES = $(CORE_SRC) $(CONTROLLER_SRC) $(NCURSES_VIEWS_SRC) $(UTILS_SRC) $(MAIN_NCURSES_SRC)

# Object files
SDL_OBJECTS = $(SDL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/sdl/%.o)
NCURSES_OBJECTS = $(NCURSES_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/ncurses/%.o)

# Target executables
SDL_TARGET = $(BIN_DIR)/space_invaders_sdl
NCURSES_TARGET = $(BIN_DIR)/space_invaders_ncurses

# Default target
.DEFAULT_GOAL := help

# Help target
.PHONY: help
help:
	@echo "Space Invaders MVC - Build System"
	@echo "Platform: $(PLATFORM)"
	@echo ""
	@echo "Targets:"
	@echo "  make all          - Build both versions"
	@echo "  make sdl          - Build SDL version"
	@echo "  make ncurses      - Build ncurses version"
	@echo "  make run-sdl      - Build and run SDL"
	@echo "  make run-ncurses  - Build and run ncurses"
	@echo "  make clean        - Remove build files"
	@echo "  make valgrind     - Run valgrind on SDL"

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

# SDL executable
$(SDL_TARGET): CFLAGS += $(SDL_CFLAGS) -DUSE_SDL_VIEW
$(SDL_TARGET): $(SDL_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking SDL..."
	$(CC) $(SDL_OBJECTS) -o $@ $(LDFLAGS) $(SDL_LIBS)
	@echo "Built: $@"

# ncurses executable
$(NCURSES_TARGET): CFLAGS += -DUSE_NCURSES_VIEW
$(NCURSES_TARGET): $(NCURSES_OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking ncurses..."
	$(CC) $(NCURSES_OBJECTS) -o $@ $(LDFLAGS) $(NCURSES_LIBS)
	@echo "Built: $@"

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
	./$(SDL_TARGET)

.PHONY: run-ncurses
run-ncurses: $(NCURSES_TARGET)
	@echo "Running ncurses..."
	./$(NCURSES_TARGET)

# Clean
.PHONY: clean
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR)
	rm -f $(SDL_TARGET) $(NCURSES_TARGET)
	rm -f valgrind-report.txt
	@echo "Clean complete"

# Valgrind
.PHONY: valgrind
valgrind: $(SDL_TARGET)
	@echo "Running valgrind..."
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --log-file=valgrind-report.txt \
	         ./$(SDL_TARGET)
	@echo "Report: valgrind-report.txt"

# Install dependencies
.PHONY: install-deps-ubuntu
install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev libncurses5-dev valgrind

.PHONY: install-deps-arch
install-deps-arch:
	sudo pacman -S --needed base-devel sdl2 ncurses valgrind

.PHONY: install-deps-macos
install-deps-macos:
	brew install sdl2 ncurses

.PHONY: install-deps
install-deps:
ifeq ($(PLATFORM),Linux)
	@if [ -f /etc/debian_version ]; then \
		$(MAKE) install-deps-ubuntu; \
	elif [ -f /etc/arch-release ]; then \
		$(MAKE) install-deps-arch; \
	fi
else ifeq ($(PLATFORM),macOS)
	$(MAKE) install-deps-macos
endif

.PHONY: info
info:
	@echo "Configuration:"
	@echo "  Platform: $(PLATFORM)"
	@echo "  CC: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  SDL libs: $(SDL_LIBS)"
	@echo "  ncurses libs: $(NCURSES_LIBS)"
	@echo ""
	@echo "Sources:"
	@echo "  Core: $(CORE_SRC)"
	@echo "  Controller: $(CONTROLLER_SRC)"
	@echo "  Utils: $(UTILS_SRC)"

.PHONY: all clean help sdl ncurses run-sdl run-ncurses valgrind install-deps info