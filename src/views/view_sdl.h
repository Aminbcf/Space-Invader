#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../core/model.h"
#include <stdbool.h>

// view_sdl.h - add these to your SDLView struct
typedef struct SDLView {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool initialized;
    int width;
    int height;
    
    // Textures
    SDL_Texture* player_tex;
    SDL_Texture* boss_tex;
    SDL_Texture* explosion_tex;
    SDL_Texture* bullet_player_tex;
    SDL_Texture* bullet_enemy_tex;
    SDL_Texture* saucer_tex;
    SDL_Texture* damage_tex;
    SDL_Texture* invader_tex[3][2];
    
    // Fonts
    TTF_Font* font_large;
    TTF_Font* font_small;
    
    // Timing
    Uint32 frame_count;
    Uint32 fps;
    Uint32 last_frame_time;
} SDLView;

SDLView* sdl_view_create(void);
void sdl_view_destroy(SDLView* view);
bool sdl_view_init(SDLView* view, int width, int height);
bool sdl_view_poll_event(SDLView* view, SDL_Event* event);
void sdl_view_render(SDLView* view, const GameModel* model);

#endif