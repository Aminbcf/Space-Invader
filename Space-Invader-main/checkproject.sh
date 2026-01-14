#!/bin/bash
# Space Invaders MVC - Validation de la structure du projet
# Vérifie que tous les fichiers requis sont présents et correctement structurés

set -e

# Couleurs pour la sortie
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Compteurs
PASSED=0
FAILED=0
WARNING=0

# En-tête
echo -e "${BLUE}╔════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Space Invaders MVC - Validateur de projet  ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
echo ""

# Fonction pour vérifier l'existence d'un fichier
check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $1 ${RED}(MANQUANT)${NC}"
        ((FAILED++))
        return 1
    fi
}

# Fonction pour vérifier l'existence d'un répertoire
check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $1/ ${RED}(MANQUANT)${NC}"
        ((FAILED++))
        return 1
    fi
}

# Fonction pour vérifier un fichier optionnel
check_optional() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1 ${GREEN}(optionnel)${NC}"
        ((PASSED++))
    else
        echo -e "${YELLOW}⚠${NC} $1 ${YELLOW}(optionnel - manquant)${NC}"
        ((WARNING++))
    fi
}

# Vérification des fichiers racine
echo -e "\n${BLUE}Vérification des fichiers racine...${NC}"
check_file "Makefile"
check_optional "LICENSE"
check_optional ".gitignore"

# Vérification de la structure des répertoires
echo -e "\n${BLUE}Vérification de la structure des répertoires...${NC}"
check_dir "src"
check_dir "src/core"
check_dir "src/controller"
check_dir "src/views"
check_dir "src/utils"
check_optional "assets"
check_optional "docs"
check_optional "fonts"

# Vérification des fichiers CORE (MODÈLE)
echo -e "\n${BLUE}Vérification des fichiers CORE (Modèle)...${NC}"
check_file "src/core/model.h"
check_file "src/core/model.c"
check_file "src/core/game_state.h"
check_file "src/core/game_state.c"

# Vérification des fichiers CONTRÔLEUR
echo -e "\n${BLUE}Vérification des fichiers CONTRÔLEUR...${NC}"
check_file "src/controller/controller.h"
check_file "src/controller/controller.c"
check_file "src/controller/input_handler.h"
check_file "src/controller/input_handler.c"
check_file "src/controller/commands.h"

# Vérification des fichiers VUE
echo -e "\n${BLUE}Vérification des fichiers VUE...${NC}"
check_file "src/views/view_base.h"
check_optional "src/views/view_sdl.h"
check_file "src/views/view_sdl.c"
check_optional "src/views/view_ncurses.h"
check_file "src/views/view_ncurses.c"
check_optional "src/views/rect_utils.h"

# Vérification des points d'entrée principaux
echo -e "\n${BLUE}Vérification des points d'entrée...${NC}"
check_file "src/main_sdl.c"
check_file "src/main_ncurses.c"

# Vérification des utilitaires
echo -e "\n${BLUE}Vérification des fichiers utilitaires...${NC}"
check_file "src/utils/platform.h"
check_optional "src/utils/platform_sdl.c"
check_optional "src/utils/font_manager.h"
check_optional "src/utils/font_manager.c"
check_optional "src/utils/config.h"
check_optional "src/utils/config.c"

# Vérification des artéfacts de compilation (ne devraient pas exister dans le dépôt)
echo -e "\n${BLUE}Vérification des artéfacts de compilation (doit être propre)...${NC}"
if [ -d "build" ]; then
    if [ -z "$(ls -A build)" ]; then
        echo -e "${GREEN}✓${NC} build/ existe mais est vide (propre)"
        ((PASSED++))
    else
        echo -e "${YELLOW}⚠${NC} build/ contient des fichiers ${YELLOW}(devrait être dans .gitignore)${NC}"
        ((WARNING++))
    fi
else
    echo -e "${GREEN}✓${NC} build/ n'existe pas (propre)"
    ((PASSED++))
fi

if [ -d "bin" ]; then
    if [ -z "$(ls -A bin 2>/dev/null)" ]; then
        echo -e "${GREEN}✓${NC} bin/ existe mais est vide (propre)"
        ((PASSED++))
    else
        echo -e "${YELLOW}⚠${NC} bin/ contient des fichiers ${YELLOW}(devrait être dans .gitignore)${NC}"
        ((WARNING++))
    fi
else
    echo -e "${GREEN}✓${NC} bin/ n'existe pas (propre)"
    ((PASSED++))
