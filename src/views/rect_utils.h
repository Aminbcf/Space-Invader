#include <SDL2/SDL.h>
#include "../core/model.h"

// Helper to convert Rect to SDL_Rect
static inline SDL_Rect rect_to_sdl_rect(const Rect* r) {
    SDL_Rect sdlr = { r->x, r->y, r->width, r->height };
    return sdlr;
}
