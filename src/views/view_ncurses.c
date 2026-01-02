#include "view_ncurses.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Helper function to convert keycode to human-readable name
static const char *get_ncurses_key_name(int key) {
  static char buf[16];
  switch (key) {
  case KEY_UP:
    return "UP";
  case KEY_DOWN:
    return "DOWN";
  case KEY_LEFT:
    return "LEFT";
  case KEY_RIGHT:
    return "RIGHT";
  case 32:
    return "SPACE";
  case 27:
    return "ESC";
  case 10:
    return "ENTER";
  case 13:
    return "ENTER";
  case 9:
    return "TAB";
  case 127:
    return "BKSP";
  case KEY_BACKSPACE:
    return "BKSP";
  // Common SDL keycodes (for display if loaded from SDL config)
  case 1073741904:
    return "L-ARROW";
  case 1073741903:
    return "R-ARROW";
  case 1073741906:
    return "U-ARROW";
  case 1073741905:
    return "D-ARROW";
  case 1073742049:
    return "L-SHIFT";
  default:
    if (key >= 32 && key <= 126) {
      buf[0] = (char)key;
      buf[1] = '\0';
      return buf;
    }
    snprintf(buf, 16, "<%d>", key);
    return buf;
  }
}

// Helper function to convert pixel coordinates to grid coordinates
int ncurses_scale_x(int pixel_x) {
  int char_x = pixel_x / CHAR_SCALE_X;
  if (char_x < 0)
    char_x = 0;
  if (char_x >= NCURSES_GAME_WIDTH)
    char_x = NCURSES_GAME_WIDTH - 1;
  return char_x;
}

int ncurses_scale_y(int pixel_y) {
  int char_y = pixel_y / CHAR_SCALE_Y;
  if (char_y < 0)
    char_y = 0;
  if (char_y >= NCURSES_GAME_HEIGHT)
    char_y = NCURSES_GAME_HEIGHT - 1;
  return char_y;
}

