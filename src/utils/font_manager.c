// font_manager.c
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

TTF_Font* load_font(const char* path, int size) {
    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font) {
        fprintf(stderr, "Failed to load font %s: %s\n", path, TTF_GetError());
        return NULL;
    }
    return font;
}

void free_font(TTF_Font* font) {
    if (font) {
        TTF_CloseFont(font);
    }
}