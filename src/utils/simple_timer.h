#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

// Simple cross-platform timer
static inline uint32_t get_current_ticks(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static inline void delay_ms(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

#endif // SIMPLE_TIMER_H
