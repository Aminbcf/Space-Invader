#include "input_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_SDL_VIEW
#include <SDL3/SDL.h>
#endif

InputHandler *input_handler_create(void) {
  InputHandler *handler = malloc(sizeof(InputHandler));
  if (!handler)
    return NULL;

  memset(handler, 0, sizeof(InputHandler));

  memset(&handler->keyboard_state, 0, sizeof(InputState));
  handler->keyboard_state.initial_delay = 300;
  handler->keyboard_state.repeat_delay = 50;

  handler->joystick_states = NULL;
  handler->joystick_count = 0;

#ifdef USE_SDL_VIEW
  /* Default: Arrows, Space, P, Escape */
  input_handler_set_keybindings(handler, SDLK_LEFT, SDLK_RIGHT, SDLK_SPACE,
                                SDLK_P, SDLK_ESCAPE);
#else
  input_handler_set_keybindings(handler, 'a', 'd', ' ', 'p', 27);
#endif

  input_handler_set_joystick_bindings(handler, 0, 1, 2, 3, 4);
  handler->axis_threshold = 0.5f;

  /*
  #ifdef USE_SDL_VIEW
  input_handler_detect_joysticks(handler);
  #endif
  */

  return handler;
}

void input_handler_destroy(InputHandler *handler) {
  if (handler) {
    if (handler->joystick_states) {
      free(handler->joystick_states);
    }
    free(handler);
  }
}

#ifdef USE_SDL_VIEW
void input_handler_set_keybindings(InputHandler *handler, SDL_Keycode left,
                                   SDL_Keycode right, SDL_Keycode shoot,
                                   SDL_Keycode pause, SDL_Keycode quit) {
  if (!handler)
    return;

  handler->key_left = left;
  handler->key_right = right;
  handler->key_shoot = shoot;
  handler->key_pause = pause;
  handler->key_quit = quit;
}
#else
void input_handler_set_keybindings(InputHandler *handler, int left, int right,
                                   int shoot, int pause, int quit) {
  if (!handler)
    return;

  handler->key_left = left;
  handler->key_right = right;
  handler->key_shoot = shoot;
  handler->key_pause = pause;
  handler->key_quit = quit;
}
#endif

void input_handler_set_joystick_bindings(InputHandler *handler, int left,
                                         int right, int shoot, int pause,
                                         int quit) {
  if (!handler)
    return;

  handler->joy_button_left = left;
  handler->joy_button_right = right;
  handler->joy_button_shoot = shoot;
  handler->joy_button_pause = pause;
  handler->joy_button_quit = quit;
}

#ifdef USE_SDL_VIEW
void input_handler_detect_joysticks(InputHandler *handler) {
  if (!handler)
    return;

  int count = 0;
  SDL_JoystickID *joysticks = SDL_GetJoysticks(&count);

  if (count < 0)
    count = 0;

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

  if (joysticks) {
    SDL_free(joysticks);
  }
}
#else
void input_handler_detect_joysticks(InputHandler *handler) {
  if (!handler)
    return;
  handler->joystick_states = NULL;
  handler->joystick_count = 0;
}
#endif

