#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Include ncurses.h FIRST, then undefine color macros to prevent clashes */
#include <ncurses.h>

#ifdef COLOR_RED
#undef COLOR_RED
#endif
#ifdef COLOR_GREEN
#undef COLOR_GREEN
#endif
#ifdef COLOR_YELLOW
#undef COLOR_YELLOW
#endif
#ifdef COLOR_BLUE
#undef COLOR_BLUE
#endif
#ifdef COLOR_CYAN
#undef COLOR_CYAN
#endif
#ifdef COLOR_MAGENTA
#undef COLOR_MAGENTA
#endif
#ifdef COLOR_WHITE
#undef COLOR_WHITE
#endif
#ifdef COLOR_BLACK
#undef COLOR_BLACK
#endif

#include "core/model.h"
#include "core/game_state.h"
#include "controller/controller.h"
#include "views/view_ncurses.h"
#include "utils/platform.h"



/* Simple sleep using busy wait */
/* Sleep using ncurses napms (avoids busy wait) */
static void sleep_ms(uint32_t ms) {
    napms(ms);
}



/* Input smoothing state */
static Direction current_move_dir_p1 = DIR_STATIONARY;
static uint32_t last_input_time_p1 = 0;
static Direction current_move_dir_p2 = DIR_STATIONARY;
static uint32_t last_input_time_p2 = 0;
static const uint32_t INPUT_TIMEOUT_MS = 100; // Large buffer for smooth movement on all terminals

/* Process a single key for gameplay - updates state only */
static void process_gameplay_key(GameModel *model, Controller *controller, int ch) {
    uint32_t now = platform_get_ticks();
    
    /* Check P1 keybinds */
    if (ch == model->keybinds_p1[0]) { current_move_dir_p1 = DIR_LEFT; last_input_time_p1 = now; }
    else if (ch == model->keybinds_p1[1]) { current_move_dir_p1 = DIR_RIGHT; last_input_time_p1 = now; }
    else if (ch == model->keybinds_p1[2]) { current_move_dir_p1 = DIR_UP; last_input_time_p1 = now; }
    else if (ch == model->keybinds_p1[3]) { current_move_dir_p1 = DIR_DOWN; last_input_time_p1 = now; }
    else if (ch == model->keybinds_p1[4]) { model_player_shoot(model, 0); }
    
    /* P2 keybinds */
    else if (ch == model->keybinds_p2[0]) { current_move_dir_p2 = DIR_LEFT; last_input_time_p2 = now; }
    else if (ch == model->keybinds_p2[1]) { current_move_dir_p2 = DIR_RIGHT; last_input_time_p2 = now; }
    else if (ch == model->keybinds_p2[2]) { current_move_dir_p2 = DIR_UP; last_input_time_p2 = now; }
    else if (ch == model->keybinds_p2[3]) { current_move_dir_p2 = DIR_DOWN; last_input_time_p2 = now; }
    else if (ch == model->keybinds_p2[4]) { model_player_shoot(model, 1); }
    
    /* Hardcoded controls */
    else if (ch == 'p' || ch == 'P') { model_toggle_pause(model); }
    else if (ch == 'q' || ch == 'Q' || ch == 27) { controller->quit_requested = true; }
    else if (ch == 'r' || ch == 'R') {
        if (model->state == STATE_GAME_OVER || model->state == STATE_WIN) {
            model_reset_game(model);
            model->state = STATE_PLAYING;
        }
    }
}

