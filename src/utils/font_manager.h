#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <SDL2/SDL_ttf.h>

TTF_Font* load_font(const char* path, int size);
void free_font(TTF_Font* font);

#endif