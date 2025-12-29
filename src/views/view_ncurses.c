#include "view_ncurses.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Helper function to convert pixel coordinates to grid coordinates
int ncurses_scale_x(int pixel_x) {
    // Convert pixel X to character X within game area
    int char_x = pixel_x / CHAR_SCALE_X;
    if (char_x < 0) char_x = 0;
    if (char_x >= NCURSES_GAME_WIDTH) char_x = NCURSES_GAME_WIDTH - 1;
    return char_x;
}

int ncurses_scale_y(int pixel_y) {
    // Convert pixel Y to character Y within game area
    int char_y = pixel_y / CHAR_SCALE_Y;
    if (char_y < 0) char_y = 0;
    if (char_y >= NCURSES_GAME_HEIGHT) char_y = NCURSES_GAME_HEIGHT - 1;
    return char_y;
}

NcursesView* ncurses_view_create(void) {
    NcursesView* view = malloc(sizeof(NcursesView));
    if (!view) return NULL;
    memset(view, 0, sizeof(NcursesView));
    view->width = NCURSES_SCREEN_WIDTH;
    view->height = NCURSES_SCREEN_HEIGHT;
    view->initialized = false;
    view->last_score = 0;
    view->last_win = -1;
    
    // Set grid boundaries
    view->game_start_x = 0;
    view->game_start_y = 3; // Leave space for HUD at top
    view->score_start_x = NCURSES_GAME_WIDTH;
    
    return view;
}

void ncurses_view_destroy(NcursesView* view) {
    if (view) {
        if (view->initialized) {
            endwin();
        }
        free(view);
    }
}

bool ncurses_view_init(NcursesView* view, int width, int height) {
    if (!view) return false;
    
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    
    // Check if terminal is large enough
    if (COLS < NCURSES_SCREEN_WIDTH || LINES < NCURSES_SCREEN_HEIGHT) {
        endwin();
        fprintf(stderr, "Terminal too small! Need at least %dx%d, got %dx%d\n",
                NCURSES_SCREEN_WIDTH, NCURSES_SCREEN_HEIGHT, COLS, LINES);
        return false;
    }
    
    // Use centered game area if terminal is larger
    if (width > 0 && height > 0) {
        view->width = width;
        view->height = height;
        view->game_start_x = (width - NCURSES_GAME_WIDTH - NCURSES_SCORE_WIDTH) / 2;
        view->game_start_y = 3; // HUD at top
        view->score_start_x = view->game_start_x + NCURSES_GAME_WIDTH;
    } else {
        view->width = NCURSES_SCREEN_WIDTH;
        view->height = NCURSES_SCREEN_HEIGHT;
        view->game_start_x = 0;
        view->game_start_y = 3;
        view->score_start_x = NCURSES_GAME_WIDTH;
    }
    
    view->initialized = true;
    
    // Draw initial border
    clear();
    // Draw border around game area
    for (int x = view->game_start_x; x < view->game_start_x + NCURSES_GAME_WIDTH + 1; x++) {
        mvaddch(view->game_start_y - 1, x, '-');
        mvaddch(view->game_start_y + NCURSES_GAME_HEIGHT, x, '-');
    }
    for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT; y++) {
        mvaddch(y, view->game_start_x - 1, '|');
        mvaddch(y, view->game_start_x + NCURSES_GAME_WIDTH, '|');
    }
    
    // Draw border around score area
    for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT; y++) {
        mvaddch(y, view->score_start_x - 1, '|');
        mvaddch(y, view->score_start_x + NCURSES_SCORE_WIDTH, '|');
    }
    for (int x = view->score_start_x - 1; x < view->score_start_x + NCURSES_SCORE_WIDTH + 1; x++) {
        mvaddch(view->game_start_y - 1, x, '-');
        mvaddch(view->game_start_y + NCURSES_GAME_HEIGHT, x, '-');
    }
    
    refresh();
    return true;
}

bool ncurses_view_load_resources(NcursesView* view) {
    (void)view; // Suppress unused parameter warning
    // No resources to load for ncurses
    return true;
}

// Draw player tank
static void ncurses_draw_player(NcursesView* view, const GameModel* model) {
    int x = view->game_start_x + ncurses_scale_x(model->player.hitbox.x);
    int y = view->game_start_y + ncurses_scale_y(model->player.hitbox.y);
    mvaddch(y, x, '^');
}

