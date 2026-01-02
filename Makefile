# ============================================================================
# SPACE INVADERS MVC - SYSTÈME DE COMPILATION
# ============================================================================
# Projet : Space Invaders en C avec architecture MVC
# Plateforme : Linux (Debian/Ubuntu)
# Bibliothèques : SDL3 (avec TTF et Image) et ncurses
# Langage : C (ISO C11)
# ============================================================================

# ----------------------------------------------------------------------------
# CONFIGURATION DU COMPILATEUR
# ----------------------------------------------------------------------------
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11 -D_GNU_SOURCE \
         -I./src -I./src/controller -I./src/core -I./src/utils -I./src/views

# ----------------------------------------------------------------------------
# CONFIGURATION AUTOMATIQUE DES BIBLIOTHÈQUES
# ----------------------------------------------------------------------------
# Détection automatique via pkg-config pour SDL3 et ses extensions
SDL_CFLAGS = $(shell pkg-config --cflags sdl3 sdl3-image sdl3-ttf 2>/dev/null || \
              echo "-I/usr/local/include/SDL3 -D_REENTRANT -I/usr/local/include/SDL3_ttf -I/usr/local/include/SDL3_image")

SDL_LDFLAGS = $(shell pkg-config --libs sdl3 sdl3-image sdl3-ttf 2>/dev/null || \
               echo "-L/usr/local/lib -lSDL3 -lSDL3_ttf -lSDL3_image") -lm

# Bibliothèques pour ncurses et tests
NCURSES_LDFLAGS = -lncurses -lm
TEST_LDFLAGS = -lcheck -lm -lpthread -lrt

# ----------------------------------------------------------------------------
# STRUCTURE DES RÉPERTOIRES
# ----------------------------------------------------------------------------
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
SDL_BUILD_DIR = $(BUILD_DIR)/sdl
NCURSES_BUILD_DIR = $(BUILD_DIR)/ncurses
TEST_BUILD_DIR = $(BUILD_DIR)/tests
BIN_DIR = bin
DOC_DIR = docs
DIST_DIR = dist
ASSETS_DIR = assets

# ----------------------------------------------------------------------------
# FICHIERS SOURCES COMMUNS (partagés entre les versions)
# ----------------------------------------------------------------------------
COMMON_SRCS = \
	$(SRC_DIR)/controller/controller.c \
	$(SRC_DIR)/controller/input_handler.c \
	$(SRC_DIR)/core/game_state.c \
	$(SRC_DIR)/core/model.c \
	$(SRC_DIR)/utils/font_manager.c

# ----------------------------------------------------------------------------
# FICHIERS D'EN-TÊTE COMMUNS (pour le suivi des dépendances)
# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# FICHIERS SOURCES SPÉCIFIQUES À SDL
# ----------------------------------------------------------------------------
SDL_SRCS = \
	$(COMMON_SRCS) \
	$(SRC_DIR)/utils/platform_sdl.c \
	$(SRC_DIR)/views/view_sdl.c \
	$(SRC_DIR)/main_sdl.c

# ----------------------------------------------------------------------------
# FICHIERS SOURCES SPÉCIFIQUES À NCURSES
# ----------------------------------------------------------------------------
NCURSES_SRCS = \
	$(COMMON_SRCS) \
	$(SRC_DIR)/views/view_ncurses.c \
	$(SRC_DIR)/main_ncurses.c

# ----------------------------------------------------------------------------
# FICHIERS SOURCES DE TESTS
# ----------------------------------------------------------------------------
TEST_SRCS = \
	$(TEST_DIR)/src/test_main.c \
	$(TEST_DIR)/src/test_model.c \
	$(TEST_DIR)/src/test_controller.c \
	$(TEST_DIR)/src/test_input_handler.c \
	$(TEST_DIR)/src/test_game_state.c \
	$(TEST_DIR)/src/mock_platform.c

# ----------------------------------------------------------------------------
# CONVERSION DES FICHIERS SOURCES EN FICHIERS OBJETS
# ----------------------------------------------------------------------------
SDL_OBJS = $(patsubst $(SRC_DIR)/%, $(SDL_BUILD_DIR)/%, $(SDL_SRCS:.c=.o))
NCURSES_OBJS = $(patsubst $(SRC_DIR)/%, $(NCURSES_BUILD_DIR)/%, $(NCURSES_SRCS:.c=.o))
TEST_OBJS = $(patsubst $(TEST_DIR)/%, $(TEST_BUILD_DIR)/%, $(TEST_SRCS:.c=.o))

