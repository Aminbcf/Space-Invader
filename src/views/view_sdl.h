#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../core/model.h"
#include <stdbool.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Textures
    SDL_Texture* player_tex;
    SDL_Texture* invader_tex[3][2]; // 3 Types, 2 Animation Frames
    SDL_Texture* explosion_tex;
    SDL_Texture* boss_tex;
    SDL_Texture* bullet_player_tex;
    SDL_Texture* bullet_enemy_tex;
    SDL_Texture* saucer_tex;
    SDL_Texture* damage_tex; // From Oldassets
    
    // Fonts
    TTF_Font* font_large;
    TTF_Font* font_small;
    
    int width, height;
    bool initialized;
    uint32_t last_frame_time;
} SDLView;

SDLView* sdl_view_create(void);
void sdl_view_destroy(SDLView* view);
bool sdl_view_init(SDLView* view, int width, int height);
bool sdl_view_poll_event(SDLView* view, SDL_Event* event);
void sdl_view_render(SDLView* view, const GameModel* model);

#endif