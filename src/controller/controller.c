#include "controller.h"
#include "input_handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "commands.h"

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
    
    // Configuration par défaut
    controller_set_keybindings(controller, 
                              SDLK_LEFT, SDLK_RIGHT, 
                              SDLK_SPACE, SDLK_p, 
                              SDLK_ESCAPE);
    
    // Callbacks
    controller->render_callback = NULL;
    controller->audio_callback = NULL;
    controller->callback_data = NULL;
    
    // État
    controller->quit_requested = false;
    controller->paused = false;
    controller->last_input_time = SDL_GetTicks();
    
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
    controller->view_context = view_context;
}

void controller_set_keybindings(Controller* controller, 
                               int left, int right, int shoot, 
                               int pause, int quit) {
    controller->key_left = left;
    controller->key_right = right;
    controller->key_shoot = shoot;
    controller->key_pause = pause;
    controller->key_quit = quit;
    
    if (controller->input_handler) {
        input_handler_set_keybindings(controller->input_handler,
                                     left, right, shoot, pause, quit);
    }
}

void controller_set_callbacks(Controller* controller,
                             RenderCallback render_cb,
                             AudioCallback audio_cb,
                             void* data) {
    controller->render_callback = render_cb;
    controller->audio_callback = audio_cb;
    controller->callback_data = data;
}

Command controller_translate_input(InputEvent* event) {
    if (!event) return CMD_NONE;
    
    switch (event->type) {
        case INPUT_KEYBOARD:
            switch (event->key) {
                case SDLK_LEFT:
                case SDLK_a:
                    return CMD_MOVE_LEFT;
                case SDLK_RIGHT:
                case SDLK_d:
                    return CMD_MOVE_RIGHT;
                case SDLK_SPACE:
                    return CMD_SHOOT;
                case SDLK_p:
                case SDLK_PAUSE:
                    return CMD_PAUSE;
                case SDLK_ESCAPE:
                    return CMD_QUIT;
                case SDLK_r:
                    return CMD_RESET_GAME;
                case SDLK_RETURN:
                    if (event->mod & KMOD_ALT) {
                        return CMD_TOGGLE_VIEW;
                    }
                    return CMD_CONFIRM;
                case SDLK_UP:
                case SDLK_w:
                    return CMD_UP;
                case SDLK_DOWN:
                case SDLK_s:
                    return CMD_DOWN;
                case SDLK_F1:
                    return CMD_START_GAME;
                default:
                    return CMD_NONE;
            }
            break;
            
        case INPUT_JOYSTICK:
            if (event->button == 0) return CMD_SHOOT;      // A
            if (event->button == 1) return CMD_PAUSE;      // B
            if (event->button == 2) return CMD_RESET_GAME; // X
            if (event->button == 3) return CMD_QUIT;       // Y
            
            // Axes pour mouvement
            if (event->axis == 0) {  // Axe X
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
    
    controller->last_input_time = SDL_GetTicks();
    
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
            // Navigation menu (implémenté dans la vue)
            break;
            
        case CMD_DOWN:
            // Navigation menu (implémenté dans la vue)
            break;
            
        case CMD_CONFIRM:
            // Confirmation menu (implémenté dans la vue)
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
    
    // Appeler le callback de rendu si défini
    if (controller->render_callback) {
        controller->render_callback(controller->callback_data);
    }
    
    // Appeler le callback audio si défini
    if (controller->audio_callback) {
        controller->audio_callback(controller->callback_data);
    }
}

void controller_process_input(Controller* controller) {
    if (!controller || !controller->input_handler) return;
    
    Command cmd = CMD_NONE;
    
    // Vérifier les entrées du gestionnaire
    if (input_handler_get_command(controller->input_handler, &cmd)) {
        controller_execute_command(controller, cmd);
    }
    
    // Mise à jour du gestionnaire d'entrées
    input_handler_update(controller->input_handler, SDL_GetTicks());
}

void controller_handle_event(Controller* controller, InputEvent* event) {
    if (!controller || !event) return;
    
    // Traduire l'événement en commande
    Command cmd = controller_translate_input(event);
    
    if (cmd != CMD_NONE) {
        controller_execute_command(controller, cmd);
    }
}

void controller_update(Controller* controller, float delta_time) {
    if (!controller) return;
    
    // Vérifier l'état de pause
    controller->paused = (controller->model->state == STATE_PAUSED);
    
    // Si en pause, ne pas mettre à jour la logique
    if (controller->paused) return;
    
    // Mise à jour du modèle (gérée par le contexte principal)
    // controller->model est mis à jour par le contexte de jeu
}

bool controller_is_quit_requested(Controller* controller) {
    return controller ? controller->quit_requested : true;
}

bool controller_is_paused(Controller* controller) {
    return controller ? controller->paused : false;
}