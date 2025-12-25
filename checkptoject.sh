#!/bin/bash
# Space Invaders MVC - Project Structure Validator
# Checks that all required files are present and properly structured

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
PASSED=0
FAILED=0
WARNING=0

# Header
echo -e "${BLUE}╔════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   Space Invaders MVC - Project Validator      ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
echo ""

# Function to check file existence
check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $1 ${RED}(MISSING)${NC}"
        ((FAILED++))
        return 1
    fi
}

# Function to check directory existence
check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
        ((PASSED++))
        return 0
    else
        echo -e "${RED}✗${NC} $1/ ${RED}(MISSING)${NC}"
        ((FAILED++))
        return 1
    fi
}

# Function to check optional file
check_optional() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1 ${GREEN}(optional)${NC}"
        ((PASSED++))
    else
        echo -e "${YELLOW}⚠${NC} $1 ${YELLOW}(optional - missing)${NC}"
        ((WARNING++))
    fi
}

# Check root files
echo -e "\n${BLUE}Checking root files...${NC}"
check_file "Makefile"
check_file "README.md"
check_optional "LICENSE"
check_optional ".gitignore"

# Check directory structure
echo -e "\n${BLUE}Checking directory structure...${NC}"
check_dir "src"
check_dir "src/core"
check_dir "src/controller"
check_dir "src/views"
check_dir "src/utils"
check_optional "assets"
check_optional "doc"

# Check core files (MODÈLE)
echo -e "\n${BLUE}Checking CORE files (Model)...${NC}"
check_file "src/core/model.h"
check_file "src/core/model.c"
check_file "src/core/game_state.h"
check_file "src/core/game_state.c"

# Check controller files (CONTRÔLEUR)
echo -e "\n${BLUE}Checking CONTROLLER files...${NC}"
check_file "src/controller/controller.h"
check_file "src/controller/controller.c"
check_file "src/controller/input_handler.h"
check_file "src/controller/input_handler.c"
check_file "src/controller/commands.h"

# Check view files (VUES)
echo -e "\n${BLUE}Checking VIEW files...${NC}"
check_file "src/views/view_base.h"
check_file "src/views/view_sdl.h"
check_file "src/views/view_sdl.c"
check_file "src/views/view_ncurses.h"
check_file "src/views/view_ncurses.c"
check_file "src/views/rect_utils.h"

# Check main entry points
echo -e "\n${BLUE}Checking main entry points...${NC}"
check_file "src/main_sdl.c"
check_file "src/main_ncurses.c"

# Check utils
echo -e "\n${BLUE}Checking utility files...${NC}"
check_file "src/utils/platform.h"
check_optional "src/utils/config.h"
check_optional "src/utils/config.c"
check_optional "src/utils/logger.h"
check_optional "src/utils/logger.c"

# Check for build artifacts (should not exist in repo)
echo -e "\n${BLUE}Checking for build artifacts (should be clean)...${NC}"
if [ -d "build" ]; then
    echo -e "${YELLOW}⚠${NC} build/ exists ${YELLOW}(should be gitignored)${NC}"
    ((WARNING++))
else
    echo -e "${GREEN}✓${NC} build/ not present (clean)"
    ((PASSED++))
fi

if [ -f "space_invaders_sdl" ]; then
    echo -e "${YELLOW}⚠${NC} space_invaders_sdl exists ${YELLOW}(should be gitignored)${NC}"
    ((WARNING++))
else
    echo -e "${GREEN}✓${NC} No SDL executable (clean)"
    ((PASSED++))
fi

if [ -f "space_invaders_ncurses" ]; then
    echo -e "${YELLOW}⚠${NC} space_invaders_ncurses exists ${YELLOW}(should be gitignored)${NC}"
    ((WARNING++))
else
    echo -e "${GREEN}✓${NC} No ncurses executable (clean)"
    ((PASSED++))
fi

# Check MVC separation (basic)
echo -e "\n${BLUE}Checking MVC separation...${NC}"

# Model should not include SDL or ncurses
if grep -r "SDL\.h\|ncurses\.h" src/core/*.c src/core/*.h 2>/dev/null; then
    echo -e "${RED}✗${NC} Model includes SDL/ncurses headers ${RED}(VIOLATION!)${NC}"
    ((FAILED++))
else
    echo -e "${GREEN}✓${NC} Model is independent of views"
    ((PASSED++))
fi

# Views should not include model logic keywords
if grep -rE "(model_update|model_move_player|model_player_shoot)" src/views/*.c 2>/dev/null | grep -v "model->"; then
    echo -e "${RED}✗${NC} Views contain model logic ${RED}(VIOLATION!)${NC}"
    ((FAILED++))
else
    echo -e "${GREEN}✓${NC} Views don't contain game logic"
    ((PASSED++))
fi

# Check for C99 compliance indicators
echo -e "\n${BLUE}Checking C99 compliance...${NC}"
if grep -r "\/\/" src/*.c src/*/*.c 2>/dev/null | head -1 > /dev/null; then
    echo -e "${GREEN}✓${NC} C99 style comments found"
    ((PASSED++))
else
    echo -e "${YELLOW}⚠${NC} No C99 comments (using C89 style)"
    ((WARNING++))
fi

# Check for common issues
echo -e "\n${BLUE}Checking for common issues...${NC}"

# Check for memory leaks patterns
if grep -r "malloc\|calloc" src/ 2>/dev/null | wc -l > /dev/null; then
    MALLOC_COUNT=$(grep -r "malloc\|calloc" src/ 2>/dev/null | wc -l)
    FREE_COUNT=$(grep -r "free(" src/ 2>/dev/null | wc -l)
    echo -e "${BLUE}ℹ${NC} Found $MALLOC_COUNT allocations and $FREE_COUNT frees"
    if [ $FREE_COUNT -lt $MALLOC_COUNT ]; then
        echo -e "${YELLOW}⚠${NC} Potential memory leaks (run valgrind)"
        ((WARNING++))
    else
        echo -e "${GREEN}✓${NC} Allocation/free balance looks good"
        ((PASSED++))
    fi
fi

# Check Makefile targets
echo -e "\n${BLUE}Checking Makefile targets...${NC}"
REQUIRED_TARGETS=("all" "clean" "run-sdl" "run-ncurses" "valgrind")
for target in "${REQUIRED_TARGETS[@]}"; do
    if grep -q "^$target:" Makefile 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Target: $target"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} Missing target: $target"
        ((FAILED++))
    fi
done

# Summary
echo -e "\n${BLUE}╔════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                   SUMMARY                      ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
echo -e "${GREEN}Passed:   $PASSED${NC}"
echo -e "${YELLOW}Warnings: $WARNING${NC}"
echo -e "${RED}Failed:   $FAILED${NC}"
echo ""

# Exit code
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ Project structure is VALID${NC}"
    echo -e "${BLUE}Ready to compile with: make all${NC}"
    exit 0
else
    echo -e "${RED}✗ Project structure has ERRORS${NC}"
    echo -e "${YELLOW}Please fix the missing files/issues above${NC}"
    exit 1
fi