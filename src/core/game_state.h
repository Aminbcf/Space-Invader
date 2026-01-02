#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "model.h"
#include <stdint.h>

typedef struct {
  GameModel *model;
  void *view_data;
  void *controller_data;
  bool running;
  uint32_t last_update;
  float time_scale;
} GameContext;

// Gestion du contexte
GameContext *game_context_create(void);
void game_context_destroy(GameContext *context);
void game_context_update(GameContext *context);
void game_context_render(GameContext *context);

// États du jeu
typedef void (*StateFunction)(GameContext *);

void state_menu(GameContext *context);
void state_playing(GameContext *context);
void state_paused(GameContext *context);
void state_game_over(GameContext *context);
void state_level_transition(GameContext *context);

// Transition d'états
void transition_to_state(GameContext *context, GameState new_state);

#endif // GAME_STATE_H