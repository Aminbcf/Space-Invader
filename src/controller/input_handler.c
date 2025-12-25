#include "input_handler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_SDL_VIEW
#include <SDL2/SDL.h>
#endif

InputHandler* input_handler_create(void) {
    InputHandler* handler = malloc(sizeof(InputHandler));
    if (!handler) return NULL;
    
    memset(handler, 0, sizeof(InputHandler));
    
    // Initialiser l'état clavier
    memset(&handler->keyboard_state, 0, sizeof(InputState));
    handler->keyboard_state.initial_delay = 300;  // 300ms avant répétition
    handler->keyboard_state.repeat_delay = 50;    // 50ms entre répétitions
    
    // Joysticks
    handler->joystick_states = NULL;
    handler->joystick_count = 0;
    
    // Configuration par défaut (touches)
    #ifdef USE_SDL_VIEW
    input_handler_set_keybindings(handler,
                                 SDLK_LEFT, SDLK_RIGHT,
                                 SDLK_SPACE, SDLK_p,
                                 SDLK_ESCAPE);
    #else
    // For ncurses, use character codes
    input_handler_set_keybindings(handler,
                                 'a', 'd',
                                 ' ', 'p',
                                 27); // ESC
    #endif
    
    // Configuration par défaut (joystick)
    input_handler_set_joystick_bindings(handler,
                                       0, 1, 2, 3, 4);
    
    // Seuil pour axes analogiques
    handler->axis_threshold = 0.5f;
    
    // Détecter les joysticks (SDL only)
    #ifdef USE_SDL_VIEW
    input_handler_detect_joysticks(handler);
    #endif
    
    return handler;
}

void input_handler_destroy(InputHandler* handler) {
    if (handler) {
        if (handler->joystick_states) {
            free(handler->joystick_states);
        }
        free(handler);
    }
}

void input_handler_set_keybindings(InputHandler* handler,
                                  SDL_Keycode left, SDL_Keycode right,
                                  SDL_Keycode shoot, SDL_Keycode pause,
                                  SDL_Keycode quit) {
    if (!handler) return;
    
    handler->key_left = left;
    handler->key_right = right;
    handler->key_shoot = shoot;
    handler->key_pause = pause;
    handler->key_quit = quit;
}

void input_handler_set_joystick_bindings(InputHandler* handler,
                                        int left, int right, int shoot,
                                        int pause, int quit) {
    if (!handler) return;
    
    handler->joy_button_left = left;
    handler->joy_button_right = right;
    handler->joy_button_shoot = shoot;
    handler->joy_button_pause = pause;
    handler->joy_button_quit = quit;
}

#ifdef USE_SDL_VIEW
void input_handler_detect_joysticks(InputHandler* handler) {
    if (!handler) return;
    
    // Compter les joysticks (SDL2)
    int count = SDL_NumJoysticks();
    if (count < 0) count = 0;
    
    // Redimensionner le tableau d'états
    if (handler->joystick_states) {
        free(handler->joystick_states);
    }
    
    if (count > 0) {
        handler->joystick_states = malloc(count * sizeof(InputState));
        if (handler->joystick_states) {
            memset(handler->joystick_states, 0, count * sizeof(InputState));
            handler->joystick_count = count;
        } else {
            handler->joystick_count = 0;
        }
    } else {
        handler->joystick_states = NULL;
        handler->joystick_count = 0;
    }
}
#else
void input_handler_detect_joysticks(InputHandler* handler) {
    if (!handler) return;
    // No joystick support for ncurses
    handler->joystick_states = NULL;
    handler->joystick_count = 0;
}
#endif

