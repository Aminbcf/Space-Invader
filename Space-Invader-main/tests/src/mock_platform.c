#include "mock_platform.h"
#include <stdint.h>

static uint32_t mock_current_ticks = 0;

uint32_t mock_platform_get_ticks(void) {
    return mock_current_ticks;
}

void mock_platform_set_ticks(uint32_t ticks) {
    mock_current_ticks = ticks;
}

void mock_platform_increment_ticks(uint32_t delta) {
    mock_current_ticks += delta;
}