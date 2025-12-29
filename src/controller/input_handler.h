#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include "commands.h"

#ifdef USE_SDL_VIEW
#include <SDL3/SDL.h>
#endif

// Structure pour gérer l'état des touches
typedef struct {
    bool left_pressed;
    bool right_pressed;
    bool shoot_pressed;
    bool pause_pressed;
    bool quit_pressed;
    
    // État précédent pour détection de front
    bool left_was_pressed;
    bool right_was_pressed;
    bool shoot_was_pressed;
    bool pause_was_pressed;
    
    // Temps de répétition
    uint32_t left_repeat_time;
    uint32_t right_repeat_time;
    uint32_t shoot_repeat_time;
    
    // Configuration des délais (ms)
    uint32_t initial_delay;
    uint32_t repeat_delay;
} InputState;

// Gestionnaire d'entrées abstrait
typedef struct {
    InputState keyboard_state;
    InputState* joystick_states;
    int joystick_count;
    
    // Mappage des touches
#ifdef USE_SDL_VIEW
    SDL_Keycode key_left;
    SDL_Keycode key_right;
    SDL_Keycode key_shoot;
    SDL_Keycode key_pause;
    SDL_Keycode key_quit;
#else
    int key_left;
    int key_right;
    int key_shoot;
    int key_pause;
    int key_quit;
#endif
    
    // Mappage des boutons joystick
    int joy_button_left;
    int joy_button_right;
    int joy_button_shoot;
    int joy_button_pause;
    int joy_button_quit;
    
    // Sensibilité des axes
    float axis_threshold;
} InputHandler;

// Création et destruction
InputHandler* input_handler_create(void);
void input_handler_destroy(InputHandler* handler);

// Configuration
#ifdef USE_SDL_VIEW
void input_handler_set_keybindings(InputHandler* handler,
                                  SDL_Keycode left, SDL_Keycode right,
                                  SDL_Keycode shoot, SDL_Keycode pause,
                                  SDL_Keycode quit);
#else
void input_handler_set_keybindings(InputHandler* handler,
                                  int left, int right,
                                  int shoot, int pause,
                                  int quit);
#endif

void input_handler_set_joystick_bindings(InputHandler* handler,
                                        int left, int right, int shoot,
                                        int pause, int quit);

// Traitement des événements
#ifdef USE_SDL_VIEW
void input_handler_process_sdl_event(InputHandler* handler, const SDL_Event* event);
void input_handler_handle_joystick_event(InputHandler* handler, const SDL_Event* event);
#else
void input_handler_process_event(InputHandler* handler, void* event);
#endif

void input_handler_process_ncurses_input(InputHandler* handler, int ch);
void input_handler_update(InputHandler* handler, uint32_t current_time);

// Récupération de l'état
bool input_handler_get_command(InputHandler* handler, Command* cmd);

#ifdef USE_SDL_VIEW
bool input_handler_is_key_pressed(InputHandler* handler, SDL_Keycode key);
bool input_handler_was_key_pressed(InputHandler* handler, SDL_Keycode key);
#else
bool input_handler_is_key_pressed(InputHandler* handler, int key);
bool input_handler_was_key_pressed(InputHandler* handler, int key);
#endif

// Joystick
void input_handler_detect_joysticks(InputHandler* handler);

// Gestion des axes analogiques
float input_handler_get_joystick_axis(InputHandler* handler, 
                                      int joystick_id, int axis);
bool input_handler_get_joystick_button(InputHandler* handler,
                                      int joystick_id, int button);

// Sauvegarde/chargement de la configuration
bool input_handler_save_config(InputHandler* handler, const char* filename);
bool input_handler_load_config(InputHandler* handler, const char* filename);

#endif // INPUT_HANDLER_H