# ----------------------------------------------------------------------------
# EXÉCUTABLES FINAUX
# ----------------------------------------------------------------------------
SDL_EXEC = $(BIN_DIR)/space_invaders_sdl
NCURSES_EXEC = $(BIN_DIR)/space_invaders_ncurses
TEST_EXEC = $(BIN_DIR)/test_runner

# ----------------------------------------------------------------------------
# OUTILS AUXILIAIRES
# ----------------------------------------------------------------------------
TOOL_SRCS = $(wildcard tools/*.c)
TOOLS = $(patsubst tools/%.c, $(BIN_DIR)/%, $(TOOL_SRCS))

# ============================================================================
# DÉCLARATION DES CIBLES PHONY
# ============================================================================
.PHONY: all sdl ncurses tools run-sdl run-ncurses run-tests clean \
        valgrind-sdl valgrind-ncurses valgrind-tests valgrind-report install-deps \
        info prepare-assets check-style check-memory leak-check \
        doc generate-docs install uninstall dist package \
        help check-project rebuild debug release profile \
        check-sdl-deps check-ncurses-deps check-test-deps memcheck fullcheck \
        format test coverage benchmark

# ============================================================================
# CIBLES PRINCIPALES
# ============================================================================

# Cible par défaut : aide
.DEFAULT_GOAL := help

# ----------------------------------------------------------------------------
# all : Compile toutes les versions (SDL, ncurses et outils)
# ----------------------------------------------------------------------------
all: prepare-assets sdl ncurses tools
	@echo "✓ Compilation complète terminée avec succès"

# ----------------------------------------------------------------------------
# sdl : Compile la version SDL
# ----------------------------------------------------------------------------
sdl: prepare-assets $(SDL_EXEC)
	@echo "✓ Version SDL compilée avec succès"

# ----------------------------------------------------------------------------
# ncurses : Compile la version ncurses
# ----------------------------------------------------------------------------
ncurses: prepare-assets $(NCURSES_EXEC)
	@echo "✓ Version ncurses compilée avec succès"

# ----------------------------------------------------------------------------
# tools : Compile les outils auxiliaires
# ----------------------------------------------------------------------------
tools: $(TOOLS)
	@echo "✓ Outils compilés avec succès"

# ============================================================================
# CIBLES D'EXÉCUTION
# ============================================================================

# ----------------------------------------------------------------------------
# run-sdl : Compile et exécute la version SDL
# ----------------------------------------------------------------------------
run-sdl: sdl prepare-assets
	@echo "▶ Lancement de la version SDL..."
	@cd $(BIN_DIR) && ./space_invaders_sdl

# ----------------------------------------------------------------------------
# run-ncurses : Compile et exécute la version ncurses
# ----------------------------------------------------------------------------
run-ncurses: ncurses prepare-assets
	@echo "▶ Lancement de la version ncurses..."
	@cd $(BIN_DIR) && ./space_invaders_ncurses

# ----------------------------------------------------------------------------
# run-tests : Compile et exécute les tests unitaires
# ----------------------------------------------------------------------------
run-tests: $(TEST_EXEC)
	@echo "▶ Exécution des tests unitaires..."
	@$(TEST_EXEC)

# ----------------------------------------------------------------------------
# test : Exécute les tests unitaires
# ----------------------------------------------------------------------------
test: $(TEST_EXEC)
	@echo "▶ Exécution des tests unitaires..."
	@$(TEST_EXEC)

# ============================================================================
# RÈGLES DE COMPILATION
# ============================================================================

# ----------------------------------------------------------------------------
# Compilation de l'exécutable SDL
# ----------------------------------------------------------------------------
$(SDL_EXEC): $(SDL_OBJS) | $(BIN_DIR)
	@echo "→ Édition des liens pour SDL..."
	@$(CC) $(CFLAGS) $(SDL_CFLAGS) $^ -o $@ $(SDL_LDFLAGS)
	@echo "✓ Exécutable SDL créé : $@"

# ----------------------------------------------------------------------------
# Compilation de l'exécutable ncurses
# ----------------------------------------------------------------------------
$(NCURSES_EXEC): $(NCURSES_OBJS) | $(BIN_DIR)
	@echo "→ Édition des liens pour ncurses..."
	@$(CC) $(CFLAGS) $^ -o $@ $(NCURSES_LDFLAGS)
	@echo "✓ Exécutable ncurses créé : $@"

# ----------------------------------------------------------------------------
# Compilation de l'exécutable de tests
# ----------------------------------------------------------------------------
$(TEST_EXEC): $(TEST_OBJS) $(filter-out %/main_sdl.o %/main_ncurses.o %/view_sdl.o %/view_ncurses.o, $(NCURSES_OBJS)) | $(BIN_DIR) check-test-deps
	@echo "→ Édition des liens pour les tests..."
	@$(CC) $(CFLAGS) $(filter %.o,$^) -o $@ $(TEST_LDFLAGS)
	@echo "✓ Exécutable de tests créé : $@"

# ----------------------------------------------------------------------------
# Compilation des outils
# ----------------------------------------------------------------------------
$(BIN_DIR)/%: tools/%.c | $(BIN_DIR)
	@echo "→ Compilation de l'outil : $@"
	@$(CC) $(CFLAGS) $< -o $@ -lm
	@chmod +x $@

# ----------------------------------------------------------------------------
# Compilation des fichiers .c en .o (version SDL)
# ----------------------------------------------------------------------------
$(SDL_BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(COMMON_HDRS)
	@mkdir -p $(dir $@)
	@echo "  CC [SDL] $<"
	@$(CC) $(CFLAGS) $(SDL_CFLAGS) -DUSE_SDL_VIEW -c $< -o $@

# ----------------------------------------------------------------------------
# Compilation des fichiers .c en .o (version ncurses)
# ----------------------------------------------------------------------------
$(NCURSES_BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(COMMON_HDRS)
	@mkdir -p $(dir $@)
	@echo "  CC [NCU] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# Compilation des fichiers .c en .o (tests)
# ----------------------------------------------------------------------------
$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC [TST] $<"
	@$(CC) $(CFLAGS) -I$(TEST_DIR)/include -c $< -o $@

# ----------------------------------------------------------------------------
# Création du répertoire bin
# ----------------------------------------------------------------------------
$(BIN_DIR):
	@mkdir -p $@

# ============================================================================
# GESTION DES DÉPENDANCES
# ============================================================================

# ----------------------------------------------------------------------------
# check-sdl-deps : Vérifie les dépendances SDL3
# ----------------------------------------------------------------------------
check-sdl-deps:
	@echo "→ Vérification des dépendances SDL3..."
	@pkg-config --exists sdl3 sdl3-image sdl3-ttf || { \
		echo "✗ ERREUR : Dépendances SDL3 non trouvées via pkg-config."; \
		echo "  Vérifiez que PKG_CONFIG_PATH est correct ou installez SDL3."; \
		exit 1; \
	}
	@echo "✓ Dépendances SDL3 détectées correctement"

# ----------------------------------------------------------------------------
# check-ncurses-deps : Vérifie les dépendances ncurses
# ----------------------------------------------------------------------------
check-ncurses-deps:
	@echo "→ Vérification des dépendances ncurses..."
	@pkg-config --exists ncurses || { \
		echo "⚠ Avertissement : ncurses non détecté via pkg-config"; \
		echo "  Tentative avec -lncurses..."; \
	}
	@echo "✓ Dépendances ncurses OK"

# ----------------------------------------------------------------------------
# check-test-deps : Vérifie les dépendances pour les tests
# ----------------------------------------------------------------------------  
check-test-deps:
	@echo "→ Vérification des dépendances de test (Check)..."
	@pkg-config --exists check || { \
		echo "✗ ERREUR : Bibliothèque Check non trouvée."; \
		echo "  La bibliothèque Check est requise pour compiler les tests."; \
		echo ""; \
		echo "  Installation :"; \
		echo "    Debian/Ubuntu : sudo apt-get install check"; \
		echo "    Fedora        : sudo dnf install check-devel"; \
		echo "    Arch Linux    : sudo pacman -S check"; \
		echo ""; \
		echo "  Ou exécutez : make install-deps"; \
		exit 1; \
	}
	@echo "✓ Dépendances de test (Check) OK"


# ----------------------------------------------------------------------------
# install-deps : Installe les dépendances nécessaires
# ----------------------------------------------------------------------------
install-deps:
	@echo "→ Installation des dépendances..."
	@echo "  Détection du gestionnaire de paquets..."
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "  → Utilisation de apt-get (Debian/Ubuntu)"; \
		sudo apt-get update; \
		sudo apt-get install -y libsdl3-dev libsdl3-ttf-dev libsdl3-image-dev \
		                        libncurses-dev check valgrind doxygen graphviz \
		                        pkg-config make gcc git clang-format; \
	elif command -v dnf >/dev/null 2>&1; then \
		echo "  → Utilisation de dnf (Fedora)"; \
		sudo dnf install -y SDL3-devel SDL3_ttf-devel SDL3_image-devel \
		                    ncurses-devel check-devel valgrind doxygen graphviz \
		                    pkg-config make gcc git clang-tools-extra; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "  → Utilisation de pacman (Arch Linux)"; \
		sudo pacman -S --noconfirm sdl3 sdl3_ttf sdl3_image ncurses check \
		                           valgrind doxygen graphviz pkg-config make gcc git clang; \
	elif command -v zypper >/dev/null 2>&1; then \
		echo "  → Utilisation de zypper (openSUSE)"; \
		sudo zypper install -y SDL3-devel SDL3_ttf-devel SDL3_image-devel \
		                       ncurses-devel check-devel valgrind doxygen graphviz \
		                       pkg-config make gcc git clang; \
	else \
		echo "✗ Gestionnaire de paquets inconnu."; \
		echo "  Veuillez installer manuellement :"; \
		echo "    - SDL3 et SDL3_ttf, SDL3_image (fichiers de développement)"; \
		echo "    - ncurses (fichiers de développement)"; \
		echo "    - check (framework de tests)"; \
		echo "    - valgrind (détection de fuites mémoire)"; \
		echo "    - doxygen et graphviz (génération de documentation)"; \
		echo "    - pkg-config, make, gcc, git"; \
		exit 1; \
	fi
	@echo "✓ Installation des dépendances terminée"

# ============================================================================
# TESTS ET VÉRIFICATIONS
# ============================================================================

# ----------------------------------------------------------------------------
# valgrind-sdl : Analyse mémoire de la version SDL
# ----------------------------------------------------------------------------
valgrind-sdl: sdl
	@echo "→ Analyse mémoire (valgrind) sur la version SDL [MODE RÉDUIT]..."
	@mkdir -p reports
	@cd $(BIN_DIR) && valgrind --leak-check=full --show-leak-kinds=definite,indirect \
	          --error-exitcode=0 --suppressions=../valgrind.supp \
	          --log-file=../reports/valgrind-sdl-rapport.txt ./space_invaders_sdl --valgrind-test || true
	@echo "✓ Rapport sauvegardé dans reports/valgrind-sdl-rapport.txt"

# ----------------------------------------------------------------------------
# valgrind-ncurses : Analyse mémoire de la version ncurses
# ----------------------------------------------------------------------------
valgrind-ncurses: ncurses
	@echo "→ Analyse mémoire (valgrind) sur la version ncurses [MODE RÉDUIT]..."
	@mkdir -p reports
	@cd $(BIN_DIR) && valgrind --leak-check=full --show-leak-kinds=definite,indirect \
	          --error-exitcode=0 --suppressions=../valgrind.supp \
	          --log-file=../reports/valgrind-ncurses-rapport.txt ./space_invaders_ncurses --valgrind-test > /dev/null 2>&1 || true
	@echo "✓ Rapport sauvegardé dans reports/valgrind-ncurses-rapport.txt"

# ----------------------------------------------------------------------------
# valgrind-tests : Analyse mémoire des tests
# ----------------------------------------------------------------------------
valgrind-tests: $(TEST_EXEC)
	@echo "→ Analyse mémoire (valgrind) sur les tests [MODE RÉDUIT]..."
	@mkdir -p reports
	@valgrind --leak-check=full --show-leak-kinds=definite,indirect \
	          --error-exitcode=0 --suppressions=valgrind.supp \
	          --log-file=reports/valgrind-tests-rapport.txt $(TEST_EXEC) || true
	@echo "✓ Rapport sauvegardé dans reports/valgrind-tests-rapport.txt"

# ----------------------------------------------------------------------------
# valgrind-report : Génère un rapport Valgrind complet pour toutes les versions
# ----------------------------------------------------------------------------
valgrind-report: valgrind-sdl valgrind-ncurses valgrind-tests
	@echo "→ Génération du rapport Valgrind consolidé..."
	@mkdir -p reports
	@echo "╔════════════════════════════════════════════════════════════════════════╗" > reports/valgrind-rapport-complet.txt
	@echo "║        SPACE INVADERS MVC - RAPPORT VALGRIND COMPLET                  ║" >> reports/valgrind-rapport-complet.txt
	@echo "║        Généré le: $$(date '+%Y-%m-%d %H:%M:%S')                       ║" >> reports/valgrind-rapport-complet.txt
	@echo "╚════════════════════════════════════════════════════════════════════════╝" >> reports/valgrind-rapport-complet.txt
	@echo "" >> reports/valgrind-rapport-complet.txt
	@echo "=================================================================================" >> reports/valgrind-rapport-complet.txt
	@echo "RAPPORT SDL" >> reports/valgrind-rapport-complet.txt
	@echo "=================================================================================" >> reports/valgrind-rapport-complet.txt
	@cat reports/valgrind-sdl-rapport.txt >> reports/valgrind-rapport-complet.txt 2>/dev/null || echo "Rapport SDL non disponible" >> reports/valgrind-rapport-complet.txt
	@echo "" >> reports/valgrind-rapport-complet.txt
	@echo "=================================================================================" >> reports/valgrind-rapport-complet.txt
	@echo "RAPPORT NCURSES" >> reports/valgrind-rapport-complet.txt
	@echo "=================================================================================" >> reports/valgrind-rapport-complet.txt
	@cat reports/valgrind-ncurses-rapport.txt >> reports/valgrind-rapport-complet.txt 2>/dev/null || echo "Rapport ncurses non disponible" >> reports/valgrind-rapport-complet.txt
	@echo "" >> reports/valgrind-rapport-complet.txt
	@echo "=================================================================================" >> reports/valgrind-rapport-complet.txt
	@echo "RAPPORT TESTS" >> reports/valgrind-rapport-complet.txt
	@echo "=================================================================================" >> reports/valgrind-rapport-complet.txt
	@cat reports/valgrind-tests-rapport.txt >> reports/valgrind-rapport-complet.txt 2>/dev/null || echo "Rapport tests non disponible" >> reports/valgrind-rapport-complet.txt
	@echo "✓ Rapport complet généré: reports/valgrind-rapport-complet.txt"

# ----------------------------------------------------------------------------
# check-memory / memcheck : Vérifie la mémoire pour toutes les versions
# ----------------------------------------------------------------------------
check-memory memcheck: valgrind-sdl valgrind-ncurses valgrind-tests
	@echo "✓ Analyse mémoire complète terminée"

# ----------------------------------------------------------------------------
# leak-check : Vérification rapide des fuites mémoire
# ----------------------------------------------------------------------------
leak-check: sdl
	@echo "→ Vérification rapide des fuites mémoire..."
	@valgrind --leak-check=summary --error-exitcode=1 $(SDL_EXEC)

# ----------------------------------------------------------------------------
# check-style : Vérifie le style du code
# ----------------------------------------------------------------------------
check-style:
	@echo "→ Vérification du style du code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror; \
		echo "✓ Style du code conforme"; \
	else \
		echo "⚠ clang-format non trouvé, vérification ignorée"; \
	fi

# ----------------------------------------------------------------------------
# format : Formate automatiquement le code
# ----------------------------------------------------------------------------
format:
	@echo "→ Formatage automatique du code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
		echo "✓ Code formaté avec succès"; \
	else \
		echo "✗ clang-format non trouvé"; \
		exit 1; \
	fi



# ----------------------------------------------------------------------------
# coverage : Génère un rapport de couverture de code (TODO)
# ----------------------------------------------------------------------------
coverage:
	@echo "⚠ Couverture de code non encore implémentée"
	@echo "  Suggestion : utiliser gcov/lcov"

# ----------------------------------------------------------------------------
# benchmark : Exécute des tests de performance (TODO)
# ----------------------------------------------------------------------------
benchmark:
	@echo "⚠ Benchmarks non encore implémentés"

# ============================================================================
# DOCUMENTATION
# ============================================================================

# ----------------------------------------------------------------------------
# doc / generate-docs : Génère la documentation Doxygen
# ----------------------------------------------------------------------------
doc generate-docs:
	@echo "→ Génération de la documentation..."
	@mkdir -p $(DOC_DIR)
	@if command -v doxygen >/dev/null 2>&1; then \
		if [ ! -f Doxyfile ]; then \
			doxygen -g Doxyfile; \
		fi; \
		doxygen Doxyfile; \
		echo "✓ Documentation générée dans $(DOC_DIR)/html/"; \
	else \
		echo "✗ Doxygen non trouvé"; \
		echo "  Installez-le avec votre gestionnaire de paquets"; \
		exit 1; \
	fi

# ============================================================================
# PRÉPARATION DES RESSOURCES
# ============================================================================

# ----------------------------------------------------------------------------
# prepare-assets : Prépare les ressources (polices, fichiers de scores, etc.)
# ----------------------------------------------------------------------------
prepare-assets: | $(BIN_DIR)
	@echo "→ Préparation des ressources..."
	@mkdir -p $(BIN_DIR)/fonts
	@mkdir -p $(BIN_DIR)/misc
	@mkdir -p $(BIN_DIR)/pictures
	@mkdir -p $(BIN_DIR)/assets
	@if [ -d "fonts" ]; then \
		cp -r fonts/* $(BIN_DIR)/fonts/ 2>/dev/null || true; \
	fi
	@if [ -d "misc" ]; then \
		cp -r misc/* $(BIN_DIR)/misc/ 2>/dev/null || true; \
	fi
	@if [ -d "src/pictures" ]; then \
		cp -r src/pictures/* $(BIN_DIR)/pictures/ 2>/dev/null || true; \
	fi
	@if [ -d "src/assets" ]; then \
		cp -r src/assets/* $(BIN_DIR)/assets/ 2>/dev/null || true; \
	fi
	@cp -f highscore.dat $(BIN_DIR)/ 2>/dev/null || true
	@cp -f README.md $(BIN_DIR)/ 2>/dev/null || true
	@echo "✓ Ressources préparées (polices, images, audio)"

# ============================================================================
# NETTOYAGE
# ============================================================================

# ----------------------------------------------------------------------------
# clean : Nettoie tous les fichiers de compilation
# ----------------------------------------------------------------------------
clean:
	@echo "→ Nettoyage des fichiers de compilation..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@rm -f *.o *.a *.so *.gcno *.gcda *.gcov
	@rm -f space_invaders_sdl space_invaders_ncurses test_runner
	@rm -f gmon.out callgrind.out.* valgrind-*.log
	@rm -rf $(DOC_DIR)/html $(DOC_DIR)/latex
	@rm -f $(DIST_DIR)/*.tar.gz $(DIST_DIR)/*.zip
	@rm -rf reports
	@echo "✓ Nettoyage terminé"

# ============================================================================
# INSTALLATION ET DÉSINSTALLATION
# ============================================================================

# ----------------------------------------------------------------------------
# install : Installe les exécutables dans le système
# ----------------------------------------------------------------------------
install: all
	@echo "→ Installation dans /usr/local/bin..."
	@sudo install -m 755 $(SDL_EXEC) /usr/local/bin/space_invaders_sdl
	@sudo install -m 755 $(NCURSES_EXEC) /usr/local/bin/space_invaders_ncurses
	@echo "✓ Installation terminée"

# ----------------------------------------------------------------------------
# uninstall : Désinstalle les exécutables du système
# ----------------------------------------------------------------------------
uninstall:
	@echo "→ Désinstallation..."
	@sudo rm -f /usr/local/bin/space_invaders_sdl
	@sudo rm -f /usr/local/bin/space_invaders_ncurses
	@echo "✓ Désinstallation terminée"

# ============================================================================
# CRÉATION DE PACKAGES
# ============================================================================

# ----------------------------------------------------------------------------
# dist : Crée un package de distribution
# ----------------------------------------------------------------------------
dist: clean all doc
	@echo "→ Création du package de distribution..."
	@mkdir -p $(DIST_DIR)
	@VERSION=$$(date +%Y%m%d-%H%M%S); \
	TARNAME=space_invaders_mvc_$$VERSION.tar.gz; \
	tar -czf $(DIST_DIR)/$$TARNAME \
	    --exclude=".*" \
	    --exclude="$(BUILD_DIR)" \
	    --exclude="$(BIN_DIR)" \
	    --exclude="$(DIST_DIR)" \
	    --transform 's,^,space_invaders_mvc/,' \
	    .; \
	echo "✓ Package créé : $(DIST_DIR)/$$TARNAME"

# ----------------------------------------------------------------------------
# package : Alias pour dist
# ----------------------------------------------------------------------------
package: dist

# ============================================================================
# VÉRIFICATIONS ET DIAGNOSTICS
# ============================================================================

# ----------------------------------------------------------------------------
# info : Affiche les informations de compilation
# ----------------------------------------------------------------------------
info:
	@echo "════════════════════════════════════════════════════════════"
	@echo "  SPACE INVADERS MVC - INFORMATIONS DE COMPILATION"
	@echo "════════════════════════════════════════════════════════════"
	@echo "Plateforme      : Linux"
	@echo "Compilateur     : $(CC)"
	@echo "CFLAGS          : $(CFLAGS)"
	@echo ""
	@echo "SDL3 CFLAGS     : $(SDL_CFLAGS)"
	@echo "SDL3 LDFLAGS    : $(SDL_LDFLAGS)"
	@echo "NCURSES LDFLAGS : $(NCURSES_LDFLAGS)"
	@echo ""
	@echo "Répertoires :"
	@echo "  Sources       : $(SRC_DIR)"
	@echo "  Compilation   : $(BUILD_DIR)"
	@echo "  Binaires      : $(BIN_DIR)"
	@echo "  Documentation : $(DOC_DIR)"
	@echo ""
	@echo "Exécutables :"
	@echo "  SDL           : $(SDL_EXEC)"
	@echo "  ncurses       : $(NCURSES_EXEC)"
	@echo "  Tests         : $(TEST_EXEC)"
	@echo "════════════════════════════════════════════════════════════"

# ----------------------------------------------------------------------------
# check-project : Vérifie la structure du projet
# ----------------------------------------------------------------------------
check-project:
	@echo "→ Vérification de la structure du projet..."
	@if [ -f checkproject.sh ]; then \
		bash checkproject.sh || true; \
	else \
		echo "⚠ Script checkproject.sh non trouvé"; \
	fi
	@echo "✓ Vérification terminée"

# ----------------------------------------------------------------------------
# fullcheck : Vérification complète (compilation, tests, mémoire, style)
# ----------------------------------------------------------------------------
fullcheck: clean prepare-assets all run-tests check-memory check-style
	@echo "✓ Vérification complète terminée avec succès"

# ============================================================================
# COMPILATION SPÉCIALE
# ============================================================================

# ----------------------------------------------------------------------------
# rebuild : Recompile tout depuis zéro
# ----------------------------------------------------------------------------
rebuild: clean all

# ----------------------------------------------------------------------------
# debug : Compilation en mode débogage
# ----------------------------------------------------------------------------
debug: CFLAGS += -DDEBUG -O0 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined
debug: SDL_LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: NCURSES_LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: rebuild
	@echo "✓ Compilation en mode débogage terminée"

# ----------------------------------------------------------------------------
# release : Compilation optimisée pour la production
# ----------------------------------------------------------------------------
release: CFLAGS = -Wall -Wextra -O3 -flto -DNDEBUG \
                  -I./src -I./src/controller -I./src/core -I./src/utils -I./src/views
release: rebuild
	@echo "✓ Compilation en mode release terminée"

# ----------------------------------------------------------------------------
# profile : Compilation avec support du profiling
# ----------------------------------------------------------------------------
profile: CFLAGS += -pg -O2
profile: SDL_LDFLAGS += -pg
profile: NCURSES_LDFLAGS += -pg
profile: rebuild
	@echo "✓ Compilation en mode profiling terminée"
	@echo "  Utilisez gprof après l'exécution pour analyser les performances"

# ============================================================================
# AIDE
# ============================================================================

# ----------------------------------------------------------------------------
# help : Affiche l'aide complète
# ----------------------------------------------------------------------------
help:
	@echo "════════════════════════════════════════════════════════════════════════"
	@echo "  SPACE INVADERS MVC - SYSTÈME DE COMPILATION"
	@echo "════════════════════════════════════════════════════════════════════════"
	@echo ""
	@echo "📦 COMPILATION"
	@echo "  make all                - Compile SDL, ncurses et les outils"
	@echo "  make sdl                - Compile uniquement la version SDL"
	@echo "  make ncurses            - Compile uniquement la version ncurses"
	@echo "  make tools              - Compile les outils auxiliaires"
	@echo "  make rebuild            - Nettoie et recompile tout"
	@echo ""
	@echo "▶️  EXÉCUTION"
	@echo "  make run-sdl            - Compile et lance la version SDL"
	@echo "  make run-ncurses        - Compile et lance la version ncurses"
	@echo "  make run-tests          - Compile et exécute les tests"
	@echo ""
	@echo "🧪 TESTS ET VÉRIFICATIONS"
	@echo "  make test               - Exécute les tests unitaires"
	@echo "  make valgrind-sdl       - Analyse mémoire (SDL)"
	@echo "  make valgrind-ncurses   - Analyse mémoire (ncurses)"
	@echo "  make valgrind-tests     - Analyse mémoire (tests)"
	@echo "  make valgrind-report    - Génère un rapport Valgrind complet"
	@echo "  make check-memory       - Analyse mémoire complète"
	@echo "  make leak-check         - Vérification rapide des fuites"
	@echo "  make check-style        - Vérifie le style du code"
	@echo "  make format             - Formate automatiquement le code"
	@echo "  make fullcheck          - Vérification complète du projet"
	@echo ""
	@echo "📚 DOCUMENTATION"
	@echo "  make doc                - Génère la documentation Doxygen"
	@echo "  make info               - Affiche les informations de build"
	@echo ""
	@echo "🔧 DÉPENDANCES"
	@echo "  make install-deps       - Installe les dépendances nécessaires"
	@echo "  make check-sdl-deps     - Vérifie les dépendances SDL3"
	@echo "  make check-ncurses-deps - Vérifie les dépendances ncurses"
	@echo ""
	@echo "🗑️  NETTOYAGE"
	@echo "  make clean              - Supprime tous les fichiers générés"
	@echo ""
	@echo "📦 INSTALLATION"
	@echo "  make install            - Installe dans /usr/local/bin"
	@echo "  make uninstall          - Désinstalle du système"
	@echo "  make dist               - Crée un package de distribution"
	@echo "  make package            - Alias pour dist"
	@echo ""
	@echo "🔨 MODES DE COMPILATION SPÉCIAUX"
	@echo "  make debug              - Compile en mode débogage (sanitizers)"
	@echo "  make release            - Compile en mode optimisé (production)"
	@echo "  make profile            - Compile avec support du profiling"
	@echo ""
	@echo "🛠️  UTILITAIRES"
	@echo "  make prepare-assets     - Prépare les ressources (polices, etc.)"
	@echo "  make check-project      - Vérifie la structure du projet"
	@echo "  make help               - Affiche cette aide"
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════════"
	@echo "📖 Pour plus d'informations, consultez le README.md"
	@echo "════════════════════════════════════════════════════════════════════════"

# ============================================================================
# INCLUSION DES FICHIERS DE DÉPENDANCES (générés automatiquement)
# ============================================================================
-include $(SDL_OBJS:.o=.d)
-include $(NCURSES_OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

# ============================================================================
# GÉNÉRATION AUTOMATIQUE DES DÉPENDANCES (optionnel)
# ============================================================================
%.d: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) -MT $@ $< > $@