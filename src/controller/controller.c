#include "controller.h"
#include "input_handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "commands.h"

/* Simple timer using standard C */
static uint32_t get_ticks(void) {
    return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

Controller* controller_create(GameModel* model) {
    Controller* controller = malloc(sizeof(Controller));
    if (!controller) return NULL;
    
    memset(controller, 0, sizeof(Controller));
    controller->model = model;
    controller->view_context = NULL;
    controller->input_handler = input_handler_create();
    
    if (!controller->input_handler) {
        free(controller);
        return NULL;
    }
    
    /* Default configuration - keys are already set in input_handler_create */
    
    /* Callbacks */
    controller->render_callback = NULL;
    controller->audio_callback = NULL;
    controller->callback_data = NULL;
    
    /* State */
    controller->quit_requested = false;
    controller->paused = false;
    controller->last_input_time = get_ticks();
    
    return controller;
}

void controller_destroy(Controller* controller) {
    if (controller) {
        if (controller->input_handler) {
            input_handler_destroy(controller->input_handler);
        }
        free(controller);
    }
}

void controller_set_view_context(Controller* controller, void* view_context) {
    if (controller) {
        controller->view_context = view_context;
    }
}

void controller_set_keybindings(Controller* controller, 
                                int left, int right, int shoot, 
                                int pause, int quit) {
    if (!controller || !controller->input_handler) return;
    
    controller->key_left = left;
    controller->key_right = right;
    controller->key_shoot = shoot;
    controller->key_pause = pause;
    controller->key_quit = quit;
    
#ifdef USE_SDL_VIEW
    input_handler_set_keybindings(controller->input_handler,
                                 (SDL_Keycode)left, (SDL_Keycode)right,
                                 (SDL_Keycode)shoot, (SDL_Keycode)pause,
                                 (SDL_Keycode)quit);
#else
    input_handler_set_keybindings(controller->input_handler,
                                 left, right, shoot, pause, quit);
#endif
}

void controller_set_callbacks(Controller* controller,
                             RenderCallback render_cb,
                             AudioCallback audio_cb,
                             void* data) {
    if (!controller) return;
    
    controller->render_callback = render_cb;
    controller->audio_callback = audio_cb;
    controller->callback_data = data;
}

Command controller_translate_input(InputEvent* event) {
    if (!event) return CMD_NONE;
    
    switch (event->type) {
        case INPUT_KEYBOARD:
            /* Use simple keycodes */
            if (event->key == 'a' || event->key == 'A' || event->key == 260) return CMD_MOVE_LEFT;
            if (event->key == 'd' || event->key == 'D' || event->key == 261) return CMD_MOVE_RIGHT;
            if (event->key == ' ' || event->key == '\n') return CMD_SHOOT;
            if (event->key == 'p' || event->key == 'P') return CMD_PAUSE;
            if (event->key == 27 || event->key == 'q' || event->key == 'Q') return CMD_QUIT;
            if (event->key == 'r' || event->key == 'R') return CMD_RESET_GAME;
            if (event->key == '1') return CMD_START_GAME;
            
            // Arrow keys (ncurses style)
            if (event->key == 259 || event->key == 'w' || event->key == 'W') return CMD_UP;
            if (event->key == 258 || event->key == 's' || event->key == 'S') return CMD_DOWN;
            
            break;
            
        case INPUT_JOYSTICK:
            if (event->button == 0) return CMD_SHOOT;
            if (event->button == 1) return CMD_PAUSE;
            if (event->button == 2) return CMD_RESET_GAME;
            if (event->button == 3) return CMD_QUIT;
            
            if (event->axis == 0) {
                if (event->value < -16000) return CMD_MOVE_LEFT;
                if (event->value > 16000) return CMD_MOVE_RIGHT;
            }
            break;
            
        default:
            break;
    }
    
    return CMD_NONE;
}

void controller_execute_command(Controller* controller, Command cmd) {
    if (!controller || !controller->model) return;
    
    controller->last_input_time = get_ticks();
    
    switch (cmd) {
        case CMD_MOVE_LEFT:
            if (controller->model->state == STATE_PLAYING) {
                model_move_player(controller->model, DIR_LEFT);
            }
            break;
            
        case CMD_MOVE_RIGHT:
            if (controller->model->state == STATE_PLAYING) {
                model_move_player(controller->model, DIR_RIGHT);
            }
            break;
            
        case CMD_SHOOT:
            if (controller->model->state == STATE_PLAYING) {
                model_player_shoot(controller->model);
            } else if (controller->model->state == STATE_MENU) {
                model_set_state(controller->model, STATE_PLAYING);
            } else if (controller->model->state == STATE_GAME_OVER) {
                model_reset_game(controller->model);
            }
            break;
            
        case CMD_PAUSE:
            if (controller->model->state == STATE_PLAYING || 
                controller->model->state == STATE_PAUSED) {
                model_toggle_pause(controller->model);
            }
            break;
            
        case CMD_START_GAME:
            if (controller->model->state == STATE_MENU) {
                model_set_state(controller->model, STATE_PLAYING);
            }
            break;
            
        case CMD_QUIT:
            controller->quit_requested = true;
            break;
            
        case CMD_RESET_GAME:
            if (controller->model->state == STATE_GAME_OVER ||
                controller->model->state == STATE_PLAYING) {
                model_reset_game(controller->model);
            }
            break;
            
        case CMD_UP:
        case CMD_DOWN:
        case CMD_CONFIRM:
            /* Menu navigation logic */
            break;
            
        case CMD_BACK:
            if (controller->model->state == STATE_PLAYING ||
                controller->model->state == STATE_PAUSED) {
                model_set_state(controller->model, STATE_MENU);
            }
            break;
            
        default:
            break;
    }
    
    if (controller->render_callback) {
        controller->render_callback(controller->callback_data);
    }
    
    if (controller->audio_callback) {
        controller->audio_callback(controller->callback_data);
    }
}

void controller_process_input(Controller* controller) {
    if (!controller || !controller->input_handler) return;
    
    Command cmd = CMD_NONE;
    
    /* Check for commands from input handler */
    if (input_handler_get_command(controller->input_handler, &cmd)) {
        controller_execute_command(controller, cmd);
    }
    
    /* Update input handler timing */
    input_handler_update(controller->input_handler, get_ticks());
}

void controller_handle_event(Controller* controller, InputEvent* event) {
    if (!controller || !event) return;
    
    Command cmd = controller_translate_input(event);
    
    if (cmd != CMD_NONE) {
        controller_execute_command(controller, cmd);
    }
}

void controller_update(Controller* controller, float delta_time) {
    (void)delta_time;
    if (!controller) return;
    
    controller->paused = (controller->model->state == STATE_PAUSED);
}

bool controller_is_quit_requested(Controller* controller) {
    return controller ? controller->quit_requested : true;
}

bool controller_is_paused(Controller* controller) {
    return controller ? controller->paused : false;
}