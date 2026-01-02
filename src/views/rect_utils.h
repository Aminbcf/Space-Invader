#ifndef RECT_UTILS_H
#define RECT_UTILS_H

#include "../core/model.h"
#include <SDL3/SDL.h>

// Helper to convert Rect to SDL_FRect
static inline SDL_FRect rect_to_sdl_frect(const Rect *r) {
  SDL_FRect sdlr = {(float)r->x, (float)r->y, (float)r->width,
                    (float)r->height};
  return sdlr;
}

#endif // RECT_UTILS_H