void input_handler_process_sdl_event(InputHandler* handler, SDL_Event* event) {
#ifdef USE_SDL_VIEW
    if (!handler || !event) return;
    
    switch (event->type) {
        case SDL_KEYDOWN:
            // Mettre à jour l'état des touches
            if (event->key.keysym.sym == handler->key_left) {
                handler->keyboard_state.left_pressed = true;
            } else if (event->key.keysym.sym == handler->key_right) {
                handler->keyboard_state.right_pressed = true;
            } else if (event->key.keysym.sym == handler->key_shoot) {
                handler->keyboard_state.shoot_pressed = true;
            } else if (event->key.keysym.sym == handler->key_pause) {
                handler->keyboard_state.pause_pressed = true;
            } else if (event->key.keysym.sym == handler->key_quit) {
                handler->keyboard_state.quit_pressed = true;
            }
            break;
            
        case SDL_KEYUP:
            // Mettre à jour l'état des touches
            if (event->key.keysym.sym == handler->key_left) {
                handler->keyboard_state.left_pressed = false;
                handler->keyboard_state.left_was_pressed = false;
                handler->keyboard_state.left_repeat_time = 0;
            } else if (event->key.keysym.sym == handler->key_right) {
                handler->keyboard_state.right_pressed = false;
                handler->keyboard_state.right_was_pressed = false;
                handler->keyboard_state.right_repeat_time = 0;
            } else if (event->key.keysym.sym == handler->key_shoot) {
                handler->keyboard_state.shoot_pressed = false;
                handler->keyboard_state.shoot_was_pressed = false;
                handler->keyboard_state.shoot_repeat_time = 0;
            } else if (event->key.keysym.sym == handler->key_pause) {
                handler->keyboard_state.pause_pressed = false;
                handler->keyboard_state.pause_was_pressed = false;
            } else if (event->key.keysym.sym == handler->key_quit) {
                handler->keyboard_state.quit_pressed = false;
            }
            break;
            
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
            // Re-détecter les joysticks
            input_handler_detect_joysticks(handler);
            break;
            
        case SDL_JOYBUTTONDOWN:
            input_handler_handle_joystick_event(handler, event);
            break;
            
        case SDL_JOYBUTTONUP:
            input_handler_handle_joystick_event(handler, event);
            break;
            
        case SDL_JOYAXISMOTION:
            input_handler_handle_joystick_event(handler, event);
            break;
            
        default:
            break;
    }
#endif
}

#ifdef USE_SDL_VIEW
void input_handler_handle_joystick_event(InputHandler* handler, SDL_Event* event) {
    if (!handler || !event) return;
    
    int joystick_id = event->jbutton.which;
    
    if (joystick_id < 0 || joystick_id >= handler->joystick_count) {
        return;
    }
    
    InputState* joy_state = &handler->joystick_states[joystick_id];
    
    switch (event->type) {
        case SDL_JOYBUTTONDOWN:
            if (event->jbutton.button == handler->joy_button_left) {
                joy_state->left_pressed = true;
            } else if (event->jbutton.button == handler->joy_button_right) {
                joy_state->right_pressed = true;
            } else if (event->jbutton.button == handler->joy_button_shoot) {
                joy_state->shoot_pressed = true;
            } else if (event->jbutton.button == handler->joy_button_pause) {
                joy_state->pause_pressed = true;
            } else if (event->jbutton.button == handler->joy_button_quit) {
                joy_state->quit_pressed = true;
            }
            break;
            
        case SDL_JOYBUTTONUP:
            if (event->jbutton.button == handler->joy_button_left) {
                joy_state->left_pressed = false;
                joy_state->left_was_pressed = false;
            } else if (event->jbutton.button == handler->joy_button_right) {
                joy_state->right_pressed = false;
                joy_state->right_was_pressed = false;
            } else if (event->jbutton.button == handler->joy_button_shoot) {
                joy_state->shoot_pressed = false;
                joy_state->shoot_was_pressed = false;
            } else if (event->jbutton.button == handler->joy_button_pause) {
                joy_state->pause_pressed = false;
                joy_state->pause_was_pressed = false;
            } else if (event->jbutton.button == handler->joy_button_quit) {
                joy_state->quit_pressed = false;
            }
            break;
            
        case SDL_JOYAXISMOTION:
            // Gestion des axes analogiques (simplifiée)
            break;
            
        default:
            break;
    }
}
#endif