NcursesView *ncurses_view_create(void) {
  NcursesView *view = malloc(sizeof(NcursesView));
  if (!view)
    return NULL;
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

void ncurses_view_destroy(NcursesView *view) {
  if (view) {
    if (view->initialized)
      endwin();
    free(view);
  }
}

bool ncurses_view_init(NcursesView *view, int width, int height) {
  if (!view)
    return false;

  initscr();
  start_color();
  cbreak();
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);

  // Initialize Colors
  init_pair(1, COLOR_CYAN, COLOR_BLACK);    // Player 1
  init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Invaders
  init_pair(3, COLOR_RED, COLOR_BLACK);     // Enemy Bullets / Boss
  init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Player 2 / Saucer
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Boss
  init_pair(6, COLOR_WHITE, COLOR_BLACK);   // HUD / Borders

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

bool ncurses_view_load_resources(NcursesView *view) {
  (void)view;
  return true;
}

// Draw players
static void ncurses_draw_player(NcursesView *view, const GameModel *model) {
  for (int p = 0; p < 2; p++) {
    if (!model->players[p].is_active)
      continue;
    // Fix: Alignment issue - calculate center of player in pixels, then scale
    // Previous code used top-left which caused visual to be shifted left of
    // hitbox
    int center_px = model->players[p].hitbox.x + PLAYER_WIDTH / 2;
    int x = view->game_start_x + ncurses_scale_x(center_px);
    int y = view->game_start_y + ncurses_scale_y(model->players[p].hitbox.y);

    // Blink if invincible
    if (model->players[p].invincibility_timer > 0) {
      // Fast blink: visible if timer * 10 is even
      int blink = (int)(model->players[p].invincibility_timer * 10) % 2;
      if (blink) {
        attron(A_DIM);
      } else {
        continue; // Skip drawing this frame for blink effect
      }
    }

    attron(COLOR_PAIR(p == 0 ? 1 : 4));
    mvaddch(y, x, p == 0 ? '^' : 'A');
    mvaddch(y, x - 1, '<');
    mvaddch(y, x + 1, '>');
    attroff(COLOR_PAIR(p == 0 ? 1 : 4));
    attroff(A_DIM);
    attroff(COLOR_PAIR(p == 0 ? 1 : 4));
  }
}

// Draw aliens
static void ncurses_draw_aliens(NcursesView *view, const GameModel *model) {
  if (model->boss.alive)
    return;

  attron(COLOR_PAIR(2));
  for (int i = 0; i < INVADER_ROWS; ++i) {
    for (int j = 0; j < INVADER_COLS; ++j) {
      const Invader *inv = &model->invaders.invaders[i][j];
      if (inv->alive && inv->dying_timer == 0) {
        int x = view->game_start_x + ncurses_scale_x(inv->hitbox.x);
        int y = view->game_start_y + ncurses_scale_y(inv->hitbox.y);
        char c = (inv->row == 0) ? 'Y' : (inv->row < 3 ? 'X' : 'W');
        mvaddch(y, x, c);
      } else if (inv->alive && inv->dying_timer > 0) {
        int x = view->game_start_x + ncurses_scale_x(inv->hitbox.x);
        int y = view->game_start_y + ncurses_scale_y(inv->hitbox.y);
        attron(COLOR_PAIR(3));
        mvaddch(y, x, '*');
        attroff(COLOR_PAIR(3));
      }
    }
  }
  attroff(COLOR_PAIR(2));

  // Draw Big Invader
  if (model->invaders.big_invader.alive) {
    int x = view->game_start_x +
            ncurses_scale_x(model->invaders.big_invader.hitbox.x);
    int y = view->game_start_y +
            ncurses_scale_y(model->invaders.big_invader.hitbox.y);
    attron(COLOR_PAIR(5));
    if (model->invaders.state == 0) {
      mvprintw(y, x, "  _/^\_  ");
      mvprintw(y + 1, x, "<[ ooo ]>");
      mvprintw(y + 2, x, " \\_V_V_/");
    } else {
      attron(A_BOLD); // Flash effect
      mvprintw(y, x, "  _/^\\_  ");
      mvprintw(y + 1, x, "<[ *** ]>");
      mvprintw(y + 2, x, " \\_W_W_/"); // Engines pulse
      attroff(A_BOLD);
    }
    attroff(COLOR_PAIR(5));

    // Show HP bar (Centered above)
    int hp_bars = (model->invaders.big_invader.health * 5) /
                  model->invaders.big_invader.max_health;
    mvprintw(y - 1, x + 1, "["); // Offset +1 to center roughly
    for (int i = 0; i < 5; i++) {
      addch(i < hp_bars ? '=' : ' ');
    }
    addch(']');
    attroff(COLOR_PAIR(5) | A_BOLD);
  }
}

// Draw Boss (ASCII Art)
static void ncurses_draw_boss(NcursesView *view, const GameModel *model) {
  if (!model->boss.alive)
    return;

  int x = view->game_start_x + ncurses_scale_x(model->boss.hitbox.x);
  int y = view->game_start_y + ncurses_scale_y(model->boss.hitbox.y);

  attron(COLOR_PAIR(5));
  mvprintw(y, x, " /=======\\ ");
  mvprintw(y + 1, x, "<( O X O )>");
  mvprintw(y + 2, x, " \\^^^^^^^/ ");
  attroff(COLOR_PAIR(5));
}

// Draw powerups
static void ncurses_draw_powerups(NcursesView *view, const GameModel *model) {
  for (int i = 0; i < 10; i++) {
    if (model->powerups[i].alive) {
      int x = view->game_start_x + ncurses_scale_x(model->powerups[i].hitbox.x);
      int y = view->game_start_y + ncurses_scale_y(model->powerups[i].hitbox.y);
      attron(COLOR_PAIR(4) | A_BOLD);
      const char *symbol = "?";
      switch (model->powerups[i].type) {
      case PWR_TRIPLE_SHOT:
        symbol = "T";
        break;
      case PWR_STRONG_MISSILE:
        symbol = "S";
        break;
      case PWR_SHIELD:
        symbol = "#";
        break;
      default:
        break;
      }
      mvprintw(y, x, "[%s]", symbol);
      attroff(COLOR_PAIR(4) | A_BOLD);
    }
  }
}

// Draw saucer
static void ncurses_draw_saucer(NcursesView *view, const GameModel *model) {
  if (model->saucer.alive) {
    int x = view->game_start_x + ncurses_scale_x(model->saucer.hitbox.x);
    int y = view->game_start_y + ncurses_scale_y(model->saucer.hitbox.y);
    attron(COLOR_PAIR(4));
    mvprintw(y, x, "<@>");
    attroff(COLOR_PAIR(4));
  }
}

// Draw bullets
static void ncurses_draw_bullets(NcursesView *view, const GameModel *model) {
  for (int p = 0; p < 2; p++) {
    attron(COLOR_PAIR(p == 0 ? 1 : 4));
    for (int i = 0; i < PLAYER_BULLETS; ++i) {
      if (model->player_bullets[p][i].alive) {
        int x = view->game_start_x +
                ncurses_scale_x(model->player_bullets[p][i].hitbox.x);
        int y = view->game_start_y +
                ncurses_scale_y(model->player_bullets[p][i].hitbox.y);
        mvaddch(y, x, '|');
      }
    }
    attroff(COLOR_PAIR(p == 0 ? 1 : 4));
  }

  // Enemy bullets with different types
  for (int i = 0; i < ENEMY_BULLETS; ++i) {
    if (model->enemy_bullets[i].alive) {
      int x = view->game_start_x +
              ncurses_scale_x(model->enemy_bullets[i].hitbox.x);
      int y = view->game_start_y +
              ncurses_scale_y(model->enemy_bullets[i].hitbox.y);

      if (model->enemy_bullets[i].type == 2) { // Laser
        attron(COLOR_PAIR(5));                 // Magenta
        mvaddch(y, x, '!');
        attroff(COLOR_PAIR(5));
      } else if (model->enemy_bullets[i].type == 1) { // ZigZag
        attron(COLOR_PAIR(4));                        // Yellow
        mvaddch(y, x, '~');
        attroff(COLOR_PAIR(4));
      } else {                 // Standard
        attron(COLOR_PAIR(3)); // Red
        mvaddch(y, x, 'o');
        attroff(COLOR_PAIR(3));
      }
    }
  }
}

// Draw HUD
static void ncurses_draw_hud(NcursesView *view, const GameModel *model) {
  attron(COLOR_PAIR(6));
  // Borders
  for (int x = view->game_start_x;
       x < view->game_start_x + NCURSES_GAME_WIDTH + 1; x++) {
    mvaddch(view->game_start_y - 1, x, '-');
    mvaddch(view->game_start_y + NCURSES_GAME_HEIGHT, x, '-');
  }
  for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT;
       y++) {
    mvaddch(y, view->game_start_x - 1, '|');
    mvaddch(y, view->game_start_x + NCURSES_GAME_WIDTH, '|');
    mvaddch(y, view->score_start_x + NCURSES_SCORE_WIDTH, '|');
  }

  int score_x = view->score_start_x + 1;
  int score_y = view->game_start_y + 1;

  mvprintw(score_y, score_x, "P1 SCORE: %d", model->players[0].score);
  mvprintw(score_y + 1, score_x, "P1 LIVES: %d", model->players[0].lives);

  if (model->two_player_mode) {
    mvprintw(score_y + 3, score_x, "P2 SCORE: %d", model->players[1].score);
    mvprintw(score_y + 4, score_x, "P2 LIVES: %d", model->players[1].lives);
  }

  mvprintw(score_y + 7, score_x, "LEVEL: %d", model->players[0].level);
  mvprintw(score_y + 10, score_x, "HI SCO: %d", model->high_score);

  // Boss HP
  if (model->boss.alive) {
    mvprintw(score_y + 14, score_x, "BOSS HP:");
    mvprintw(score_y + 15, score_x, "[");
    int bars = (model->boss.health * 10) / model->boss.max_health;
    attron(COLOR_PAIR(3));
    for (int i = 0; i < 10; i++) {
      addch(i < bars ? '=' : ' ');
    }
    attroff(COLOR_PAIR(3));
    addch(']');
  }
  attroff(COLOR_PAIR(6));
}

