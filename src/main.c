#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "core/model.h"
#include "core/game_state.h"
#include "controller/controller.h"
#include "views/view_sdl.h"

int main(int argc, char* argv[]) {
    printf("Space Invaders MVC - SDL2 Version\n");
    
    // Créer le contexte du jeu
    GameContext* context = game_context_create();
    if (!context) {
        fprintf(stderr, "Failed to create game context\n");
        return 1;
    }
    
    // Créer le contrôleur
    Controller* controller = controller_create(context->model);
    if (!controller) {
        fprintf(stderr, "Failed to create controller\n");
        game_context_destroy(context);
        return 1;
    }
    
    // Créer la vue SDL
    SDLView* view = sdl_view_create();
    if (!view) {
        fprintf(stderr, "Failed to create SDL view\n");
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    // Initialiser la vue
    if (!sdl_view_init(view, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        fprintf(stderr, "Failed to initialize SDL view\n");
        sdl_view_destroy(view);
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    // Configurer le contrôleur avec la vue
    controller_set_view_context(controller, view);
    
    // Boucle principale du jeu
    SDL_Event event;
    uint32_t last_update = SDL_GetTicks();
    const int TARGET_FPS = 60;
    const float FRAME_DELAY = 1000.0f / TARGET_FPS;
    
    printf("Game started. Controls:\n");
    printf("  Left/Right: Move\n");
    printf("  Space: Shoot\n");
    printf("  P: Pause\n");
    printf("  ESC: Quit\n");
    
    while (!controller_is_quit_requested(controller)) {
        uint32_t frame_start = SDL_GetTicks();
        
        // Gérer les événements SDL
        while (sdl_view_poll_event(view, &event)) {
            if (event.type == SDL_QUIT) {
                controller_execute_command(controller, CMD_QUIT);
            } else if (event.type == SDL_KEYDOWN) {
                InputEvent input_event;
                input_event.type = INPUT_KEYBOARD;
                input_event.key = event.key.keysym.sym;
                input_event.mod = event.key.keysym.mod;
                controller_handle_event(controller, &input_event);
            }
        }
        
        // Traiter les entrées continues (touches maintenues)
        controller_process_input(controller);
        
        // Calculer le delta time
        uint32_t current_time = SDL_GetTicks();
        float delta_time = (current_time - last_update) / 1000.0f;
        last_update = current_time;
        
        // Mettre à jour le jeu
        controller_update(controller, delta_time);
        game_context_update(context);
        
        // Rendu
        sdl_view_render(view, context->model);
        
        // Limiter le framerate
        uint32_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }
    
    // Nettoyage
    printf("Cleaning up...\n");
    sdl_view_destroy(view);
    controller_destroy(controller);
    game_context_destroy(context);
    SDL_Quit();
    
    printf("Game ended.\n");
    return 0;
}