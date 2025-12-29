#include "platform.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

uint32_t platform_get_ticks(void) {
    return (uint32_t)SDL_GetTicks();
}

void platform_delay(uint32_t ms) {
    SDL_Delay(ms);
}

void platform_sleep_ms(uint32_t ms) {
    SDL_Delay(ms);
}

bool platform_key_pressed(int key) {
    /* Get the pointer to the keyboard snapshot */
    int num_keys = 0;
    const bool* keyboard_state = SDL_GetKeyboardState(&num_keys);
    
    if (!keyboard_state) return false;

    /* SDL3 SDL_GetScancodeFromKey requires keymod state pointer, passing NULL is valid */
    SDL_Scancode scancode = SDL_GetScancodeFromKey((SDL_Keycode)key, NULL);

    /* Hack to support WASD even if controller asks for Arrows */
    if (key == SDLK_LEFT) {
        if ((int)SDL_SCANCODE_LEFT < num_keys && keyboard_state[SDL_SCANCODE_LEFT]) return true;
        if ((int)SDL_SCANCODE_A < num_keys && keyboard_state[SDL_SCANCODE_A]) return true;
    }
    else if (key == SDLK_RIGHT) {
        if ((int)SDL_SCANCODE_RIGHT < num_keys && keyboard_state[SDL_SCANCODE_RIGHT]) return true;
        if ((int)SDL_SCANCODE_D < num_keys && keyboard_state[SDL_SCANCODE_D]) return true;
    }
    else if (key == SDLK_SPACE) {
        if ((int)SDL_SCANCODE_SPACE < num_keys && keyboard_state[SDL_SCANCODE_SPACE]) return true;
    }
    
    /* Standard check for any other key */
    if (scancode != SDL_SCANCODE_UNKNOWN && (int)scancode < num_keys) {
        return keyboard_state[scancode];
    }
    
    return false;
}

bool platform_joystick_available(void) {
    int count = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
    if (joysticks) {
        SDL_free(joysticks);
    }
    return count > 0;
}

float platform_joystick_axis(int joystick_id, int axis) {
    // SDL3: Use SDL_GetJoystickFromID instead of SDL_OpenJoystick
    SDL_Joystick* joystick = SDL_GetJoystickFromID((SDL_JoystickID)joystick_id);
    if (!joystick) return 0.0f;
    
    Sint16 value = SDL_GetJoystickAxis(joystick, axis);
    // Note: In SDL3, we don't need to close joystick from GetJoystickFromID
    
    return value / 32768.0f;
}

bool platform_joystick_button(int joystick_id, int button) {
    // SDL3: Use SDL_GetJoystickFromID instead of SDL_OpenJoystick
    SDL_Joystick* joystick = SDL_GetJoystickFromID((SDL_JoystickID)joystick_id);
    if (!joystick) return false;
    
    bool pressed = SDL_GetJoystickButton(joystick, button) != 0;
    // Note: In SDL3, we don't need to close joystick from GetJoystickFromID
    
    return pressed;
}