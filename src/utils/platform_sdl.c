#include "platform.h"
#include <SDL2/SDL.h>

uint32_t platform_get_ticks(void) {
    return SDL_GetTicks();
}

void platform_delay(uint32_t ms) {
    SDL_Delay(ms);
}

bool platform_key_pressed(int key) {
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    return scancode != SDL_SCANCODE_UNKNOWN && keyboard_state[scancode] != 0;
}

bool platform_joystick_available(void) {
    return SDL_NumJoysticks() > 0;
}

float platform_joystick_axis(int joystick_id, int axis) {
    SDL_Joystick* joystick = SDL_JoystickOpen(joystick_id);
    if (!joystick) return 0.0f;
    
    Sint16 value = SDL_JoystickGetAxis(joystick, axis);
    SDL_JoystickClose(joystick);
    
    return value / 32768.0f;
}

bool platform_joystick_button(int joystick_id, int button) {
    SDL_Joystick* joystick = SDL_JoystickOpen(joystick_id);
    if (!joystick) return false;
    
    bool pressed = SDL_JoystickGetButton(joystick, button) != 0;
    SDL_JoystickClose(joystick);
    
    return pressed;
}

PlatformType platform_get_type(void) {
    return PLATFORM_SDL;
}