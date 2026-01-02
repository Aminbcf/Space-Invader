#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#ifdef USE_SDL_VIEW
// For SDL build, declare functions (implemented in platform_sdl.c)
uint32_t platform_get_ticks(void);
void platform_delay(uint32_t ms);
void platform_sleep_ms(uint32_t ms);
bool platform_key_pressed(int key);
bool platform_joystick_available(void);
float platform_joystick_axis(int joystick_id, int axis);
bool platform_joystick_button(int joystick_id, int button);

#else
// For non-SDL build (ncurses), use inline implementations
#include <time.h>

static inline uint32_t platform_get_ticks(void) {
  return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

static inline void platform_delay(uint32_t ms) {
  uint32_t start = platform_get_ticks();
  while ((platform_get_ticks() - start) < ms) {
    /* Busy wait */
  }
}

static inline void platform_sleep_ms(uint32_t ms) {
  uint32_t start = platform_get_ticks();
  while ((platform_get_ticks() - start) < ms) {
    /* Busy wait */
  }
}

static inline bool platform_key_pressed(int key) {
  (void)key;
  return false; // Not implemented for ncurses
}

static inline bool platform_joystick_available(void) {
  return false; // No joystick support in ncurses
}

static inline float platform_joystick_axis(int joystick_id, int axis) {
  (void)joystick_id;
  (void)axis;
  return 0.0f;
}

static inline bool platform_joystick_button(int joystick_id, int button) {
  (void)joystick_id;
  (void)button;
  return false;
}

#endif

#endif // PLATFORM_H