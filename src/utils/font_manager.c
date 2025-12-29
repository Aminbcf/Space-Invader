// src/utils/font_manager.c - Fixed version
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_SDL
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#else
// Empty definitions for ncurses
typedef void* TTF_Font;
#define TTF_STYLE_NORMAL 0
#endif

TTF_Font* load_font(const char* path, int size) {
#ifdef USE_SDL
    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font) {
        fprintf(stderr, "Failed to load font %s: %s\n", path, SDL_GetError());
        return NULL;
    }
    return font;
#else
    // For ncurses, return NULL (no font support)
    (void)path;
    (void)size;
    return NULL;
#endif
}

void free_font(TTF_Font* font) {
#ifdef USE_SDL
    if (font) {
        TTF_CloseFont(font);
    }
#else
    (void)font;
#endif
}