void ncurses_view_render_game(NcursesView *view, const GameModel *model) {
  if (!view || !model)
    return;

  // Increment frame counter for animation
  view->frame_count++;

  // Clear game area with space background
  for (int y = view->game_start_y; y < view->game_start_y + NCURSES_GAME_HEIGHT;
       y++) {
    mvhline(y, view->game_start_x, ' ', NCURSES_GAME_WIDTH);
  }

  ncurses_draw_player(view, model);
  ncurses_draw_boss(view, model);
  ncurses_draw_aliens(view, model);
  ncurses_draw_saucer(view, model);
  ncurses_draw_bullets(view, model);
  ncurses_draw_powerups(view, model);
  ncurses_draw_hud(view, model);

  refresh();
}

void ncurses_view_render(NcursesView *view, const GameModel *model) {
  if (!view || !model)
    return;

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
    mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT / 2,
             view->game_start_x + (NCURSES_GAME_WIDTH / 2) - 10,
             "LEVEL %d - PRESS SPACE", model->players[0].level);
    refresh();
    break;
  default:
    ncurses_view_render_game(view, model);
    break;
  }
}

void ncurses_view_render_menu(NcursesView *view, const GameModel *model) {
  if (!view || !model)
    return;
  clear();
  int cx = view->width / 2;
  int cy = 2;

  switch (model->menu_state) {
  case MENU_MAIN:
    box(stdscr, 0, 0);              // Add border
    attron(COLOR_PAIR(2) | A_BOLD); // Green Title
    mvprintw(cy, cx - 7, "SPACE INVADERS");
    attroff(COLOR_PAIR(2) | A_BOLD);
    mvprintw(cy + 1, cx - 10, "by Amine Boucif");

    const char *main_items[] = {"START 1P", "START 2P", "SETTINGS", "QUIT"};
    for (int i = 0; i < 4; i++) {
      if (i == model->menu_selection) {
        attron(COLOR_PAIR(1) | A_REVERSE | A_BOLD); // Cyan Highlight
        mvprintw(cy + 5 + i * 2, cx - 8, "  %s  ", main_items[i]);
        attroff(COLOR_PAIR(1) | A_REVERSE | A_BOLD);
      } else
        mvprintw(cy + 5 + i * 2, cx - 8, "  %s  ", main_items[i]);
    }

    attron(A_DIM);
    mvprintw(cy + 12, cx - 8, "High Score: %d", model->high_score);
    attroff(A_DIM);
    break;

  case MENU_DIFFICULTY:
    mvprintw(cy, cx - 9, "SELECT DIFFICULTY");
    const char *diff_items[] = {"EASY", "NORMAL", "HARD", "ROGUE"};
    const char *diff_desc[] = {"3 Levels, No Boss", "4 Levels, Boss Fight",
                               "Faster & Aggressive", "Random & Endless"};
    for (int i = 0; i < 4; i++) {
      if (i == model->menu_selection) {
        attron(COLOR_PAIR(1));
        mvprintw(cy + 5 + i * 3, cx - 7, "> %s <", diff_items[i]);
        mvprintw(cy + 6 + i * 3, cx - (int)strlen(diff_desc[i]) / 2, "%s",
                 diff_desc[i]);
        attroff(COLOR_PAIR(1));
      } else
        mvprintw(cy + 5 + i * 3, cx - 5, "  %s  ", diff_items[i]);
    }
    break;

  case MENU_SETTINGS:
    mvprintw(cy, cx - 4, "SETTINGS");
    const char *set_items[] = {"CONTROLS", "MUSIC VOLUME", "BACK"};
    for (int i = 0; i < 3; i++) {
      if (i == model->menu_selection) {
        if (i == 1)
          mvprintw(cy + 5 + i * 2, cx - 12, "> MUSIC VOLUME: %.0f%% <",
                   model->music_volume * 100);
        else
          mvprintw(cy + 5 + i * 2, cx - 8, "> %s <", set_items[i]);
      } else {
        if (i == 1)
          mvprintw(cy + 5 + i * 2, cx - 10, "  MUSIC VOLUME: %.0f%%  ",
                   model->music_volume * 100);
        else
          mvprintw(cy + 5 + i * 2, cx - 6, "  %s  ", set_items[i]);
      }
    }
    break;

  case MENU_CONTROLS: {
    mvprintw(cy, cx - 4, "CONTROLS");

    if (model->waiting_for_key) {
      attron(COLOR_PAIR(4) | A_BOLD);
      mvprintw(cy + 2, cx - 8, "PRESS ANY KEY...");
      attroff(COLOR_PAIR(4) | A_BOLD);
    }

    const char *actions[] = {"LEFT", "RIGHT", "UP", "DOWN", "SHOOT"};

    // Player 1 keybindings
    attron(COLOR_PAIR(1));
    mvprintw(cy + 4, cx - 6, "PLAYER 1");
    attroff(COLOR_PAIR(1));
    for (int i = 0; i < 5; i++) {
      if (model->menu_selection == i)
        attron(A_REVERSE);
      const char *keyname = get_ncurses_key_name(model->keybinds_p1[i]);
      mvprintw(cy + 5 + i, cx - 10, "%s: %s", actions[i], keyname);
      if (model->menu_selection == i)
        attroff(A_REVERSE);
    }

    // Player 2 keybindings
    attron(COLOR_PAIR(3));
    mvprintw(cy + 11, cx - 6, "PLAYER 2");
    attroff(COLOR_PAIR(3));
    for (int i = 0; i < 5; i++) {
      if (model->menu_selection == 5 + i)
        attron(A_REVERSE);
      const char *keyname = get_ncurses_key_name(model->keybinds_p2[i]);
      mvprintw(cy + 12 + i, cx - 10, "%s: %s", actions[i], keyname);
      if (model->menu_selection == 5 + i)
        attroff(A_REVERSE);
    }

    // Back button
    if (model->menu_selection == 10)
      attron(A_REVERSE);
    mvprintw(cy + 18, cx - 4, "> BACK <");
    if (model->menu_selection == 10)
      attroff(A_REVERSE);
    break;
  }
  }

  refresh();
}

void ncurses_view_render_pause(NcursesView *view) {
  mvprintw(view->game_start_y + NCURSES_GAME_HEIGHT / 2,
           view->game_start_x + (NCURSES_GAME_WIDTH / 2) - 3, "PAUSED");
  refresh();
}

void ncurses_view_render_game_over(NcursesView *view, int win) {
  int cx = view->game_start_x + (NCURSES_GAME_WIDTH / 2);
  int cy = view->game_start_y + NCURSES_GAME_HEIGHT / 2;
  if (win)
    mvprintw(cy - 1, cx - 4, "YOU WIN!");
  else
    mvprintw(cy - 1, cx - 5, "GAME OVER");
  mvprintw(cy + 1, cx - 8, "Press R to Restart");
  refresh();
}

bool ncurses_view_poll_event(NcursesView *view, int *key) {
  if (!view || !key)
    return false;
  int ch = getch();
  if (ch != ERR) {
    *key = ch;
    return true;
  }
  return false;
}