int main(int argc, char* argv[]) {
    bool valgrind_test = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--valgrind-test") == 0) {
            valgrind_test = true;
        }
    }
    
    printf("Space Invaders MVC - NCURSES Version\n");
    printf("Initializing...\n");
    
    /* Create game context */
    GameContext* context = game_context_create();
    if (!context) {
        fprintf(stderr, "Failed to create game context\n");
        return 1;
    }
    
    /* Set ncurses-compatible default keybindings in model */
    context->model->keybinds_p1[0] = KEY_LEFT;
    context->model->keybinds_p1[1] = KEY_RIGHT;
    context->model->keybinds_p1[2] = KEY_UP;
    context->model->keybinds_p1[3] = KEY_DOWN;
    context->model->keybinds_p1[4] = ' ';  // Space to shoot
    context->model->keybinds_p2[0] = 'a';
    context->model->keybinds_p2[1] = 'd';
    context->model->keybinds_p2[2] = 'w';
    context->model->keybinds_p2[3] = 's';
    context->model->keybinds_p2[4] = 'f';
    
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
    
    /* Initialize view */
    if (!ncurses_view_init(view, NCURSES_SCREEN_WIDTH, NCURSES_SCREEN_HEIGHT)) {
        fprintf(stderr, "Failed to initialize NCURSES view\n");
        ncurses_view_destroy(view);
        controller_destroy(controller);
        game_context_destroy(context);
        return 1;
    }
    
    /* Set view context */
    controller_set_view_context(controller, view);
    controller_set_keybindings(controller, 0, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, ' ', 'p', 'q');
    controller_set_keybindings(controller, 1, 'w', 's', 'a', 'd', 'f', 0, 0);

    /* Main game loop */
    int ch;
    const int TARGET_FPS = 60; // 60 FPS for smooth movement
    uint32_t last_update = platform_get_ticks();
    const float FRAME_DELAY = 1000.0f / TARGET_FPS;
    
    int frame_count = 0;
    while (!controller_is_quit_requested(controller) && context->model->state != STATE_QUIT) {
        if (valgrind_test && frame_count++ >= 60) {
            break;
        }
        uint32_t frame_start = platform_get_ticks();
        
        /* Handle input */
        while (ncurses_view_poll_event(view, &ch)) {
            if (context->model->waiting_for_key) {
                /* Settings: capture new keybind */
                model_set_keybind(context->model, ch);
            }
            else if (context->model->state == STATE_MENU) {
                /* Menu navigation */
                InputEvent evt = {0};
                evt.type = INPUT_KEYBOARD;
                if (ch == KEY_UP) evt.key = controller->key_up;
                else if (ch == KEY_DOWN) evt.key = controller->key_down;
                else if (ch == KEY_LEFT) evt.key = controller->key_left;
                else if (ch == KEY_RIGHT) evt.key = controller->key_right;
                else evt.key = ch;
                controller_handle_event(controller, &evt);
            }
            else if (context->model->state == STATE_PLAYING) {
                /* Gameplay - use model keybindings */
                process_gameplay_key(context->model, controller, ch);
            }
            else {
                /* Other states */
                InputEvent evt = {0};
                evt.type = INPUT_KEYBOARD;
                evt.key = ch;
                controller_handle_event(controller, &evt);
            }
        }
        
        /* Frame timing */
        uint32_t current_time = platform_get_ticks();
        float delta_time = (current_time - last_update) / 1000.0f;
        if (delta_time > 0.1f) delta_time = 0.1f;
        last_update = current_time;
        
        /* Update game */
        controller_update(controller, delta_time);
        
        /* Apply smooth movement if inputs are active */
        if (context->model->state == STATE_PLAYING) {
            if ((current_time - last_input_time_p1) < INPUT_TIMEOUT_MS && current_move_dir_p1 != DIR_STATIONARY) {
                model_move_player(context->model, 0, current_move_dir_p1);
            }
            if ((current_time - last_input_time_p2) < INPUT_TIMEOUT_MS && current_move_dir_p2 != DIR_STATIONARY) {
                model_move_player(context->model, 1, current_move_dir_p2);
            }
        }
        
        model_update(context->model, delta_time);
        
        /* Render */
        ncurses_view_render(view, context->model);
        
        /* Cap framerate */
        uint32_t frame_time = platform_get_ticks() - frame_start;
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