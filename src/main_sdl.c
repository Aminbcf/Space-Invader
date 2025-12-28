#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "core/model.h"
#include "core/game_state.h"
#include "controller/controller.h"
#include "views/view_sdl.h"





int main(int argc, char* argv[]) {
    printf("Space Invaders MVC - SDL2 Version\n");

    (void)argc;  // Mark as unused
    (void)argv;  // Mark as unused
    
    // Create game context
    GameContext* context = game_context_create();
    if (!context) {
        fprintf(stderr, "Failed to create game context\n");
        return 1;
    }
    
    // Create controller
    Controller* controller = controller_create(context->model);
    if (!controller) {
        fprintf(stderr, "Failed to create controller\n");
        game_context_destroy(context);
        return 1;
    }
    
    // Create SDL view
    SDLView* view = sdl_view_create();
    if (!view) {
        fprintf(stderr, "Failed to create SDL view\n");
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    // Initialize view
    if (!sdl_view_init(view, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        fprintf(stderr, "Failed to initialize SDL view\n");
        sdl_view_destroy(view);
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    // Set view context for controller
    controller_set_view_context(controller, view);
    
    // Main game loop
    SDL_Event event;
    uint32_t last_update = SDL_GetTicks();
    const int TARGET_FPS = 60;
    const float FRAME_DELAY = 1000.0f / TARGET_FPS;
    
    printf("Game started. Controls:\n");
    printf("  Left/Right Arrow or A/D: Move\n");
    printf("  Space: Shoot\n");
    printf("  P: Pause\n");
    printf("  R: Restart (when game over)\n");
    printf("  ESC: Quit\n\n");
    
    while (!controller_is_quit_requested(controller)) {
        uint32_t frame_start = SDL_GetTicks();
        
        // Handle SDL events
        while (sdl_view_poll_event(view, &event)) {
            if (event.type == SDL_QUIT) {
                controller_execute_command(controller, CMD_QUIT);
            } else if (event.type == SDL_KEYDOWN) {
                InputEvent input_event;
                input_event.type = INPUT_KEYBOARD;
                input_event.key = event.key.keysym.sym;
                input_event.scancode = event.key.keysym.scancode;
                input_event.mod = event.key.keysym.mod;
                input_event.x = 0;
                input_event.y = 0;
                input_event.button = 0;
                input_event.joy_id = 0;
                input_event.axis = 0;
                input_event.value = 0;
                controller_handle_event(controller, &input_event);
            }
        }
        
        // Process continuous input (held keys)
        controller_process_input(controller);
        
        // Calculate delta time
        uint32_t current_time = SDL_GetTicks();
        float delta_time = (current_time - last_update) / 1000.0f;
        
        // Cap delta time to prevent huge jumps
        if (delta_time > 0.1f) delta_time = 0.1f;
        
        last_update = current_time;
        
        // Update game
        controller_update(controller, delta_time);
        game_context_update(context);
        
        // Render
        sdl_view_render(view, context->model);
        
        // Cap framerate
        uint32_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay((uint32_t)(FRAME_DELAY - frame_time));
        }
    }
    
    // Cleanup
    printf("Cleaning up...\n");
    sdl_view_destroy(view);
    controller_destroy(controller);
    game_context_destroy(context);
    SDL_Quit();
    
    printf("Game ended. Thanks for playing!\n");
    return 0;
}