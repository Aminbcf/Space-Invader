#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include "../core/model.h"
#include "../utils/miniaudio.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

typedef struct SDLView {
  SDL_Window *window;
  SDL_Renderer *renderer;
  bool initialized;
  int width;
  int height;

  // Textures
  SDL_Texture *player_tex[2][2]; // [player_id][frame]
  SDL_Texture *pwr_tex[5]; // PWR_MAX is 5
  SDL_Texture *boss_tex[2];
  SDL_Texture *saucer_tex[2];
  SDL_Texture *big_invader_tex[2];
  SDL_Texture *explosion_tex;
  SDL_Texture *bullet_player_tex;
  SDL_Texture *bullet_enemy_tex;
  SDL_Texture *bullet_laser_tex;
  SDL_Texture *bullet_zigzag_tex;
  SDL_Texture *damage_tex;
  SDL_Texture *invader_tex[3][2];

  // --- AUDIO (Miniaudio) ---
  ma_engine audio_engine;    // The main audio system
  ma_sound sfx_shoot;        // Sound object
  ma_sound sfx_death;        // Enemy death sound (renamed from explosion)
  ma_sound sfx_enemy_bullet; // Enemy bullet sound
  ma_sound sfx_gameover;     // Game over sound
  ma_sound sfx_damage;       // Player damage sound
  ma_sound music_game;       // Game music (levels 1-3)
  ma_sound music_boss;       // Boss fight music
  ma_sound music_victory;    // Victory music

  // Audio State Tracking
  unsigned int last_shots_fired;
  int last_score;
  int last_enemy_bullet_count;
  int last_player_lives;
  bool game_over_played;
  int current_music_track; // 0=none, 1=game, 2=boss, 3=victory

  // Fonts
  TTF_Font *font_large;
  TTF_Font *font_small;

  // Timing
  Uint32 frame_count;
  Uint32 fps;
  Uint32 last_frame_time;
  
  // Background Stars
  struct {
    float x, y;
    float speed;
    int size;
    uint8_t alpha;
  } stars[100];
} SDLView;

SDLView *sdl_view_create(void);
void sdl_view_destroy(SDLView *view);
bool sdl_view_init(SDLView *view, int width, int height);
bool sdl_view_poll_event(SDLView *view, SDL_Event *event);
void sdl_view_render(SDLView *view, const GameModel *model);

#endif