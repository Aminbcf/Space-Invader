#include "controller/controller.h"
#include "controller/input_handler.h"
#include "core/game_state.h"
#include "core/model.h"
#include "views/view_sdl.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL)); // Initialize random seed once
  bool valgrind_test = false;
  for (int i = 1; i < argc; i++) {
    if (SDL_strcmp(argv[i], "--valgrind-test") == 0) {
      valgrind_test = true;
    }
  }

  /* Create game context */
  GameContext *context = game_context_create();
  if (!context) {
    fprintf(stderr, "Failed to create game context\n");
    return 1;
  }

  /* Create controller */
  Controller *controller = controller_create(context->model);
  if (!controller) {
    fprintf(stderr, "Failed to create controller\n");
    game_context_destroy(context);
    return 1;
  }

  // Unbind default gameplay keys from InputHandler to prevent "ghost keys"
  // We will handle gameplay inputs exclusively via Model Keybinds below.
  // Keep Pause (P) and Quit (ESC) to allow controller fallback to handle them.
  input_handler_set_keybindings(controller->input_handler, 0, 0, 0, SDLK_P,
                                SDLK_ESCAPE);

  /* Create SDL view */
  SDLView *view = sdl_view_create();
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

  // Set window title correctly
  SDL_SetWindowTitle(view->window, "Space Invader");

  /* Setup Default Keybindings - Clear gameplay keys so only Model binds work */
  /* We keep Pause(P) for fallback, but remove movement/shoot defaults */
  controller_set_keybindings(controller, 0, 0, 0, 0, 0, 0, SDLK_P, 0);
  controller_set_keybindings(controller, 1, 0, 0, 0, 0, 0, 0, 0);

  /* Set view context */
  controller_set_view_context(controller, view);

  /* Game loop timing */
  const int TARGET_FPS = 60;
  const uint32_t FRAME_DELAY = 1000 / TARGET_FPS;
  uint32_t last_update = SDL_GetTicks();

  /* Main loop */
  bool running = true;
  SDL_Event event;
  int frame_count = 0;

  printf("Game Running.\n");
  printf("P1: Arrows to Move, Space to Shoot\n");
  printf("P2: WASD to Move, Left Shift to Shoot\n");
  printf("Controls: P to Pause, ESC for Menu\n");

  while (running && !controller_is_quit_requested(controller) &&
         context->model->state != STATE_QUIT) {
    if (valgrind_test && frame_count++ >= 60) {
      break;
    }
    uint32_t frame_start = SDL_GetTicks();

    /* Handle SDL events */
    while (sdl_view_poll_event(view, &event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      } else if (event.type == SDL_EVENT_KEY_DOWN) {
        // Check if we're waiting for a keybind
        if (context->model->waiting_for_key) {
          model_set_keybind(context->model, (int)event.key.key);
        } else {
          // If playing, check model keybindings first to support custom
          // controls
          bool handled = false;
          int key = (int)event.key.key;

// Helper for discrete event checks
#define CHECK_EVENT_KEY(bind, cmd)                                             \
  if (key == (bind)) {                                                         \
    controller_execute_command(controller, cmd);                               \
    handled = true;                                                            \
  }

          // Always check model keybindings for P1 commands (Menu & Gameplay)
          CHECK_EVENT_KEY(context->model->keybinds_p1[0], CMD_LEFT);
          CHECK_EVENT_KEY(context->model->keybinds_p1[1], CMD_RIGHT);
          CHECK_EVENT_KEY(context->model->keybinds_p1[2], CMD_UP);
          CHECK_EVENT_KEY(context->model->keybinds_p1[3], CMD_DOWN);
          CHECK_EVENT_KEY(context->model->keybinds_p1[4], CMD_SHOOT);

          if (context->model->state == STATE_PLAYING) {
            // Player 2
            CHECK_EVENT_KEY(context->model->keybinds_p2[0], CMD_P2_MOVE_LEFT);
            CHECK_EVENT_KEY(context->model->keybinds_p2[1], CMD_P2_MOVE_RIGHT);
            CHECK_EVENT_KEY(context->model->keybinds_p2[2], CMD_P2_MOVE_UP);
            CHECK_EVENT_KEY(context->model->keybinds_p2[3], CMD_P2_MOVE_DOWN);
            CHECK_EVENT_KEY(context->model->keybinds_p2[4], CMD_P2_SHOOT);
          }

          // Fallback to controller mappings (e.g. Pause, Quit, Menu Nav) if not
          // handled
          if (!handled) {
            InputEvent input_event;
            memset(&input_event, 0, sizeof(InputEvent));
            input_event.type = INPUT_KEYBOARD;
            input_event.key = (int)event.key.key;
            input_event.value = 1;
            controller_handle_event(controller, &input_event);
          }
        }
      }
    }

    /* Process continuous input (held keys) using SDL state and model
     * keybindings */
    int num_keys;
    const bool *state = SDL_GetKeyboardState(&num_keys);

// Helper to check key by keycode from model
#define CHECK_MODEL_KEY(keycode, cmd)                                          \
  {                                                                            \
    SDL_Scancode sc = SDL_GetScancodeFromKey(keycode, NULL);                   \
    if ((int)sc < num_keys && state[sc])                                       \
      controller_execute_command(controller, cmd);                             \
  }

    if (context->model->state == STATE_PLAYING) {
      // P1 Model Keybinds (Continuous input for movement)
      CHECK_MODEL_KEY(context->model->keybinds_p1[0], CMD_LEFT);
      CHECK_MODEL_KEY(context->model->keybinds_p1[1], CMD_RIGHT);
      CHECK_MODEL_KEY(context->model->keybinds_p1[2], CMD_UP);
      CHECK_MODEL_KEY(context->model->keybinds_p1[3], CMD_DOWN);
      CHECK_MODEL_KEY(context->model->keybinds_p1[4], CMD_SHOOT);

      // P2 uses model keybindings
      CHECK_MODEL_KEY(context->model->keybinds_p2[0], CMD_P2_MOVE_LEFT);
      CHECK_MODEL_KEY(context->model->keybinds_p2[1], CMD_P2_MOVE_RIGHT);
      CHECK_MODEL_KEY(context->model->keybinds_p2[2], CMD_P2_MOVE_UP);
      CHECK_MODEL_KEY(context->model->keybinds_p2[3], CMD_P2_MOVE_DOWN);
      CHECK_MODEL_KEY(context->model->keybinds_p2[4], CMD_P2_SHOOT);
    }

    /* Calculate delta time */
    uint32_t current_time = SDL_GetTicks();
    float delta_time = (current_time - last_update) / 1000.0f;
    if (delta_time > 0.1f)
      delta_time = 0.1f;
    last_update = current_time;

    /* Update game */
    controller_update(controller, delta_time);
    model_update(context->model, delta_time);

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