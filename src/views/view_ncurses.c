#include "view_ncurses.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Helper function to convert pixel coordinates to grid coordinates
int ncurses_scale_x(int pixel_x) {
    int char_x = pixel_x / CHAR_SCALE_X;
    if (char_x < 0) char_x = 0;
    if (char_x >= NCURSES_GAME_WIDTH) char_x = NCURSES_GAME_WIDTH - 1;
    return char_x;
}

int ncurses_scale_y(int pixel_y) {
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
    
    view->game_start_x = 0;
    view->game_start_y = 3; 
    view->score_start_x = NCURSES_GAME_WIDTH;
    
    return view;
}

void ncurses_view_destroy(NcursesView* view) {
    if (view) {
        if (view->initialized) endwin();
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
    
    if (width > 0 && height > 0) {
        view->width = width;
        view->height = height;
        view->game_start_x = (width - NCURSES_GAME_WIDTH - NCURSES_SCORE_WIDTH) / 2;
        view->game_start_y = 3;
        view->score_start_x = view->game_start_x + NCURSES_GAME_WIDTH;
    } else {
        view->width = NCURSES_SCREEN_WIDTH;
        view->height = NCURSES_SCREEN_HEIGHT;
        view->game_start_x = 0;
        view->game_start_y = 3;
        view->score_start_x = NCURSES_GAME_WIDTH;
    }
    
    view->initialized = true;
    clear();
    refresh();
    return true;
}

bool ncurses_view_load_resources(NcursesView* view) {
    (void)view; 
    return true;
}

// Draw player
static void ncurses_draw_player(NcursesView* view, const GameModel* model) {
    int x = view->game_start_x + ncurses_scale_x(model->player.hitbox.x);
    int y = view->game_start_y + ncurses_scale_y(model->player.hitbox.y);
    mvaddch(y, x, '^');
    mvaddch(y, x-1, '<');
    mvaddch(y, x+1, '>');
}

// Draw aliens
static void ncurses_draw_aliens(NcursesView* view, const GameModel* model) {
    if (model->player.level == 4) return; 

    for (int i = 0; i < INVADER_ROWS; ++i) {
        for (int j = 0; j < INVADER_COLS; ++j) {
            const Invader* inv = &model->invaders.invaders[i][j];
            if (inv->alive && inv->dying_timer == 0) {
                int x = view->game_start_x + ncurses_scale_x(inv->hitbox.x);
                int y = view->game_start_y + ncurses_scale_y(inv->hitbox.y);
                char c = (inv->row == 0) ? 'Y' : (inv->row < 3 ? 'X' : 'W');
                mvaddch(y, x, c);
            } else if (inv->alive && inv->dying_timer > 0) {
                int x = view->game_start_x + ncurses_scale_x(inv->hitbox.x);
                int y = view->game_start_y + ncurses_scale_y(inv->hitbox.y);
                mvaddch(y, x, '*');
            }
        }
    }
}

// Draw Boss (ASCII Art)
static void ncurses_draw_boss(NcursesView* view, const GameModel* model) {
    if (!model->boss.alive) return;

    int x = view->game_start_x + ncurses_scale_x(model->boss.hitbox.x);
    int y = view->game_start_y + ncurses_scale_y(model->boss.hitbox.y);
    
    // Bounds check
    if (x < view->game_start_x) x = view->game_start_x;
    if (x > view->game_start_x + NCURSES_GAME_WIDTH - 9) x = view->game_start_x + NCURSES_GAME_WIDTH - 9;

    mvprintw(y, x,   " /=======\\ ");
    mvprintw(y+1, x, "<( O X O )>");
    mvprintw(y+2, x, " \\^^^^^^^/ ");
}

// Draw saucer
static void ncurses_draw_saucer(NcursesView* view, const GameModel* model) {
    if (model->saucer.alive) {
        int x = view->game_start_x + ncurses_scale_x(model->saucer.hitbox.x);
        int y = view->game_start_y + ncurses_scale_y(model->saucer.hitbox.y);
        mvprintw(y, x, "<@>");
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

// Draw HUD
static void ncurses_draw_hud(NcursesView* view, const GameModel* model) {
    // Borders
    for (int x = view->game_start_x; x < view->game_start_x + NCURSES_GAME_WIDTH + 1; x++) {
        mvaddch(view->game_start_y - 1, x, '-');
        mvaddch(view->game_start_y + NCURSES_GAME_HEIGHT, x, '-');
    }
    for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT; y++) {
        mvaddch(y, view->game_start_x - 1, '|');
        mvaddch(y, view->game_start_x + NCURSES_GAME_WIDTH, '|');
        mvaddch(y, view->score_start_x + NCURSES_SCORE_WIDTH, '|');
    }

    mvprintw(0, view->game_start_x + (NCURSES_GAME_WIDTH/2) - 7, "SPACE INVADERS");
    
    int score_x = view->score_start_x + 1;
    int score_y = view->game_start_y + 1;
    
    mvprintw(score_y, score_x, "SCORE: %d", model->player.score);
    mvprintw(score_y + 2, score_x, "LIVES: %d", model->player.lives);
    mvprintw(score_y + 4, score_x, "LEVEL: %d", model->player.level);
    
    // Boss Health Bar (Level 4)
    if (model->player.level == 4 && model->boss.alive) {
        mvprintw(score_y + 16, score_x, "BOSS HP:");
        mvprintw(score_y + 17, score_x, "[");
        int bars = (model->boss.health * 10) / model->boss.max_health;
        for(int i=0; i<10; i++) {
            if (i < bars) addch('='); else addch(' ');
        }
        addch(']');
    }
}

void ncurses_view_render_game(NcursesView* view, const GameModel* model) {
    if (!view || !model) return;
    
    // Clear game area
    for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT; y++) {
        for (int x = view->game_start_x; x < view->game_start_x + NCURSES_GAME_WIDTH; x++) {
            mvaddch(y, x, ' ');
        }
    }
    
    ncurses_draw_player(view, model);
    if (model->player.level == 4) ncurses_draw_boss(view, model);
    else ncurses_draw_aliens(view, model);
    
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
        case STATE_WIN:
            ncurses_view_render_game(view, model);
            ncurses_view_render_game_over(view, 1);
            break;
        case STATE_LEVEL_TRANSITION:
            ncurses_view_render_game(view, model);
            mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2,
                    view->game_start_x + (NCURSES_GAME_WIDTH/2) - 10,
                    "LEVEL %d - PRESS SPACE", model->player.level);
            refresh();
            break;
        default:
            ncurses_view_render_game(view, model);
            break;
    }
}

