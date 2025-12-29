#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

/*
 * NOTE: We use the TTF_Font struct from SDL3_ttf directly.
 * Do NOT typedef it to void* or similar as it causes conflicts.
 */

// Function prototypes
bool font_manager_init(void);
void font_manager_quit(void);
TTF_Font* font_manager_load(const char* path, int size);
void font_manager_close(TTF_Font* font);

#endif // FONT_MANAGER_H