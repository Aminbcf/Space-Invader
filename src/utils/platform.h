#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <time.h>


static inline uint32_t platform_get_ticks(void) {
    
    return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}


static inline void platform_sleep_ms(uint32_t ms) {
    
    uint32_t start = platform_get_ticks();
    while ((platform_get_ticks() - start) < ms) {
     
    }
}

#endif 