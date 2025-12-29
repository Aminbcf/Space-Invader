# Space Invaders MVC - Build System (SDL3)
# Platform: Linux
# SDL3 with TTF and Image support enabled

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11 -D_GNU_SOURCE -I./src -I./src/controller -I./src/core -I./src/utils -I./src/views

# AUTOMATIC CONFIGURATION
# Finds libraries automatically using pkg-config
SDL_CFLAGS = $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf)
SDL_LDFLAGS = $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf) -lm

NCURSES_LDFLAGS = -lncurses -lm
TEST_LDFLAGS = -lcheck -lm -lpthread -lrt

# Directories
SRC_DIR = src
BUILD_DIR = build
SDL_BUILD_DIR = $(BUILD_DIR)/sdl
NCURSES_BUILD_DIR = $(BUILD_DIR)/ncurses
TEST_BUILD_DIR = $(BUILD_DIR)/tests
BIN_DIR = bin

# Common source files (shared between versions)
COMMON_SRCS = \
	$(SRC_DIR)/controller/controller.c \
	$(SRC_DIR)/controller/input_handler.c \
	$(SRC_DIR)/core/game_state.c \
	$(SRC_DIR)/core/model.c \
	$(SRC_DIR)/utils/font_manager.c

# Common header files (for dependency tracking)
COMMON_HDRS = \
	$(SRC_DIR)/controller/commands.h \
	$(SRC_DIR)/controller/controller.h \
	$(SRC_DIR)/controller/input_handler.h \
	$(SRC_DIR)/core/game_state.h \
	$(SRC_DIR)/core/model.h \
	$(SRC_DIR)/utils/font_manager.h \
	$(SRC_DIR)/utils/platform.h \
	$(SRC_DIR)/views/rect_utils.h \
	$(SRC_DIR)/views/view_base.h

# SDL-specific source files
SDL_SRCS = \
	$(COMMON_SRCS) \
	$(SRC_DIR)/utils/platform_sdl.c \
	$(SRC_DIR)/views/view_sdl.c \
	$(SRC_DIR)/main_sdl.c

# ncurses-specific source files
NCURSES_SRCS = \
	$(COMMON_SRCS) \
	$(SRC_DIR)/views/view_ncurses.c \
	$(SRC_DIR)/main_ncurses.c

# Test source files
TEST_SRCS = \
	$(SRC_DIR)/tests/test_model.c

# Convert source files to object files
SDL_OBJS = $(patsubst $(SRC_DIR)/%, $(SDL_BUILD_DIR)/%, $(SDL_SRCS:.c=.o))
NCURSES_OBJS = $(patsubst $(SRC_DIR)/%, $(NCURSES_BUILD_DIR)/%, $(NCURSES_SRCS:.c=.o))
TEST_OBJS = $(patsubst $(SRC_DIR)/%, $(TEST_BUILD_DIR)/%, $(TEST_SRCS:.c=.o))

# Executables
SDL_EXEC = $(BIN_DIR)/space_invaders_sdl
NCURSES_EXEC = $(BIN_DIR)/space_invaders_ncurses
TEST_EXEC = $(BIN_DIR)/test_runner

.PHONY: all sdl ncurses tools run-sdl run-ncurses run-tests clean \
	valgrind-sdl valgrind-ncurses valgrind-tests install-deps \
	info prepare-assets check-style check-memory leak-check \
	doc generate-docs install uninstall dist package \
	help check-project rebuild debug release profile \
	check-sdl-deps

# Default target
all: sdl ncurses tools

# Build SDL version
sdl: $(SDL_EXEC)

# Build ncurses version
ncurses: $(NCURSES_EXEC)

# Run SDL version
run-sdl: sdl
	@echo "Running SDL version..."
	@$(SDL_EXEC)

# Run ncurses version
run-ncurses: ncurses
	@echo "Running ncurses version..."
	@$(NCURSES_EXEC)

# Run tests
run-tests: $(TEST_EXEC)
	@echo "Running tests..."
	@$(TEST_EXEC)

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete."

# Rule to build SDL executable
$(SDL_EXEC): $(SDL_OBJS) | $(BIN_DIR)
	@echo "Building SDL executable..."
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $^ -o $@ $(SDL_LDFLAGS)
	@echo "SDL executable built: $@"

# Rule to build ncurses executable
$(NCURSES_EXEC): $(NCURSES_OBJS) | $(BIN_DIR)
	@echo "Building ncurses executable..."
	$(CC) $(CFLAGS) $^ -o $@ $(NCURSES_LDFLAGS)
	@echo "ncurses executable built: $@"

# Rule to build test executable
$(TEST_EXEC): $(TEST_OBJS) $(filter-out %/main_sdl.o %/main_ncurses.o %/view_sdl.o %/view_ncurses.o, $(NCURSES_OBJS)) | $(BIN_DIR)
	@echo "Building test runner..."
	$(CC) $(CFLAGS) $^ -o $@ $(TEST_LDFLAGS)
	@echo "Test runner built: $@"

# Rule to compile SDL .c files to .o files
$(SDL_BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(COMMON_HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -DUSE_SDL_VIEW -c $< -o $@

# Rule to compile ncurses .c files to .o files
$(NCURSES_BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(COMMON_HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile test .c files to .o files
$(TEST_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(BIN_DIR):
	@mkdir -p $@

# Check SDL dependencies
check-sdl-deps:
	@echo "Checking SDL3 dependencies..."
	@pkg-config --exists sdl3 sdl3-image sdl3-ttf || { \
		echo "ERROR: SDL3 dependencies not found in pkg-config."; \
		echo "Please ensure PKG_CONFIG_PATH is correct."; \
		exit 1; \
	}
	@echo "SDL3 dependencies OK."

# Help message
help:
	@echo "Space Invaders MVC - Build System (SDL3)"
	@echo ""
	@echo "Available targets:"
	@echo "  make all          - Build both SDL and ncurses versions"
	@echo "  make sdl          - Build SDL version"
	@echo "  make ncurses      - Build ncurses version"
	@echo "  make clean        - Clean all build files"
	@echo "  make check-sdl-deps - Check SDL3 dependencies"

# Default target when running just 'make'
.DEFAULT_GOAL := help