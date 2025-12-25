#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "core/model.h"
#include "core/game_state.h"
#include "controller/controller.h"
#include "views/view_ncurses.h"

/* Define key codes for ncurses compatibility */
#define SDLK_LEFT 1073741904
#define SDLK_RIGHT 1073741903
#define SDLK_SPACE 32
#define SDLK_p 112
#define SDLK_ESCAPE 27
#define SDLK_r 114
#define SDLK_RETURN 13

/* Simple millisecond timer using standard C clock() */
static uint32_t get_time_ms(void) {
    return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

/* Simple sleep using busy wait */
static void sleep_ms(uint32_t ms) {
    uint32_t start = get_time_ms();
    while ((get_time_ms() - start) < ms) {
        /* Busy wait - not efficient but simple and portable */
    }
}


int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    printf("Space Invaders MVC - NCURSES Version\n");
    printf("Initializing...\n");
    
    /* Check terminal size */
    printf("\nChecking terminal size...\n");
    printf("Required: %dx%d characters\n", NCURSES_SCREEN_WIDTH, NCURSES_SCREEN_HEIGHT);
    
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
    
    /* Create ncurses view */
    NcursesView* view = ncurses_view_create();
    if (!view) {
        fprintf(stderr, "Failed to create NCURSES view\n");
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    /* Show initial instructions */
    printf("\nControls:\n");
    printf("  A/D or Arrow Keys: Move Left/Right\n");
    printf("  Space: Shoot\n");
    printf("  P: Pause\n");
    printf("  Q: Quit\n");
    printf("  R: Restart (when game over)\n");
    printf("\nStarting in 2 seconds...\n");
    sleep_ms(2000);
    
    /* Initialize view with proper grid size */
    if (!ncurses_view_init(view, NCURSES_SCREEN_WIDTH, NCURSES_SCREEN_HEIGHT)) {
        fprintf(stderr, "Failed to initialize NCURSES view\n");
        ncurses_view_destroy(view);
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    /* Set view context for controller */
    controller_set_view_context(controller, view);
    
    /* Main game loop */
    int ch;
    uint32_t last_update = get_time_ms();
    const int TARGET_FPS = 30;
    const float FRAME_DELAY = 1000.0f / TARGET_FPS;
    
    while (!controller_is_quit_requested(controller)) {
        uint32_t frame_start = get_time_ms();
        
        /* Handle ncurses input */
        while (ncurses_view_poll_event(view, &ch)) {
            InputEvent input_event;
            memset(&input_event, 0, sizeof(InputEvent));
            input_event.type = INPUT_KEYBOARD;
            
            /* Translate ncurses keys to SDL key codes for compatibility */
            switch (ch) {
                case 'a':
                case 'A':
                case KEY_LEFT:
                    input_event.key = SDLK_LEFT;
                    break;
                case 'd':
                case 'D':
                case KEY_RIGHT:
                    input_event.key = SDLK_RIGHT;
                    break;
                case ' ':
                    input_event.key = SDLK_SPACE;
                    break;
                case 'p':
                case 'P':
                    input_event.key = SDLK_p;
                    break;
                case 'q':
                case 'Q':
                    input_event.key = SDLK_ESCAPE;
                    break;
                case 'r':
                case 'R':
                    input_event.key = SDLK_r;
                    break;
                case '\n':
                    input_event.key = SDLK_RETURN;
                    break;
                default:
                    input_event.key = ch;
                    break;
            }
            
            controller_handle_event(controller, &input_event);
        }
        
        /* Process continuous input */
        controller_process_input(controller);
        
        /* Calculate delta time */
        uint32_t current_time = get_time_ms();
        float delta_time = (current_time - last_update) / 1000.0f;
        
        /* Cap delta time to prevent huge jumps */
        if (delta_time > 0.1f) delta_time = 0.1f;
        if (delta_time < 0.0f) delta_time = 0.0f;
        
        last_update = current_time;
        
        /* Update game */
        controller_update(controller, delta_time);
        game_context_update(context);
        
        /* Render */
        ncurses_view_render(view, context->model);
        
        /* Cap framerate */
        uint32_t frame_time = get_time_ms() - frame_start;
        if (frame_time < FRAME_DELAY) {
            sleep_ms((uint32_t)(FRAME_DELAY - frame_time));
        }
    }
    
    /* Cleanup */
    ncurses_view_destroy(view);
    controller_destroy(controller);
    game_context_destroy(context);
    
    printf("Game ended. Thanks for playing!\n");
    return 0;
}