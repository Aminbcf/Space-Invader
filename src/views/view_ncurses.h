#ifndef VIEW_NCURSES_H
#define VIEW_NCURSES_H

#include "../core/model.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include "view_base.h"

// Ncurses-specific grid constants
#define NCURSES_GAME_WIDTH 60      // Game area in characters
#define NCURSES_GAME_HEIGHT 20     // Game area in characters
#define NCURSES_SCORE_WIDTH 20     // Score area width
#define NCURSES_SCREEN_WIDTH (NCURSES_GAME_WIDTH + NCURSES_SCORE_WIDTH)
#define NCURSES_SCREEN_HEIGHT (NCURSES_GAME_HEIGHT + 3) // +3 for HUD

// Character scale factor (pixels to characters)
#define CHAR_SCALE_X (GAME_AREA_WIDTH / NCURSES_GAME_WIDTH)
#define CHAR_SCALE_Y (SCREEN_HEIGHT / NCURSES_GAME_HEIGHT)

// Ncurses view structure
typedef struct {
    // Interface (for MVC)
    ViewInterface interface;
    // State
    int width;
    int height;
    bool initialized;
    // For menu/gameover, etc.
    int last_score;
    int last_win;
    // Grid boundaries
    int game_start_x;
    int game_start_y;
    int score_start_x;
} NcursesView;

// Creation/destruction
NcursesView* ncurses_view_create(void);
void ncurses_view_destroy(NcursesView* view);

// Initialization
bool ncurses_view_init(NcursesView* view, int width, int height);
bool ncurses_view_load_resources(NcursesView* view);

// Rendering
void ncurses_view_render(NcursesView* view, const GameModel* model);
void ncurses_view_render_game(NcursesView* view, const GameModel* model);
void ncurses_view_render_menu(NcursesView* view, const GameModel* model);
void ncurses_view_render_pause(NcursesView* view);
void ncurses_view_render_game_over(NcursesView* view, int win);

// Event polling
bool ncurses_view_poll_event(NcursesView* view, int* key);

// Helper functions
int ncurses_scale_x(int pixel_x);
int ncurses_scale_y(int pixel_y);

#endif // VIEW_NCURSES_H