void input_handler_process_ncurses_input(InputHandler* handler, int ch) {
    if (!handler) return;
    
    // Réinitialiser tous les états
    handler->keyboard_state.left_pressed = false;
    handler->keyboard_state.right_pressed = false;
    handler->keyboard_state.shoot_pressed = false;
    handler->keyboard_state.pause_pressed = false;
    handler->keyboard_state.quit_pressed = false;
    
    // Traduire les codes ncurses
    switch (ch) {
        case 'a':
        case 'A':
        //case KEY_LEFT:
            handler->keyboard_state.left_pressed = true;
            break;

        case 'd':
        case 'D':
        //case KEY_RIGHT:
            handler->keyboard_state.right_pressed = true;
            break;
            
        case ' ':
        case '\n':
            handler->keyboard_state.shoot_pressed = true;
            break;
            
        case 'p':
        case 'P':
            handler->keyboard_state.pause_pressed = true;
            break;
            
        case 'q':
        case 'Q':
        case 27:  // ESC
            handler->keyboard_state.quit_pressed = true;
            break;
            
        default:
            break;
    }
}

void input_handler_update(InputHandler* handler, uint32_t current_time) {
    if (!handler) return;
    
    // Gestion de la répétition de touches
    InputState* state = &handler->keyboard_state;
    
    // Touche gauche
    if (state->left_pressed) {
        if (!state->left_was_pressed) {
            // Première pression
            state->left_was_pressed = true;
            state->left_repeat_time = current_time + state->initial_delay;
        } else if (current_time >= state->left_repeat_time) {
            // Répétition
            state->left_repeat_time = current_time + state->repeat_delay;
        }
    }
    
    // Touche droite
    if (state->right_pressed) {
        if (!state->right_was_pressed) {
            state->right_was_pressed = true;
            state->right_repeat_time = current_time + state->initial_delay;
        } else if (current_time >= state->right_repeat_time) {
            state->right_repeat_time = current_time + state->repeat_delay;
        }
    }
    
    // Touche tir
    if (state->shoot_pressed) {
        if (!state->shoot_was_pressed) {
            state->shoot_was_pressed = true;
            state->shoot_repeat_time = current_time + state->initial_delay;
        } else if (current_time >= state->shoot_repeat_time) {
            state->shoot_repeat_time = current_time + state->repeat_delay;
        }
    }
}

bool input_handler_get_command(InputHandler* handler, Command* cmd) {
    if (!handler || !cmd) return false;
    
    *cmd = CMD_NONE;
    
    // Vérifier les touches clavier
    InputState* state = &handler->keyboard_state;
    
    if (state->quit_pressed) {
        *cmd = CMD_QUIT;
        state->quit_pressed = false;  // Consommer l'entrée
        return true;
    }
    
    if (state->pause_pressed && !state->pause_was_pressed) {
        *cmd = CMD_PAUSE;
        state->pause_was_pressed = true;
        state->pause_pressed = false;  // Consommer l'entrée
        return true;
    }
    
    if (state->left_pressed && !state->left_was_pressed) {
        *cmd = CMD_MOVE_LEFT;
        return true;
    }
    
    if (state->right_pressed && !state->right_was_pressed) {
        *cmd = CMD_MOVE_RIGHT;
        return true;
    }
    
    if (state->shoot_pressed && !state->shoot_was_pressed) {
        *cmd = CMD_SHOOT;
        return true;
    }
    
    // Vérifier les joysticks (SDL only)
    #ifdef USE_SDL_VIEW
    for (int i = 0; i < handler->joystick_count; i++) {
        InputState* joy_state = &handler->joystick_states[i];
        
        if (joy_state->quit_pressed) {
            *cmd = CMD_QUIT;
            joy_state->quit_pressed = false;  // Consommer l'entrée
            return true;
        }
        
        if (joy_state->pause_pressed && !joy_state->pause_was_pressed) {
            *cmd = CMD_PAUSE;
            joy_state->pause_was_pressed = true;
            joy_state->pause_pressed = false;
            return true;
        }
        
        if (joy_state->left_pressed && !joy_state->left_was_pressed) {
            *cmd = CMD_MOVE_LEFT;
            joy_state->left_was_pressed = true;
            return true;
        }
        
        if (joy_state->right_pressed && !joy_state->right_was_pressed) {
            *cmd = CMD_MOVE_RIGHT;
            joy_state->right_was_pressed = true;
            return true;
        }
        
        if (joy_state->shoot_pressed && !joy_state->shoot_was_pressed) {
            *cmd = CMD_SHOOT;
            joy_state->shoot_was_pressed = true;
            return true;
        }
    }
    #endif
    
    return false;
}

