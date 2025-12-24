# Makefile for Space Invaders MVC with SDL2

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -I./src -I./src/core -I./src/controller -I./src/views -I./src/utils
LDFLAGS = 
LIBS = 

# Détection du système
UNAME_S := $(shell uname -s)

# Configuration pour Linux avec SDL2
ifeq ($(UNAME_S),Linux)
	CFLAGS += $(shell pkg-config --cflags sdl2)
	LIBS += $(shell pkg-config --libs sdl2) -lm
endif

# Configuration pour macOS avec SDL2
ifeq ($(UNAME_S),Darwin)
	CFLAGS += $(shell sdl2-config --cflags)
	LIBS += $(shell sdl2-config --libs) -lm
endif

# Fichiers source
SRC_DIR = src
CORE_SRC = $(SRC_DIR)/core/model.c $(SRC_DIR)/core/game_state.c
CONTROLLER_SRC = $(SRC_DIR)/controller/controller.c $(SRC_DIR)/controller/input_handler.c
VIEWS_SRC = $(SRC_DIR)/views/view_sdl.c
UTILS_SRC = $(SRC_DIR)/utils/config.c $(SRC_DIR)/utils/logger.c
MAIN_SRC = $(SRC_DIR)/main.c

SOURCES = $(CORE_SRC) $(CONTROLLER_SRC) $(VIEWS_SRC) $(UTILS_SRC) $(MAIN_SRC)

# Fichiers objets
OBJ_DIR = build
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Exécutable
TARGET = space_invaders

# Règles
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Exécution
run: $(TARGET)
	./$(TARGET)

# Debug
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Valgrind
valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Installation des dépendances (Ubuntu/Debian)
install-deps-ubuntu:
	sudo apt-get update
	sudo apt-get install -y build-essential libsdl2-dev

# Installation des dépendances (Arch)
install-deps-arch:
	sudo pacman -S --needed base-devel sdl2

# Installation des dépendances (macOS avec Homebrew)
install-deps-macos:
	brew install sdl2

.PHONY: all clean run debug valgrind install-deps-ubuntu install-deps-arch install-deps-macos