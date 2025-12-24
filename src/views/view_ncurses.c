
#include "../core/model.h"
#include <stdio.h>
#include <unistd.h>

// Draw player tank
static void ncurses_draw_player(const GameModel* model) {
	move(model->player.hitbox.y, model->player.hitbox.x);
	addch('^');
}

// Draw aliens
static void ncurses_draw_aliens(const GameModel* model) {
	for (int i = 0; i < INVADER_ROWS; ++i) {
		for (int j = 0; j < INVADER_ROWS; ++j) {
			const Invader* inv = &model->invaders.invaders[i][j];
			if (inv->alive) {
				move(inv->hitbox.y, inv->hitbox.x);
				addch('#');
			}
		}
	}
}

// Draw bases
static void ncurses_draw_bases(const GameModel* model) {
	for (int i = 0; i < BASE_COUNT; ++i) {
		const Base* base = &model->bases[i];
		if (base->alive) {
			move(base->hitbox.y, base->hitbox.x);
			addch('M');
		}
	}
}

// Draw saucer
static void ncurses_draw_saucer(const GameModel* model) {
	if (model->saucer.alive) {
		move(model->saucer.hitbox.y, model->saucer.hitbox.x);
		addch('O');
	}
}

// Draw bullets
static void ncurses_draw_bullets(const GameModel* model) {
	for (int i = 0; i < PLAYER_BULLETS; ++i) {
		if (model->player_bullets[i].alive) {
			move(model->player_bullets[i].hitbox.y, model->player_bullets[i].hitbox.x);
			addch('*');
		}
	}
	for (int i = 0; i < ENEMY_BULLETS; ++i) {
		if (model->enemy_bullets[i].alive) {
			move(model->enemy_bullets[i].hitbox.y, model->enemy_bullets[i].hitbox.x);
			addch('o');
		}
	}
}

// Draw HUD (score, etc.)
static void ncurses_draw_hud(const GameModel* model) {
	char buf[64];
	move(0, (COLS/2)-9);
	addstr("--SPACE INVADERS--");
	move(0, 1);
	addstr("SCORE: ");
	snprintf(buf, sizeof(buf), "%d", model->player.score);
	move(0, 8);
	addstr(buf);
	move(0, COLS-19);
	addstr("m = menu  q = quit");
}

void ncurses_view_render_game(NcursesView* view, const GameModel* model) {
	if (!view || !model) return;
	clear();
	ncurses_draw_hud(model);
	ncurses_draw_player(model);
	ncurses_draw_aliens(model);
	ncurses_draw_bases(model);
	ncurses_draw_saucer(model);
	ncurses_draw_bullets(model);
	refresh();
}
#include "view_ncurses.h"
#include "view_ncurses.h"
#include <stdlib.h>
#include <string.h>
#include "../core/model.h"
#include <stdlib.h>
#include <string.h>

NcursesView* ncurses_view_create(void) {
	NcursesView* view = malloc(sizeof(NcursesView));
	if (!view) return NULL;
	memset(view, 0, sizeof(NcursesView));
	view->width = 80;
	view->height = 24;
	view->initialized = false;
	view->last_score = 0;
	view->last_win = -1;
	return view;
}

void ncurses_view_destroy(NcursesView* view) {
	if (view) free(view);
}

bool ncurses_view_init(NcursesView* view, int width, int height) {
	if (!view) return false;
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, 1);
	keypad(stdscr, 1);
	view->width = width > 0 ? width : COLS;
	view->height = height > 0 ? height : LINES;
	view->initialized = true;
	return true;
}

bool ncurses_view_load_resources(NcursesView* view) {
	// No resources to load for ncurses
	return true;
}