// Draw aliens
static void ncurses_draw_aliens(NcursesView* view, const GameModel* model) {
    for (int i = 0; i < INVADER_ROWS; ++i) {
        for (int j = 0; j < INVADER_COLS; ++j) {
            const Invader* inv = &model->invaders.invaders[i][j];
            if (inv->alive) {
                int x = view->game_start_x + ncurses_scale_x(inv->hitbox.x);
                int y = view->game_start_y + ncurses_scale_y(inv->hitbox.y);
                // Different characters for different invader types
                char c = '#';
                if (inv->row == 0) c = 'O';  // Top row
                else if (inv->row < 3) c = 'M'; // Middle rows
                else c = 'V'; // Bottom rows
                mvaddch(y, x, c);
            }
        }
    }
}

// Draw bases
static void ncurses_draw_bases(NcursesView* view, const GameModel* model) {
    for (int i = 0; i < BASE_COUNT; ++i) {
        const Base* base = &model->bases[i];
        if (base->alive) {
            int x = view->game_start_x + ncurses_scale_x(base->hitbox.x);
            int y = view->game_start_y + ncurses_scale_y(base->hitbox.y);
            // Draw base as a block of characters
            for (int dx = 0; dx < 3; dx++) {
                for (int dy = 0; dy < 2; dy++) {
                    if (x + dx < view->game_start_x + NCURSES_GAME_WIDTH &&
                        y + dy < view->game_start_y + NCURSES_GAME_HEIGHT) {
                        mvaddch(y + dy, x + dx, '#');
                    }
                }
            }
        }
    }
}

// Draw saucer
static void ncurses_draw_saucer(NcursesView* view, const GameModel* model) {
    if (model->saucer.alive) {
        int x = view->game_start_x + ncurses_scale_x(model->saucer.hitbox.x);
        int y = view->game_start_y + ncurses_scale_y(model->saucer.hitbox.y);
        mvaddch(y, x, '@');
    }
}

// Draw bullets
static void ncurses_draw_bullets(NcursesView* view, const GameModel* model) {
    for (int i = 0; i < PLAYER_BULLETS; ++i) {
        if (model->player_bullets[i].alive) {
            int x = view->game_start_x + ncurses_scale_x(model->player_bullets[i].hitbox.x);
            int y = view->game_start_y + ncurses_scale_y(model->player_bullets[i].hitbox.y);
            mvaddch(y, x, '|');
        }
    }
    for (int i = 0; i < ENEMY_BULLETS; ++i) {
        if (model->enemy_bullets[i].alive) {
            int x = view->game_start_x + ncurses_scale_x(model->enemy_bullets[i].hitbox.x);
            int y = view->game_start_y + ncurses_scale_y(model->enemy_bullets[i].hitbox.y);
            mvaddch(y, x, 'o');
        }
    }
}

// Clear game area
static void ncurses_clear_game_area(NcursesView* view) {
    for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT; y++) {
        for (int x = view->game_start_x; x < view->game_start_x + NCURSES_GAME_WIDTH; x++) {
            mvaddch(y, x, ' ');
        }
    }
}

// Draw HUD (score, etc.)
static void ncurses_draw_hud(NcursesView* view, const GameModel* model) {
    // Top bar with game title
    mvprintw(0, view->game_start_x + (NCURSES_GAME_WIDTH/2) - 7, "SPACE INVADERS");
    
    // Draw score area
    int score_x = view->score_start_x + 1;
    int score_y = view->game_start_y + 2;
    
    mvprintw(score_y, score_x, "SCORE: %d", model->player.score);
    mvprintw(score_y + 2, score_x, "LIVES: %d", model->player.lives);
    mvprintw(score_y + 4, score_x, "LEVEL: %d", model->player.level);
    mvprintw(score_y + 6, score_x, "HIGH: %d", model->high_score);
    
    // Controls help
    mvprintw(score_y + 10, score_x, "CONTROLS:");
    mvprintw(score_y + 11, score_x, "A/D: Move");
    mvprintw(score_y + 12, score_x, "SPACE: Shoot");
    mvprintw(score_y + 13, score_x, "P: Pause");
    mvprintw(score_y + 14, score_x, "R: Restart");
    mvprintw(score_y + 15, score_x, "Q: Quit");
}

