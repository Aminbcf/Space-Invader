#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "core/model.h"
#include "core/game_state.h"
#include "controller/controller.h"
#include "controller/input_handler.h"
#include "views/view_sdl.h"

int main(int argc, char* argv[]) {
    printf("Space Invaders MVC - SDL3 Version\n");

    (void)argc;
    (void)argv;
    
    /* Create game context */
    GameContext* context = game_context_create();
    if (!context) {
        fprintf(stderr, "Failed to create game context\n");
        return 1;
    }
    
    /* Create controller */
    Controller* controller = controller_create(context->model);
    if (!controller) {
        fprintf(stderr, "Failed to create controller\n");
        game_context_destroy(context);
        return 1;
    }
    
    /* Create SDL view */
    SDLView* view = sdl_view_create();
    if (!view) {
        fprintf(stderr, "Failed to create SDL view\n");
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    /* Initialize view */
    if (!sdl_view_init(view, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        fprintf(stderr, "Failed to initialize SDL view\n");
        sdl_view_destroy(view);
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    /* Set view context */
    controller_set_view_context(controller, view);
    
    /* Game loop timing */
    const int TARGET_FPS = 60;
    const uint32_t FRAME_DELAY = 1000 / TARGET_FPS;
    uint32_t last_update = SDL_GetTicks();
    
    /* Main loop */
    bool running = true;
    SDL_Event event;
    
    printf("Game Running. Controls: Arrows/WASD to Move, Space to Shoot, P to Pause, R to Restart.\n");

    while (running && !controller_is_quit_requested(controller)) {
        uint32_t frame_start = SDL_GetTicks();
        
        /* Handle SDL events */
        while (sdl_view_poll_event(view, &event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            // FIX: Only handle KEY_DOWN events. 
            // Ncurses only sends "key pressed", so SDL must behave the same way 
            // to work with the shared controller logic. Ignoring KEY_UP prevents
            // the "toggle on press, toggle back on release" glitch.
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                InputEvent input_event;
                memset(&input_event, 0, sizeof(InputEvent));
                
                input_event.type = INPUT_KEYBOARD;
                
                // MAP SDL KEYS TO CONTROLLER EXPECTED VALUES
                if (event.key.key == SDLK_LEFT)       input_event.key = 260;
                else if (event.key.key == SDLK_RIGHT) input_event.key = 261;
                else if (event.key.key == SDLK_UP)    input_event.key = 259;
                else if (event.key.key == SDLK_DOWN)  input_event.key = 258;
                else                                  input_event.key = (int)event.key.key;

                input_event.scancode = event.key.scancode;
                input_event.mod = event.key.mod;
                input_event.value = 1; // Mark as pressed
                
                /* Zero out unused fields */
                input_event.x = 0; input_event.y = 0; 
                input_event.button = 0; input_event.joy_id = 0; 
                input_event.axis = 0; 
                
                controller_handle_event(controller, &input_event);
            }
        }
        
        /* Process continuous input (held keys) */
        controller_process_input(controller);
        
        /* Calculate delta time */
        uint32_t current_time = SDL_GetTicks();
        float delta_time = (current_time - last_update) / 1000.0f;
        
        /* Cap delta time */
        if (delta_time > 0.1f) delta_time = 0.1f;
        
        last_update = current_time;
        
        /* Update game */
        controller_update(controller, delta_time);
        game_context_update(context);
        
        /* Render */
        sdl_view_render(view, context->model);
        
        /* Cap framerate */
        uint32_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay((uint32_t)(FRAME_DELAY - frame_time));
        }
    }
    
    /* Cleanup */
    printf("Cleaning up...\n");
    sdl_view_destroy(view);
    controller_destroy(controller);
    game_context_destroy(context);
    
    return 0;
}