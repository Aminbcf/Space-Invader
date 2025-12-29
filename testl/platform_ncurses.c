#include "platform.h"
#include <time.h>
#include <stdint.h>

uint32_t platform_get_ticks(void) {
    return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

void platform_sleep_ms(uint32_t ms) {
    uint32_t start = platform_get_ticks();
    while ((platform_get_ticks() - start) < ms) {
        /* Busy wait */
    }
}