void ncurses_view_render_game(NcursesView* view, const GameModel* model) {
    if (!view || !model) return;
    
    // Clear only the game area, not the entire screen
    ncurses_clear_game_area(view);
    
    // Draw game elements
    ncurses_draw_player(view, model);
    ncurses_draw_aliens(view, model);
    //ncurses_draw_bases(view, model);
    ncurses_draw_saucer(view, model);
    ncurses_draw_bullets(view, model);
    ncurses_draw_hud(view, model);
    
    refresh();
}

void ncurses_view_render(NcursesView* view, const GameModel* model) {
    if (!view || !model) return;
    
    switch (model->state) {
        case STATE_MENU:
            ncurses_view_render_menu(view, model);
            break;
        case STATE_PLAYING:
            ncurses_view_render_game(view, model);
            break;
        case STATE_PAUSED:
            ncurses_view_render_game(view, model);
            ncurses_view_render_pause(view);
            break;
        case STATE_GAME_OVER:
            ncurses_view_render_game(view, model);
            ncurses_view_render_game_over(view, 0);
            break;
        case STATE_LEVEL_TRANSITION:
            ncurses_view_render_game(view, model);
            // Show level transition message
            mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2,
                    view->game_start_x + (NCURSES_GAME_WIDTH/2) - 8,
                    "LEVEL %d", model->player.level);
            refresh();
            break;
        default:
            ncurses_view_render_game(view, model);
            break;
    }
}

void ncurses_view_render_menu(NcursesView* view, const GameModel* model) {
    clear();
    
    int center_x = view->width / 2;
    int center_y = view->height / 2;
    
    mvprintw(center_y - 3, center_x - 7, "SPACE INVADERS");
    mvprintw(center_y - 1, center_x - 10, "====================");
    mvprintw(center_y + 1, center_x - 8, "1. Start Game");
    mvprintw(center_y + 2, center_x - 8, "2. High Score: %d", model->high_score);
    mvprintw(center_y + 3, center_x - 8, "3. Controls");
    mvprintw(center_y + 4, center_x - 8, "4. Quit");
    mvprintw(center_y + 6, center_x - 12, "Press 1-4 to select");
    
    // Draw a simple spaceship
    mvprintw(center_y - 6, center_x - 3, "  ^  ");
    mvprintw(center_y - 5, center_x - 3, " / \\ ");
    mvprintw(center_y - 4, center_x - 3, "/___\\");
    
    refresh();
}

void ncurses_view_render_pause(NcursesView* view) {
    // Draw semi-transparent overlay
    for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT; y++) {
        for (int x = view->game_start_x; x < view->game_start_x + NCURSES_GAME_WIDTH; x++) {
            mvaddch(y, x, '.'); // Dots to indicate paused state
        }
    }
    
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2,
            view->game_start_x + (NCURSES_GAME_WIDTH/2) - 3,
            "PAUSED");
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2 + 1,
            view->game_start_x + (NCURSES_GAME_WIDTH/2) - 8,
            "Press P to resume");
    refresh();
}

void ncurses_view_render_game_over(NcursesView* view, int win) {
    // Keep the game screen but overlay message
    if (win) {
        mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2 - 1,
                view->game_start_x + (NCURSES_GAME_WIDTH/2) - 4,
                "YOU WIN!");
    } else {
        mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2 - 1,
                view->game_start_x + (NCURSES_GAME_WIDTH/2) - 5,
                "GAME OVER");
    }
    
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2,
            view->game_start_x + (NCURSES_GAME_WIDTH/2) - 10,
            "Score: %d", view->last_score);
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2 + 1,
            view->game_start_x + (NCURSES_GAME_WIDTH/2) - 10,
            "Press R to restart");
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2 + 2,
            view->game_start_x + (NCURSES_GAME_WIDTH/2) - 8,
            "Press Q to quit");
    
    refresh();
}

bool ncurses_view_poll_event(NcursesView* view, int* key) {
    if (!view || !key) return false;
    
    int ch = getch();
    if (ch != ERR) {
        *key = ch;
        return true;
    }
    
    return false;
}
