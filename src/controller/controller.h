#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "../core/model.h"
#include "commands.h"
#include "input_handler.h"

// Événements d'entrée
typedef enum {
    INPUT_KEYBOARD,
    INPUT_MOUSE,
    INPUT_JOYSTICK,
    INPUT_TOUCH
} InputType;

// Structure pour les données d'entrée
typedef struct {
    InputType type;
    int key;      // Code de touche
    int scancode;         // Scan code
    int mod;       // Modificateurs
    int x, y;             // Coordonnées (pour souris/touch)
    int button;           // Bouton souris
    int joy_id;           // ID joystick
    int axis;             // Axe joystick
    int value;            // Valeur axe/bouton
} InputEvent;

// Callback pour les événements de rendu
typedef void (*RenderCallback)(void* data);
typedef void (*AudioCallback)(void* data);

// Structure du contrôleur
typedef struct {
    GameModel* model;
    void* view_context;
    InputHandler* input_handler;
    
    // Callbacks
    RenderCallback render_callback;
    AudioCallback audio_callback;
    void* callback_data;
    
    // Configuration
    int key_left;
    int key_right;
    int key_shoot;
    int key_pause;
    int key_quit;
    
    // État
    bool quit_requested;
    bool paused;
    uint32_t last_input_time;
} Controller;

// Initialisation et gestion
Controller* controller_create(GameModel* model);
void controller_destroy(Controller* controller);
void controller_set_view_context(Controller* controller, void* view_context);

// Gestion des entrées
void controller_process_input(Controller* controller);
void controller_handle_event(Controller* controller, InputEvent* event);
Command controller_translate_input(InputEvent* event);

// Exécution des commandes
void controller_execute_command(Controller* controller, Command cmd);
void controller_update(Controller* controller, float delta_time);

// Configuration
void controller_set_keybindings(Controller* controller, 
                                int left, int right, int shoot, 
                                int pause, int quit);
void controller_set_callbacks(Controller* controller,
                             RenderCallback render_cb,
                             AudioCallback audio_cb,
                             void* data);

// Getters
bool controller_is_quit_requested(Controller* controller);
bool controller_is_paused(Controller* controller);

#endif // CONTROLLER_H