#ifndef MOCK_PLATFORM_H
#define MOCK_PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

// Mock platform functions for testing
#ifdef PLATFORM_MOCK

uint32_t mock_platform_get_ticks(void);
void mock_platform_set_ticks(uint32_t ticks);
void mock_platform_increment_ticks(uint32_t delta);

// Redefine platform functions to use mocks
#define platform_get_ticks mock_platform_get_ticks
#define platform_delay(ms) ((void)(ms))
#define platform_sleep_ms(ms) ((void)(ms))
#define platform_key_pressed(key) false
#define platform_joystick_available() false
#define platform_joystick_axis(joy, axis) 0.0f
#define platform_joystick_button(joy, button) false

#endif // PLATFORM_MOCK

#endif // MOCK_PLATFORM_H