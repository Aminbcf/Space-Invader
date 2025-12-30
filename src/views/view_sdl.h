#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../utils/miniaudio.h"  
#include "../core/model.h"
#include <stdbool.h>

typedef struct SDLView {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool initialized;
    int width;
    int height;
    
    // Textures
    SDL_Texture* player_tex[2];
    SDL_Texture* boss_tex[2];    
    SDL_Texture* saucer_tex[2]; 
    SDL_Texture* explosion_tex;
    SDL_Texture* bullet_player_tex;
    SDL_Texture* bullet_enemy_tex;
    SDL_Texture* damage_tex;
    SDL_Texture* invader_tex[3][2];
    
    // --- AUDIO (Miniaudio) ---
    ma_engine audio_engine;      // The main audio system
    ma_sound sfx_shoot;          // Sound object
    ma_sound sfx_explosion;      // Sound object
    ma_sound music_bg;           // Background music
    
    // Audio State Tracking
    unsigned int last_shots_fired;
    int last_score;
    
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