fi

# Vérification de la séparation MVC (basique)
echo -e "\n${BLUE}Vérification de la séparation MVC...${NC}"

# Le modèle ne devrait pas inclure SDL ou ncurses
if [ -d "src/core" ]; then
    if grep -r "SDL\.h\|ncurses\.h" src/core/*.c src/core/*.h 2>/dev/null; then
        echo -e "${RED}✗${NC} Le modèle inclut des en-têtes SDL/ncurses ${RED}(VIOLATION!)${NC}"
        ((FAILED++))
    else
        echo -e "${GREEN}✓${NC} Le modèle est indépendant des vues"
        ((PASSED++))
    fi
fi

# Les vues ne devraient pas contenir de logique métier
if [ -d "src/views" ]; then
    if grep -rE "(model_update|model_move_player|model_player_shoot)" src/views/*.c 2>/dev/null | grep -v "model->"; then
        echo -e "${RED}✗${NC} Les vues contiennent de la logique métier ${RED}(VIOLATION!)${NC}"
        ((FAILED++))
    else
        echo -e "${GREEN}✓${NC} Les vues ne contiennent pas de logique de jeu"
        ((PASSED++))
    fi
fi

# Vérification de la conformité C11
echo -e "\n${BLUE}Vérification de la conformité C11...${NC}"
if grep -r "//" src/*.c src/*/*.c 2>/dev/null | head -1 > /dev/null; then
    echo -e "${GREEN}✓${NC} Commentaires de style C99/C11 trouvés"
    ((PASSED++))
else
    echo -e "${YELLOW}⚠${NC} Aucun commentaire C99 (utilise le style C89)"
    ((WARNING++))
fi

# Vérification des problèmes courants
echo -e "\n${BLUE}Vérification des problèmes courants...${NC}"

# Vérification des patterns de fuites mémoire
if grep -r "malloc\|calloc" src/ 2>/dev/null | wc -l > /dev/null; then
    MALLOC_COUNT=$(grep -r "malloc\|calloc" src/ 2>/dev/null | wc -l)
    FREE_COUNT=$(grep -r "free(" src/ 2>/dev/null | wc -l)
    echo -e "${BLUE}ℹ${NC} Trouvé $MALLOC_COUNT allocations et $FREE_COUNT libérations"
    if [ $FREE_COUNT -lt $MALLOC_COUNT ]; then
        echo -e "${YELLOW}⚠${NC} Fuites mémoire potentielles (exécutez valgrind)"
        ((WARNING++))
    else
        echo -e "${GREEN}✓${NC} Balance allocation/libération correcte"
        ((PASSED++))
    fi
fi

# Vérification des cibles du Makefile
echo -e "\n${BLUE}Vérification des cibles du Makefile...${NC}"
REQUIRED_TARGETS=("all" "clean" "run-sdl" "run-ncurses")
for target in "${REQUIRED_TARGETS[@]}"; do
    if grep -q "^$target:" Makefile 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Cible: $target"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} Cible manquante: $target"
        ((FAILED++))
    fi
done

# Vérification des cibles Valgrind (optionnelles mais recommandées)
echo -e "\n${BLUE}Vérification des cibles Valgrind (optionnelles)...${NC}"
VALGRIND_TARGETS=("valgrind-sdl" "valgrind-ncurses" "check-memory")
for target in "${VALGRIND_TARGETS[@]}"; do
    if grep -q "^$target:" Makefile 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Cible Valgrind: $target"
        ((PASSED++))
    else
        echo -e "${YELLOW}⚠${NC} Cible Valgrind manquante: $target ${YELLOW}(recommandée)${NC}"
        ((WARNING++))
    fi
done

# Résumé
echo -e "\n${BLUE}╔════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                   RÉSUMÉ                       ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
echo -e "${GREEN}Réussi:        $PASSED${NC}"
echo -e "${YELLOW}Avertissements: $WARNING${NC}"
echo -e "${RED}Échoué:        $FAILED${NC}"
echo ""

# Code de sortie
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ La structure du projet est VALIDE${NC}"
    echo -e "${BLUE}Prêt à compiler avec: make all${NC}"
    exit 0
else
    echo -e "${RED}✗ La structure du projet a des ERREURS${NC}"
    echo -e "${YELLOW}Veuillez corriger les fichiers/problèmes manquants ci-dessus${NC}"
    exit 1
fi