#ifdef USE_SDL_VIEW
void input_handler_process_sdl_event(InputHandler *handler,
                                     const SDL_Event *event) {
  if (!handler || !event)
    return;

  bool is_left = false;
  bool is_right = false;
  bool is_shoot = false;
  bool is_pause = false;
  bool is_quit = false;
  bool is_up = false;
  bool is_down = false;
  bool is_enter = false;

  if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
    SDL_Keycode key = event->key.key;

    /* Support both Primary binds and WASD hardcoded for convenience */
    is_left = (key == handler->key_left) || (key == SDLK_A);
    is_right = (key == handler->key_right) || (key == SDLK_D);
    is_shoot = (key == handler->key_shoot);
    is_pause = (key == handler->key_pause) || (key == SDLK_P);
    is_quit = (key == handler->key_quit) || (key == SDLK_Q);
    is_up = (key == SDLK_UP) || (key == SDLK_W);
    is_down = (key == SDLK_DOWN) || (key == SDLK_S);
    is_enter =
        (key == SDLK_RETURN) || (key == SDLK_RETURN2) || (key == SDLK_KP_ENTER);
  }

  switch (event->type) {
  case SDL_EVENT_KEY_DOWN:
    if (is_left)
      handler->keyboard_state.left_pressed = true;
    if (is_right)
      handler->keyboard_state.right_pressed = true;
    if (is_shoot)
      handler->keyboard_state.shoot_pressed = true;
    if (is_pause)
      handler->keyboard_state.pause_pressed = true;
    if (is_quit)
      handler->keyboard_state.quit_pressed = true;
    if (is_up)
      handler->keyboard_state.up_pressed = true;
    if (is_down)
      handler->keyboard_state.down_pressed = true;
    if (is_enter)
      handler->keyboard_state.enter_pressed = true;
    break;

  case SDL_EVENT_KEY_UP:
    if (is_left) {
      handler->keyboard_state.left_pressed = false;
      handler->keyboard_state.left_was_pressed = false;
      handler->keyboard_state.left_repeat_time = 0;
    }
    if (is_right) {
      handler->keyboard_state.right_pressed = false;
      handler->keyboard_state.right_was_pressed = false;
      handler->keyboard_state.right_repeat_time = 0;
    }
    if (is_shoot) {
      handler->keyboard_state.shoot_pressed = false;
      handler->keyboard_state.shoot_was_pressed = false;
      handler->keyboard_state.shoot_repeat_time = 0;
    }
    if (is_pause) {
      handler->keyboard_state.pause_pressed = false;
      handler->keyboard_state.pause_was_pressed = false;
    }
    if (is_quit) {
      handler->keyboard_state.quit_pressed = false;
    }
    if (is_up) {
      handler->keyboard_state.up_pressed = false;
      handler->keyboard_state.up_was_pressed = false;
    }
    if (is_down) {
      handler->keyboard_state.down_pressed = false;
      handler->keyboard_state.down_was_pressed = false;
    }
    if (is_enter) {
      handler->keyboard_state.enter_pressed = false;
      handler->keyboard_state.enter_was_pressed = false;
    }
    break;

  case SDL_EVENT_JOYSTICK_ADDED:
  case SDL_EVENT_JOYSTICK_REMOVED:
    // input_handler_detect_joysticks(handler);
    break;

  case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
  case SDL_EVENT_JOYSTICK_BUTTON_UP:
  case SDL_EVENT_JOYSTICK_AXIS_MOTION:
    // input_handler_handle_joystick_event(handler, event);
    break;
  }
}

void input_handler_handle_joystick_event(InputHandler *handler,
                                         const SDL_Event *event) {
  if (!handler || !event)
    return;

  SDL_JoystickID joystick_id = event->jbutton.which;

  // Check if joystick_id is valid
  if (joystick_id >= (SDL_JoystickID)handler->joystick_count)
    return;

  InputState *joy_state = &handler->joystick_states[joystick_id];

  switch (event->type) {
  case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
    if (event->jbutton.button == handler->joy_button_left)
      joy_state->left_pressed = true;
    if (event->jbutton.button == handler->joy_button_right)
      joy_state->right_pressed = true;
    if (event->jbutton.button == handler->joy_button_shoot)
      joy_state->shoot_pressed = true;
    if (event->jbutton.button == handler->joy_button_pause)
      joy_state->pause_pressed = true;
    if (event->jbutton.button == handler->joy_button_quit)
      joy_state->quit_pressed = true;
    break;

  case SDL_EVENT_JOYSTICK_BUTTON_UP:
    if (event->jbutton.button == handler->joy_button_left) {
      joy_state->left_pressed = false;
      joy_state->left_was_pressed = false;
    }
    if (event->jbutton.button == handler->joy_button_right) {
      joy_state->right_pressed = false;
      joy_state->right_was_pressed = false;
    }
    if (event->jbutton.button == handler->joy_button_shoot) {
      joy_state->shoot_pressed = false;
      joy_state->shoot_was_pressed = false;
    }
    if (event->jbutton.button == handler->joy_button_pause) {
      joy_state->pause_pressed = false;
      joy_state->pause_was_pressed = false;
    }
    if (event->jbutton.button == handler->joy_button_quit) {
      joy_state->quit_pressed = false;
    }
    break;

  case SDL_EVENT_JOYSTICK_AXIS_MOTION:
    // Handle axis motion for analog controls
    if (event->jaxis.axis == 0) { // X axis
      float value = event->jaxis.value / 32767.0f;
      if (value < -handler->axis_threshold) {
        joy_state->left_pressed = true;
        joy_state->right_pressed = false;
      } else if (value > handler->axis_threshold) {
        joy_state->right_pressed = true;
        joy_state->left_pressed = false;
      } else {
        joy_state->left_pressed = false;
        joy_state->right_pressed = false;
      }
    }
    break;
  }
}
#endif

void input_handler_process_ncurses_input(InputHandler *handler, int ch) {
  if (!handler)
    return;

  handler->keyboard_state.left_pressed = false;
  handler->keyboard_state.right_pressed = false;
  handler->keyboard_state.shoot_pressed = false;
  handler->keyboard_state.pause_pressed = false;
  handler->keyboard_state.quit_pressed = false;

  switch (ch) {
  case 'a':
  case 'A':
    handler->keyboard_state.left_pressed = true;
    break;
  case 'd':
  case 'D':
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
  case 27:
    handler->keyboard_state.quit_pressed = true;
    break;
  }
}