void ncurses_view_render_menu(NcursesView* view, const GameModel* model) {
    clear();
    int cx = view->width / 2;
    int cy = view->height / 2;
    mvprintw(cy - 4, cx - 7, "SPACE INVADERS");
    mvprintw(cy - 2, cx - 10, "by Amine Boucif");
    mvprintw(cy + 1, cx - 8, "PRESS SPACE");
    mvprintw(cy + 3, cx - 8, "High Score: %d", model->high_score);
    refresh();
}

void ncurses_view_render_pause(NcursesView* view) {
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT/2,
            view->game_start_x + (NCURSES_GAME_WIDTH/2) - 3, "PAUSED");
    refresh();
}

void ncurses_view_render_game_over(NcursesView* view, int win) {
    int cx = view->game_start_x + (NCURSES_GAME_WIDTH/2);
    int cy = view->game_start_y + NCURSES_GAME_HEIGHT/2;
    if (win) mvprintw(cy - 1, cx - 4, "YOU WIN!");
    else mvprintw(cy - 1, cx - 5, "GAME OVER");
    mvprintw(cy + 1, cx - 8, "Press R to Restart");
    refresh();
}

bool ncurses_view_poll_event(NcursesView* view, int* key) {
    if (!view || !key) return false;
    int ch = getch();
    if (ch != ERR) { *key = ch; return true; }
    return false;
}