#ifdef USE_SDL_VIEW
bool input_handler_is_key_pressed(InputHandler* handler, SDL_Keycode key) {
    if (!handler) return false;
    
    // SDL2: obtenir l'état du clavier
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    
    if (scancode != SDL_SCANCODE_UNKNOWN) {
        return keyboard_state[scancode] != 0;
    }
    
    return false;
}

bool input_handler_was_key_pressed(InputHandler* handler, SDL_Keycode key) {
    // Pour une détection de front, nous aurions besoin de garder
    // l'état précédent. Cette fonction est simplifiée.
    return input_handler_is_key_pressed(handler, key);
}

float input_handler_get_joystick_axis(InputHandler* handler, 
                                      int joystick_id, int axis) {
    if (!handler || joystick_id < 0 || 
        joystick_id >= handler->joystick_count) {
        return 0.0f;
    }
    
    // SDL2: obtenir le joystick par index
    SDL_Joystick* joystick = SDL_JoystickOpen(joystick_id);
    if (!joystick) return 0.0f;
    
    Sint16 value = SDL_JoystickGetAxis(joystick, axis);
    SDL_JoystickClose(joystick);
    
    return value / 32768.0f;
}

bool input_handler_get_joystick_button(InputHandler* handler,
                                      int joystick_id, int button) {
    if (!handler || joystick_id < 0 || 
        joystick_id >= handler->joystick_count) {
        return false;
    }
    
    // SDL2: obtenir le joystick par index
    SDL_Joystick* joystick = SDL_JoystickOpen(joystick_id);
    if (!joystick) return false;
    
    bool pressed = SDL_JoystickGetButton(joystick, button) != 0;
    SDL_JoystickClose(joystick);
    
    return pressed;
}
#endif

bool input_handler_save_config(InputHandler* handler, const char* filename) {
    if (!handler || !filename) return false;
    
    FILE* file = fopen(filename, "w");
    if (!file) return false;
    
    fprintf(file, "# Configuration des contrôles Space Invaders\n");
    fprintf(file, "key_left=%d\n", handler->key_left);
    fprintf(file, "key_right=%d\n", handler->key_right);
    fprintf(file, "key_shoot=%d\n", handler->key_shoot);
    fprintf(file, "key_pause=%d\n", handler->key_pause);
    fprintf(file, "key_quit=%d\n", handler->key_quit);
    
    fclose(file);
    return true;
}

bool input_handler_load_config(InputHandler* handler, const char* filename) {
    if (!handler || !filename) return false;
    
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    
    char line[256];
    SDL_Keycode left = SDLK_LEFT;
    SDL_Keycode right = SDLK_RIGHT;
    SDL_Keycode shoot = SDLK_SPACE;
    SDL_Keycode pause = SDLK_p;
    SDL_Keycode quit = SDLK_ESCAPE;
    
    while (fgets(line, sizeof(line), file)) {
        // Ignorer les commentaires et lignes vides
        if (line[0] == '#' || line[0] == '\n') continue;
        
        int value;
        
        if (sscanf(line, "key_left=%d", &value) == 1) {
            left = (SDL_Keycode)value;
        } else if (sscanf(line, "key_right=%d", &value) == 1) {
            right = (SDL_Keycode)value;
        } else if (sscanf(line, "key_shoot=%d", &value) == 1) {
            shoot = (SDL_Keycode)value;
        } else if (sscanf(line, "key_pause=%d", &value) == 1) {
            pause = (SDL_Keycode)value;
        } else if (sscanf(line, "key_quit=%d", &value) == 1) {
            quit = (SDL_Keycode)value;
        }
    }
    
    fclose(file);
    
    // Appliquer la configuration
    input_handler_set_keybindings(handler, left, right, shoot, pause, quit);
    
    return true;
}