void input_handler_update(InputHandler *handler, uint32_t current_time) {
  if (!handler)
    return;

  InputState *state = &handler->keyboard_state;

  if (state->left_pressed) {
    if (!state->left_was_pressed) {
      state->left_was_pressed = true;
      state->left_repeat_time = current_time + state->initial_delay;
    } else if (current_time >= state->left_repeat_time) {
      state->left_repeat_time = current_time + state->repeat_delay;
    }
  }

  if (state->right_pressed) {
    if (!state->right_was_pressed) {
      state->right_was_pressed = true;
      state->right_repeat_time = current_time + state->initial_delay;
    } else if (current_time >= state->right_repeat_time) {
      state->right_repeat_time = current_time + state->repeat_delay;
    }
  }

  if (state->shoot_pressed) {
    if (!state->shoot_was_pressed) {
      state->shoot_was_pressed = true;
      state->shoot_repeat_time = current_time + state->initial_delay;
    } else if (current_time >= state->shoot_repeat_time) {
      state->shoot_repeat_time = current_time + state->repeat_delay;
    }
  }
}

bool input_handler_get_command(InputHandler *handler, Command *cmd) {
  if (!handler || !cmd)
    return false;

  *cmd = CMD_NONE;
  InputState *state = &handler->keyboard_state;

  if (state->quit_pressed) {
    *cmd = CMD_QUIT;
    state->quit_pressed = false;
    return true;
  }
  if (state->pause_pressed && !state->pause_was_pressed) {
    *cmd = CMD_PAUSE;
    state->pause_was_pressed = true;
    state->pause_pressed = false;
    return true;
  }
  if (state->left_pressed) {
    *cmd = CMD_MOVE_LEFT;
    return true;
  }
  if (state->right_pressed) {
    *cmd = CMD_MOVE_RIGHT;
    return true;
  }
  if (state->shoot_pressed && !state->shoot_was_pressed) {
    *cmd = CMD_SHOOT;
    return true;
  }

  return false;
}

#ifdef USE_SDL_VIEW
bool input_handler_is_key_pressed(InputHandler *handler, SDL_Keycode key) {
  if (!handler)
    return false;

  // Check current keyboard state
  int num_keys = 0;
  const bool *state = SDL_GetKeyboardState(&num_keys);
  if (!state)
    return false;

  // Convert keycode to scancode - SDL3 requires modstate parameter
  SDL_Scancode scancode = SDL_GetScancodeFromKey(key, NULL);

  if (scancode == SDL_SCANCODE_UNKNOWN)
    return false;
  if ((int)scancode >= num_keys)
    return false;

  return state[scancode];
}

bool input_handler_was_key_pressed(InputHandler *handler, SDL_Keycode key) {
  (void)handler;
  (void)key;
  return false; // Not implemented
}

float input_handler_get_joystick_axis(InputHandler *handler, int joystick_id,
                                      int axis) {
  if (!handler || joystick_id < 0 || joystick_id >= handler->joystick_count)
    return 0.0f;

  // SDL3: Use SDL_GetJoystickFromID instead of SDL_JoystickFromInstanceID
  SDL_Joystick *joystick = SDL_GetJoystickFromID((SDL_JoystickID)joystick_id);
  if (!joystick)
    return 0.0f;

  Sint16 value = SDL_GetJoystickAxis(joystick, axis);
  return value / 32767.0f;
}

bool input_handler_get_joystick_button(InputHandler *handler, int joystick_id,
                                       int button) {
  if (!handler || joystick_id < 0 || joystick_id >= handler->joystick_count)
    return false;

  // SDL3: Use SDL_GetJoystickFromID instead of SDL_JoystickFromInstanceID
  SDL_Joystick *joystick = SDL_GetJoystickFromID((SDL_JoystickID)joystick_id);
  if (!joystick)
    return false;

  return SDL_GetJoystickButton(joystick, button);
}
#else
bool input_handler_is_key_pressed(InputHandler *handler, int key) {
  (void)handler;
  (void)key;
  return false;
}

bool input_handler_was_key_pressed(InputHandler *handler, int key) {
  (void)handler;
  (void)key;
  return false;
}
#endif

bool input_handler_save_config(InputHandler *handler, const char *filename) {
  (void)handler;
  (void)filename;
  return true;
}

bool input_handler_load_config(InputHandler *handler, const char *filename) {
  (void)handler;
  (void)filename;
  return true;
}