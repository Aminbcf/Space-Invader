#include "controller.h"
#include "commands.h"
#include "input_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Simple timer using standard C */
static uint32_t get_ticks(void) {
  return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

Controller *controller_create(GameModel *model) {
  Controller *controller = malloc(sizeof(Controller));
  if (!controller)
    return NULL;

  memset(controller, 0, sizeof(Controller));
  controller->model = model;
  controller->view_context = NULL;
  controller->input_handler = input_handler_create();

  if (!controller->input_handler) {
    free(controller);
    return NULL;
  }

// Initialize default keybindings
#ifdef USE_SDL_VIEW
  controller_set_keybindings(controller, 0, 1073741906, 1073741905, 1073741904,
                             1073741903, 32, 112, 27);
#else
  controller_set_keybindings(controller, 0, 'w', 's', 'a', 'd', ' ', 'p', 27);
#endif

  controller->render_callback = NULL;
  controller->audio_callback = NULL;
  controller->callback_data = NULL;

  controller->quit_requested = false;
  controller->paused = false;
  controller->last_input_time = get_ticks();

  return controller;
}

void controller_destroy(Controller *controller) {
  if (controller) {
    if (controller->input_handler) {
      input_handler_destroy(controller->input_handler);
    }
    free(controller);
  }
}

void controller_set_view_context(Controller *controller, void *view_context) {
  if (controller) {
    controller->view_context = view_context;
  }
}

void controller_set_keybindings(Controller *controller, int player_id, int up,
                                int down, int left, int right, int shoot,
                                int pause, int quit) {
  if (!controller)
    return;

  if (player_id == 0) {
    controller->key_up = up;
    controller->key_down = down;
    controller->key_left = left;
    controller->key_right = right;
    controller->key_shoot = shoot;
    controller->key_pause = pause;
    controller->key_quit = quit;
  } else {
    controller->key_p2_up = up;
    controller->key_p2_down = down;
    controller->key_p2_left = left;
    controller->key_p2_right = right;
    controller->key_p2_shoot = shoot;
  }
}

void controller_set_callbacks(Controller *controller, RenderCallback render_cb,
                              AudioCallback audio_cb, void *data) {
  if (!controller)
    return;
  controller->render_callback = render_cb;
  controller->audio_callback = audio_cb;
  controller->callback_data = data;
}

Command controller_translate_input(Controller *controller, InputEvent *event) {
  if (!event || !controller)
    return CMD_NONE;

  switch (event->type) {
  case INPUT_KEYBOARD:
    // Player 1
    if (event->key == controller->key_up)
      return CMD_UP;
    if (event->key == controller->key_down)
      return CMD_DOWN;
    if (event->key == controller->key_left)
      return CMD_LEFT;
    if (event->key == controller->key_right)
      return CMD_RIGHT;
    if (event->key == controller->key_shoot)
      return CMD_SHOOT;
    if (event->key == controller->key_pause)
      return CMD_PAUSE;
    if (event->key == controller->key_quit)
      return CMD_QUIT;

    // Player 2
    if (event->key == controller->key_p2_up)
      return CMD_P2_MOVE_UP;
    if (event->key == controller->key_p2_down)
      return CMD_P2_MOVE_DOWN;
    if (event->key == controller->key_p2_left)
      return CMD_P2_MOVE_LEFT;
    if (event->key == controller->key_p2_right)
      return CMD_P2_MOVE_RIGHT;
    if (event->key == controller->key_p2_shoot)
      return CMD_P2_SHOOT;

    // Hardcoded shortcuts
    if (event->key == 'r' || event->key == 'R')
      return CMD_RESET_GAME;
    if (event->key == 13 || event->key == 10)
      return CMD_CONFIRM; // Enter
    if (event->key == 27)
      return CMD_BACK; // ESC
    break;

  case INPUT_JOYSTICK:
    // Basic joystick mapping (could be improved to be dynamic)
    if (event->button == 0)
      return CMD_SHOOT;
    if (event->button == 1)
      return CMD_PAUSE;
    if (event->axis == 0) {
      if (event->value < -16000)
        return CMD_LEFT;
      if (event->value > 16000)
        return CMD_RIGHT;
    }
    break;
  default:
    break;
  }

  return CMD_NONE;
}

void controller_execute_command(Controller *controller, Command cmd) {
  if (!controller || !controller->model)
    return;

  controller->last_input_time = get_ticks();

  // Menu navigation
  if (controller->model->state == STATE_MENU) {
    switch (cmd) {
    case CMD_UP:
    case CMD_P2_MOVE_UP:
      model_process_menu_input(controller->model, -1);
      return;
    case CMD_DOWN:
    case CMD_P2_MOVE_DOWN:
      model_process_menu_input(controller->model, 1);
      return;
    case CMD_LEFT:
    case CMD_P2_MOVE_LEFT:
      if (controller->model->menu_state == MENU_SETTINGS &&
          controller->model->menu_selection == 1)
        model_adjust_music_volume(controller->model, -1);
      return;
    case CMD_RIGHT:
    case CMD_P2_MOVE_RIGHT:
      if (controller->model->menu_state == MENU_SETTINGS &&
          controller->model->menu_selection == 1)
        model_adjust_music_volume(controller->model, 1);
      return;
    case CMD_CONFIRM:
    case CMD_SHOOT:
    case CMD_START_GAME:
    case CMD_P2_SHOOT:
      model_process_menu_input(controller->model, 0);
      return;
    case CMD_BACK:
      if (controller->model->menu_state == MENU_CONTROLS) {
        controller->model->menu_state = MENU_SETTINGS;
        controller->model->menu_selection = 0;
        controller->model->needs_redraw = true;
      } else if (controller->model->menu_state == MENU_SETTINGS) {
        controller->model->menu_state = MENU_MAIN;
        controller->model->menu_selection = 2;
        controller->model->needs_redraw = true;
      } else if (controller->model->menu_state == MENU_DIFFICULTY) {
        controller->model->menu_state = MENU_MAIN;
        controller->model->menu_selection = 0;
        controller->model->needs_redraw = true;
      } else if (controller->model->menu_state == MENU_MAIN) {
        controller->quit_requested = true;
      }
      return;
    case CMD_QUIT:
      controller->quit_requested = true;
      return;
    default:
      break;
    }
  }
  if (controller->model->state == STATE_LEVEL_TRANSITION) {
    if (cmd == CMD_SHOOT || cmd == CMD_CONFIRM || cmd == CMD_P2_SHOOT) {
      controller->model->state = STATE_PLAYING;
      controller->model->needs_redraw = true;
    }
    return;
  }

  // Handle Game Over / Win
  if (controller->model->state == STATE_GAME_OVER ||
      controller->model->state == STATE_WIN) {
    if (cmd == CMD_RESET_GAME) {
      model_reset_game(controller->model);
      controller->model->state = STATE_PLAYING;
    }
    if (cmd == CMD_BACK) {
      controller->model->state = STATE_MENU;
    }
    return;
  }

  // Active Gameplay Commands
  switch (cmd) {
  // P1 Movement
  case CMD_MOVE_LEFT:
  case CMD_LEFT:
    model_move_player(controller->model, 0, DIR_LEFT);
    break;
  case CMD_MOVE_RIGHT:
  case CMD_RIGHT:
    model_move_player(controller->model, 0, DIR_RIGHT);
    break;
  case CMD_MOVE_UP:
  case CMD_UP:
    model_move_player(controller->model, 0, DIR_UP);
    break;
  case CMD_MOVE_DOWN:
  case CMD_DOWN:
    model_move_player(controller->model, 0, DIR_DOWN);
    break;
  case CMD_SHOOT:
    model_player_shoot(controller->model, 0);
    break;

  // P2 Movement
  case CMD_P2_MOVE_LEFT:
    model_move_player(controller->model, 1, DIR_LEFT);
    break;
  case CMD_P2_MOVE_RIGHT:
    model_move_player(controller->model, 1, DIR_RIGHT);
    break;
  case CMD_P2_MOVE_UP:
    model_move_player(controller->model, 1, DIR_UP);
    break;
  case CMD_P2_MOVE_DOWN:
    model_move_player(controller->model, 1, DIR_DOWN);
    break;
  case CMD_P2_SHOOT:
    model_player_shoot(controller->model, 1);
    break;

  case CMD_PAUSE:
    model_toggle_pause(controller->model);
    break;
  case CMD_RESET_GAME:
    model_reset_game(controller->model);
    break;
  case CMD_BACK:
    model_set_state(controller->model, STATE_MENU);
    break;
  case CMD_QUIT:
    controller->quit_requested = true;
    break;

  default:
    break;
  }

  if (controller->render_callback)
    controller->render_callback(controller->callback_data);
}

void controller_process_input(Controller *controller) {
  if (!controller || !controller->input_handler)
    return;

  Command cmd = CMD_NONE;
  if (input_handler_get_command(controller->input_handler, &cmd)) {
    controller_execute_command(controller, cmd);
  }
  input_handler_update(controller->input_handler, get_ticks());
}

void controller_handle_event(Controller *controller, InputEvent *event) {
  if (!controller || !event)
    return;
  Command cmd = controller_translate_input(controller, event);
  if (cmd != CMD_NONE) {
    controller_execute_command(controller, cmd);
  }
}

void controller_update(Controller *controller, float delta_time) {
  (void)delta_time;
  if (!controller)
    return;
  controller->paused = (controller->model->state == STATE_PAUSED);
}

bool controller_is_quit_requested(Controller *controller) {
  return controller ? controller->quit_requested : true;
}

bool controller_is_paused(Controller *controller) {
  return controller ? controller->paused : false;
}