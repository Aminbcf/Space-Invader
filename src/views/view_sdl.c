#include "view_sdl.h"
#include "rect_utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- MINIAUDIO IMPLEMENTATION ---
#define MINIAUDIO_IMPLEMENTATION
#include "../utils/miniaudio.h"

// Includes for SDL3
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

/* --- Color Constants --- */
#define COLOR_DARK_SPACE 0, 0, 0, 255
#define COLOR_SPACE_BG 0, 0, 0, 255
#define COLOR_HUD_BG 20, 25, 45, 220
#define COLOR_PLAYER 0, 200, 255, 255
#define COLOR_ENEMY 255, 80, 100, 255
#define COLOR_BULLET_PLAYER 0, 255, 200, 255
#define COLOR_BULLET_ENEMY 255, 150, 50, 255
#define COLOR_EXPLOSION 255, 180, 50, 255
#define COLOR_TEXT_HIGHLIGHT 0, 255, 200, 255
#define COLOR_TEXT_PRIMARY 220, 240, 255, 255
#define COLOR_TEXT_SECONDARY 255, 200, 100, 255

/* --- Helper: Draw Text --- */
void draw_fallback_text(SDLView *view, const char *text, int x, int y,
                        uint8_t r, uint8_t g, uint8_t b) {
  if (!view || !view->renderer)
    return;
  SDL_SetRenderDrawColor(view->renderer, r, g, b, 255);
  int cursor = x;
  for (int i = 0; text[i]; i++) {
    SDL_FRect rect = {(float)cursor, (float)y, 8.0f, 12.0f};
    SDL_RenderFillRect(view->renderer, &rect);
    cursor += 10;
  }
}

/* --- Core View Functions --- */

SDLView *sdl_view_create(void) {
  SDLView *view = malloc(sizeof(SDLView));
  if (view) {
    memset(view, 0, sizeof(SDLView));
    view->last_frame_time = SDL_GetTicks();
    view->frame_count = 0;
    view->fps = 0;
    view->last_shots_fired = 0;
    view->last_score = 0;
    view->last_enemy_bullet_count = 0;
    view->last_player_lives = 3; // Initialize with starting lives
    view->game_over_played = false;
    view->current_music_track = 0;
  }
  return view;
}

void sdl_view_destroy(SDLView *view) {
  if (!view)
    return;

  // 1. Cleanup Audio
  ma_sound_uninit(&view->sfx_shoot);
  ma_sound_uninit(&view->sfx_death);
  ma_sound_uninit(&view->sfx_enemy_bullet);
  ma_sound_uninit(&view->sfx_gameover);
  ma_sound_uninit(&view->sfx_damage);
  ma_sound_uninit(&view->sfx_select);
  ma_sound_uninit(&view->music_game);
  ma_sound_uninit(&view->music_boss);
  ma_sound_uninit(&view->music_victory);
  ma_engine_uninit(&view->audio_engine);

  // 2. Cleanup Textures
  for (int p = 0; p < 2; p++) {
    for (int f = 0; f < 2; f++) {
      if (view->player_tex[p][f])
        SDL_DestroyTexture(view->player_tex[p][f]);
    }
  }

  for (int j = 0; j < 2; j++) {
    if (view->boss_tex[j])
      SDL_DestroyTexture(view->boss_tex[j]);
    if (view->saucer_tex[j])
      SDL_DestroyTexture(view->saucer_tex[j]);
    if (view->big_invader_tex[j])
      SDL_DestroyTexture(view->big_invader_tex[j]);
  }

  if (view->explosion_tex)
    SDL_DestroyTexture(view->explosion_tex);
  if (view->bullet_player_tex)
    SDL_DestroyTexture(view->bullet_player_tex);
  if (view->bullet_enemy_tex)
    SDL_DestroyTexture(view->bullet_enemy_tex);
  if (view->bullet_laser_tex)
    SDL_DestroyTexture(view->bullet_laser_tex);
  if (view->bullet_zigzag_tex)
    SDL_DestroyTexture(view->bullet_zigzag_tex);
  if (view->damage_tex)
    SDL_DestroyTexture(view->damage_tex);

  for (int p = 0; p < 5; p++) {
    if (view->pwr_tex[p])
      SDL_DestroyTexture(view->pwr_tex[p]);
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      if (view->invader_tex[i][j])
        SDL_DestroyTexture(view->invader_tex[i][j]);
    }
  }

  // 3. Cleanup Fonts and Systems
  if (view->font_large)
    TTF_CloseFont(view->font_large);
  if (view->font_small)
    TTF_CloseFont(view->font_small);
  if (view->renderer)
    SDL_DestroyRenderer(view->renderer);
  if (view->window)
    SDL_DestroyWindow(view->window);

  TTF_Quit();
  SDL_Quit();

  free(view);
}

bool sdl_view_load_resources(SDLView *view) {
  if (!view)
    return false;
  bool success = true;

  // --- LOAD AUDIO (Miniaudio) ---
  if (ma_sound_init_from_file(
          &view->audio_engine, "assets/shooting_improved.wav",
          MA_SOUND_FLAG_DECODE, NULL, NULL, &view->sfx_shoot) != MA_SUCCESS) {
    fprintf(stderr, "Warning: Failed to load assets/shooting_improved.wav\n");
  }

  if (ma_sound_init_from_file(&view->audio_engine, "assets/explosion.mp3",
                              MA_SOUND_FLAG_DECODE, NULL, NULL,
                              &view->sfx_death) != MA_SUCCESS) {
    fprintf(stderr, "Warning: Failed to load assets/explosion.mp3\n");
  }

  if (ma_sound_init_from_file(&view->audio_engine, "assets/enemy_bullet.wav",
                              MA_SOUND_FLAG_DECODE, NULL, NULL,
                              &view->sfx_enemy_bullet) != MA_SUCCESS) {
    fprintf(stderr, "Warning: Failed to load assets/enemy_bullet.wav\n");
  }

  if (ma_sound_init_from_file(&view->audio_engine, "assets/gameover.wav",
                              MA_SOUND_FLAG_DECODE, NULL, NULL,
                              &view->sfx_gameover) != MA_SUCCESS) {
    fprintf(stderr, "Warning: Failed to load assets/gameover.wav\n");
  }

  if (ma_sound_init_from_file(&view->audio_engine, "assets/damage.wav",
                              MA_SOUND_FLAG_DECODE, NULL, NULL,
                              &view->sfx_damage) != MA_SUCCESS) {
    fprintf(stderr, "Warning: Failed to load assets/damage.wav\n");
  }

  if (ma_sound_init_from_file(&view->audio_engine, "assets/select.wav",
                              MA_SOUND_FLAG_DECODE, NULL, NULL,
                              &view->sfx_select) != MA_SUCCESS) {
    fprintf(stderr, "Warning: Failed to load assets/select.wav\n");
  }

  if (ma_sound_init_from_file(&view->audio_engine, "assets/music_game.mp3",
                              MA_SOUND_FLAG_DECODE, NULL, NULL,
                              &view->music_game) != MA_SUCCESS) {
    fprintf(stderr, "AUDIO ERROR: Failed to load assets/music_game.mp3\n");
  } else {
    printf("AUDIO: Loaded assets/music_game.mp3 successfully\n");
  }
  ma_sound_set_looping(&view->music_game, MA_TRUE);
  ma_sound_set_volume(&view->music_game, 1.0f); // Default initial volume, will be updated by model

  ma_sound_init_from_file(&view->audio_engine, "assets/music_boss.wav",
                          MA_SOUND_FLAG_DECODE, NULL, NULL, &view->music_boss);
  ma_sound_set_looping(&view->music_boss, MA_TRUE);

  ma_sound_init_from_file(&view->audio_engine, "assets/music_victory.wav",
                          MA_SOUND_FLAG_DECODE, NULL, NULL,
                          &view->music_victory);
  ma_sound_set_looping(&view->music_victory, MA_TRUE);

  // --- LOAD FONTS ---
  const char *font_paths[] = {
      "fonts/venite-adoremus-font/VeniteAdoremus-rgRBA.ttf", "assets/font.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", NULL};

  bool font_loaded = false;
  for (int i = 0; font_paths[i]; i++) {
    view->font_large = TTF_OpenFont(font_paths[i], 48);
    if (view->font_large) {
      view->font_small = TTF_OpenFont(font_paths[i], 18);
      if (view->font_small) {
        font_loaded = true;
        break;
      } else {
        TTF_CloseFont(view->font_large);
        view->font_large = NULL;
      }
    }
  }
  if (!font_loaded) {
    fprintf(stderr, "Error: Could not load any gameplay fonts.\n");
    success = false;
  }

// --- LOAD TEXTURES ---
#define LOAD_TEXTURE_SAFE(path, dest)                                          \
  {                                                                            \
    SDL_Surface *s = IMG_Load(path);                                           \
    if (s) {                                                                   \
      SDL_SetSurfaceColorKey(                                                  \
          s, true,                                                             \
          SDL_MapRGB(SDL_GetPixelFormatDetails(s->format), NULL, 0, 0, 0));    \
      dest = SDL_CreateTextureFromSurface(view->renderer, s);                  \
      SDL_DestroySurface(s);                                                   \
      if (dest) {                                                              \
        SDL_SetTextureScaleMode(dest, SDL_SCALEMODE_NEAREST);                  \
        SDL_SetTextureBlendMode(dest, SDL_BLENDMODE_BLEND);                    \
      } else {                                                                 \
        fprintf(stderr, "Error creating texture from %s\n", path);             \
        success = false;                                                       \
      }                                                                        \
    } else {                                                                   \
      fprintf(stderr, "Error loading image %s\n", path);                       \
      success = false;                                                         \
    }                                                                          \
  }
  LOAD_TEXTURE_SAFE("pictures/player_p1_f1.bmp", view->player_tex[0][0]);
  LOAD_TEXTURE_SAFE("pictures/player_p1_f2.bmp", view->player_tex[0][1]);
  LOAD_TEXTURE_SAFE("pictures/player_p2_f1.bmp", view->player_tex[1][0]);
  LOAD_TEXTURE_SAFE("pictures/player_p2_f2.bmp", view->player_tex[1][1]);
  LOAD_TEXTURE_SAFE("pictures/explosion.bmp", view->explosion_tex);
  LOAD_TEXTURE_SAFE("pictures/bullet_player.bmp", view->bullet_player_tex);
  LOAD_TEXTURE_SAFE("pictures/bullet_enemy.bmp", view->bullet_enemy_tex);
  LOAD_TEXTURE_SAFE("pictures/bullet_laser.bmp", view->bullet_laser_tex);
  LOAD_TEXTURE_SAFE("pictures/bullet_zigzag.bmp", view->bullet_zigzag_tex);
  LOAD_TEXTURE_SAFE("pictures/pwr_triple.bmp",
                    view->pwr_tex[1]); // PWR_TRIPLE_SHOT
  LOAD_TEXTURE_SAFE("pictures/pwr_strong.bmp",
                    view->pwr_tex[2]); // PWR_STRONG_MISSILE
  LOAD_TEXTURE_SAFE(
      "pictures/pwr_shield.bmp",
      view->pwr_tex[3]); // PWR_SHIELD (actually shield is index 3 in enum)

  const char *boss_paths[2] = {"pictures/boss_dreadnought_f1.bmp",
                               "pictures/boss_dreadnought_f2.bmp"};
  const char *saucer_paths[2] = {"pictures/bonus_saucer_f1.bmp",
                                 "pictures/bonus_saucer_f2.bmp"};
  for (int j = 0; j < 2; j++) {
    LOAD_TEXTURE_SAFE(boss_paths[j], view->boss_tex[j]);
    LOAD_TEXTURE_SAFE(saucer_paths[j], view->saucer_tex[j]);
  }

  // LOAD BIG INVADER (AS BOSS Clone)
  LOAD_TEXTURE_SAFE("pictures/boss_dreadnought_f1.bmp",
                    view->big_invader_tex[0]);
  LOAD_TEXTURE_SAFE("pictures/boss_dreadnought_f2.bmp",
                    view->big_invader_tex[1]);
  // Apply Tint to distinguish from actual boss (Magenta)
  if (view->big_invader_tex[0])
    SDL_SetTextureColorMod(view->big_invader_tex[0], 255, 100, 255);
  if (view->big_invader_tex[1])
    SDL_SetTextureColorMod(view->big_invader_tex[1], 255, 100, 255);

  const char *invader_paths[3][2] = {
      {"pictures/invader1_1.bmp", "pictures/invader1_2.bmp"},
      {"pictures/invader2_1.bmp", "pictures/invader2_2.bmp"},
      {"pictures/invader3_1.bmp", "pictures/invader3_2.bmp"}};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      LOAD_TEXTURE_SAFE(invader_paths[i][j], view->invader_tex[i][j]);
    }
  }

  // --- INIT STARS (WARP SPEED) ---
  // --- INIT STARS (3D RADIAL) ---
  for (int i = 0; i < 200; i++) {
    // Random spread around center (0,0 is center in 3D space)
    view->stars[i].x = (float)((rand() % 2000) - 1000);
    view->stars[i].y = (float)((rand() % 2000) - 1000);
    view->stars[i].z = (float)(rand() % 1000) + 1; // Depth 1..1000
    view->stars[i].speed = 5.0f + (rand() % 10);   // Z-speed
    view->stars[i].size = (rand() % 2) + 1;
    view->stars[i].alpha = 255;
  }

  return success;
}

bool sdl_view_init(SDLView *view, int width, int height) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init Failed: %s\n", SDL_GetError());
    return false;
  }
  if (!TTF_Init())
    return false;

  // --- INIT MINIAUDIO ---
  ma_result result = ma_engine_init(NULL, &view->audio_engine);
  if (result != MA_SUCCESS) {
    fprintf(stderr,
            "AUDIO ERROR: Failed to initialize audio engine (error %d). Game "
            "will play without sound.\n",
            result);
  } else {
    printf("AUDIO: Engine initialized successfully.\n");
  }

  view->window =
      SDL_CreateWindow("Space Invader", width, height, SDL_WINDOW_RESIZABLE);
  if (!view->window)
    return false;

  view->renderer = SDL_CreateRenderer(view->window, NULL);
  if (!view->renderer)
    return false;

  // Set logical size for automatic scaling
  SDL_SetRenderLogicalPresentation(view->renderer, GAME_AREA_WIDTH + 200,
                                   SCREEN_HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
  view->width = width;
  view->height = height;

  if (!sdl_view_load_resources(view)) {
    fprintf(stderr, "Error: Failed to load some or all resources.\n");
    // Return false to exit if resources are missing
    return false;
  }
  view->initialized = true;

  return true;
}

bool sdl_view_poll_event(SDLView *view, SDL_Event *event) {
  (void)view;
  return SDL_PollEvent(event);
}

// ... (Helper Functions) ...
void draw_text(SDLView *view, const char *text, int x, int y, SDL_Color col) {
  if (!view || !view->renderer)
    return;
  if (view->font_small) {
    SDL_Surface *s = TTF_RenderText_Blended(view->font_small, text, 0, col);
    if (s) {
      SDL_Texture *t = SDL_CreateTextureFromSurface(view->renderer, s);
      SDL_FRect dst = {(float)x, (float)y, (float)s->w, (float)s->h};
      SDL_RenderTexture(view->renderer, t, NULL, &dst);
      SDL_DestroySurface(s);
      SDL_DestroyTexture(t);
    }
  } else {
    draw_fallback_text(view, text, x, y, col.r, col.g, col.b);
  }
}
void draw_text_centered(SDLView *view, const char *text, int y, SDL_Color col,
                        bool large) {
  if (!view || !view->renderer)
    return;
  TTF_Font *font = large ? view->font_large : view->font_small;
  if (font) {
    SDL_Surface *s = TTF_RenderText_Blended(font, text, 0, col);
    if (s) {
      int x = (view->width - s->w) / 2;
      SDL_Texture *t = SDL_CreateTextureFromSurface(view->renderer, s);
      SDL_FRect dst = {(float)x, (float)y, (float)s->w, (float)s->h};
      SDL_RenderTexture(view->renderer, t, NULL, &dst);
      SDL_DestroySurface(s);
      SDL_DestroyTexture(t);
    }
  } else {
    draw_fallback_text(view, text, (view->width - (int)strlen(text) * 10) / 2,
                       y, col.r, col.g, col.b);
  }
}
void draw_particle_effect(SDLView *view, float x, float y, float size,
                          Uint32 color) {
  if (!view || !view->renderer)
    return;
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_ADD);
  SDL_SetRenderDrawColor(view->renderer, (color >> 16) & 0xFF,
                         (color >> 8) & 0xFF, color & 0xFF, 100);
  for (int i = 0; i < 4; i++) {
    float angle = (view->frame_count * 5 + i * 90) * 3.14159f / 180.0f;
    float px = x + cosf(angle) * size * 0.5f;
    float py = y + sinf(angle) * size * 0.5f;
    SDL_FRect particle = {px - 1, py - 1, 2.0f, 2.0f};
    SDL_RenderFillRect(view->renderer, &particle);
  }
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
}

void sdl_view_draw_hud(SDLView *view, const GameModel *model) {
  SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255); // PURE BLACK
  SDL_FRect hud_bg = {600.0f, 0.0f, 200.0f, 600.0f};
  SDL_RenderFillRect(view->renderer, &hud_bg);
  // --- UI / HUD ---
  // 1. Semi-transparent HUD Bar (Top)
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(view->renderer, 0, 0, 40,
                         200); // Dark Blue, Transparent
  SDL_FRect hud_rect = {0, 0, (float)view->width, 50.0f};
  SDL_RenderFillRect(view->renderer, &hud_rect);

  // 2. Game Border
  SDL_SetRenderDrawColor(view->renderer, 0, 200, 255, 150); // Cyan glow
  SDL_FRect border_rect = {10.0f, 55.0f, 580.0f, 535.0f};
  SDL_RenderRect(view->renderer, &border_rect);

  // 3. Vertical Separator
  SDL_SetRenderDrawColor(view->renderer, 100, 100, 100, 255);
  SDL_RenderLine(view->renderer, 600.0f, 0, 600.0f, 600.0f);

  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_NONE);

  // Score
  SDL_Color color_score = {255, 215, 0, 255}; // Gold
  char score_text[32];
  sprintf(score_text, "SCORE: %05d", model->players[0].score);
  // Assuming render_text is a helper function similar to draw_text but takes
  // font directly For now, using draw_text with view->font_large
  draw_text(view, score_text, 20, 10, color_score);

  // Lives
  SDL_Color color_lives = {0, 255, 255, 255}; // Cyan
  char lives_text[32];
  sprintf(lives_text, "LIVES: %d", model->players[0].lives);
  // Assuming render_text is a helper function similar to draw_text but takes
  // font directly For now, using draw_text with view->font_large
  draw_text(view, lives_text, view->width - 150, 10, color_lives);

  for (int i = 0; i < model->players[0].lives && i < 5; i++) {
    SDL_FRect life = {(float)(620 + i * 25), 90.0f, 20.0f, 15.0f};
    if (view->player_tex[0][0])
      SDL_RenderTexture(view->renderer, view->player_tex[0][0], NULL, &life);
  }

  char buf[64];
  SDL_Color title_col = {COLOR_TEXT_HIGHLIGHT};
  SDL_Color val_col = {COLOR_TEXT_SECONDARY};

  // Player 1
  snprintf(buf, 64, "P1 SCORE");
  draw_text(view, buf, 620, 30, title_col);
  snprintf(buf, 64, "%06d", model->players[0].score);
  draw_text(view, buf, 620, 60, val_col);

  // Player 2
  if (model->two_player_mode) {
    snprintf(buf, 64, "P2 SCORE");
    draw_text(view, buf, 620, 150, title_col);
    snprintf(buf, 64, "%06d", model->players[1].score);
    draw_text(view, buf, 620, 180, val_col);

    for (int i = 0; i < model->players[1].lives && i < 5; i++) {
      SDL_FRect life = {(float)(620 + i * 25), 210.0f, 20.0f, 15.0f};
      if (view->player_tex[1][0])
        SDL_RenderTexture(view->renderer, view->player_tex[1][0], NULL, &life);
      else if (view->player_tex[0][0])
        SDL_RenderTexture(view->renderer, view->player_tex[0][0], NULL, &life);
    }
  }

  // Power-up status
  for (int p = 0; p < 2; p++) {
    if (model->players[p].is_active &&
        model->players[p].active_powerup != PWR_NONE) {
      int py = (p == 0) ? 115 : 235;
      SDL_FRect pwr_rect = {620.0f, (float)py, 20.0f, 20.0f};
      int type = (int)model->players[p].active_powerup;
      if (type < 5 && view->pwr_tex[type]) {
        SDL_RenderTexture(view->renderer, view->pwr_tex[type], NULL, &pwr_rect);
      }
      // Timer bar
      SDL_SetRenderDrawColor(view->renderer, 50, 50, 50, 255);
      SDL_FRect t_bg = {650.0f, (float)py + 5, 100.0f, 10.0f};
      SDL_RenderFillRect(view->renderer, &t_bg);

      SDL_SetRenderDrawColor(view->renderer, 255, 255, 0, 255);
      SDL_FRect t_fill = {650.0f, (float)py + 5,
                          model->players[p].powerup_timer * 20.0f, 10.0f};
      SDL_RenderFillRect(view->renderer, &t_fill);
    }
  }

  snprintf(buf, 64, "LEVEL");
  draw_text(view, buf, 620, 300, title_col);
  snprintf(buf, 64, "%d", model->players[0].level);
  draw_text(view, buf, 620, 330, val_col);

  // Boss HP
  if (model->boss.alive) {
    draw_text(view, "MOTHERSHIP HP", 620, 380, (SDL_Color){255, 50, 50, 255});
    SDL_SetRenderDrawColor(view->renderer, 50, 0, 0, 255);
    SDL_FRect hp_bg = {620.0f, 410.0f, 150.0f, 15.0f};
    SDL_RenderFillRect(view->renderer, &hp_bg);

    SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255);
    float hp_w = (model->boss.health * 150.0f) / model->boss.max_health;
    SDL_FRect hp_fill = {620.0f, 410.0f, hp_w, 15.0f};
    SDL_RenderFillRect(view->renderer, &hp_fill);
  }

  snprintf(buf, 64, "HIGH SCORE");
  draw_text(view, buf, 620, 450, title_col);
  snprintf(buf, 64, "%06d", model->high_score);
  draw_text(view, buf, 620, 480, val_col);
}

// Menu rendering helper functions
static void sdl_view_render_main_menu(SDLView *view, const GameModel *model) {
  // Background gradient
  for (int i = 0; i < view->height; i++) {
    SDL_SetRenderDrawColor(view->renderer, 10, 15 + i / 20, 30 + i / 10, 255);
    SDL_FRect line = {0, (float)i, (float)view->width, 1};
    SDL_RenderFillRect(view->renderer, &line);
  }

  draw_text_centered(view, "SPACE INVADERS", 100,
                     (SDL_Color){COLOR_TEXT_HIGHLIGHT}, true);

  // Box background for menu
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(view->renderer, 0, 0, 50, 200);
  SDL_FRect box = {(float)(view->width / 2 - 200), 200.0f, 400.0f, 300.0f};
  SDL_RenderFillRect(view->renderer, &box);
  SDL_SetRenderDrawColor(view->renderer, 0, 200, 255, 255);
  SDL_RenderRect(view->renderer, &box);
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_NONE);

  draw_text_centered(view, "by Amine Boucif", 160,
                     (SDL_Color){COLOR_TEXT_SECONDARY}, false);

  // Menu items
  const char *items[] = {"START 1P", "START 2P", "SETTINGS", "QUIT"};
  int y_start = 250;
  int y_spacing = 60;

  for (int i = 0; i < 4; i++) {
    SDL_Color color = (model->menu_selection == i)
                          ? (SDL_Color){255, 255, 0, 255}
                          : (SDL_Color){COLOR_TEXT_PRIMARY};

    const char *prefix = (model->menu_selection == i) ? "> " : "  ";
    char text[64];
    snprintf(text, sizeof(text), "%s%s", prefix, items[i]);
    draw_text_centered(view, text, y_start + i * y_spacing, color, false);
  }

  draw_text_centered(view, "Use UP/DOWN arrows to navigate, ENTER to select",
                     view->height - 50, (SDL_Color){COLOR_TEXT_SECONDARY},
                     false);
}

static void sdl_view_render_difficulty_menu(SDLView *view,
                                            const GameModel *model) {
  // Background gradient
  for (int i = 0; i < view->height; i++) {
    SDL_SetRenderDrawColor(view->renderer, 10, 15 + i / 20, 30 + i / 10, 255);
    SDL_FRect line = {0, (float)i, (float)view->width, 1};
    SDL_RenderFillRect(view->renderer, &line);
  }

  draw_text_centered(view, "SELECT DIFFICULTY", 100,
                     (SDL_Color){COLOR_TEXT_HIGHLIGHT}, true);

  // Difficulty items with descriptions
  const char *items[] = {
      "EASY   - 3 Levels Only", "NORMAL - 4 Levels with Boss",
      "HARD   - Faster & Harder", "ROGUE  - Infinite & Randomized"};
  int y_start = 230;
  int y_spacing = 60;

  for (int i = 0; i < 4; i++) {
    SDL_Color color = (model->menu_selection == i)
                          ? (SDL_Color){255, 255, 0, 255}
                          : (SDL_Color){COLOR_TEXT_PRIMARY};

    const char *prefix = (model->menu_selection == i) ? "> " : "  ";
    char text[128];
    snprintf(text, sizeof(text), "%s%s", prefix, items[i]);
    draw_text_centered(view, text, y_start + i * y_spacing, color, false);
  }

  draw_text_centered(view, "ENTER to select, ESC to go back", view->height - 50,
                     (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

static void sdl_view_render_settings_menu(SDLView *view,
                                          const GameModel *model) {
  // Background gradient
  for (int i = 0; i < view->height; i++) {
    SDL_SetRenderDrawColor(view->renderer, 10, 15 + i / 20, 30 + i / 10, 255);
    SDL_FRect line = {0, (float)i, (float)view->width, 1};
    SDL_RenderFillRect(view->renderer, &line);
  }

  draw_text_centered(view, "SETTINGS", 100, (SDL_Color){COLOR_TEXT_HIGHLIGHT},
                     true);

  // Menu items
  int y_start = 230;
  int y_spacing = 70;

  // Controls
  SDL_Color color0 = (model->menu_selection == 0)
                         ? (SDL_Color){255, 255, 0, 255}
                         : (SDL_Color){COLOR_TEXT_PRIMARY};
  draw_text_centered(view,
                     model->menu_selection == 0 ? "> CONTROLS" : "  CONTROLS",
                     y_start, color0, false);

  // Music Volume with slider
  SDL_Color color1 = (model->menu_selection == 1)
                         ? (SDL_Color){255, 255, 0, 255}
                         : (SDL_Color){COLOR_TEXT_PRIMARY};
  char vol_text[64];
  int vol_percent = (int)(model->music_volume * 100);
  snprintf(vol_text, sizeof(vol_text), "%s MUSIC VOLUME: %d%%",
           model->menu_selection == 1 ? ">" : " ", vol_percent);
  draw_text_centered(view, vol_text, y_start + y_spacing, color1, false);

  // Volume slider bar
  if (model->menu_selection == 1) {
    int bar_width = 300;
    int bar_x = (view->width - bar_width) / 2;
    int bar_y = y_start + y_spacing + 40;

    // Background bar
    SDL_SetRenderDrawColor(view->renderer, 50, 50, 50, 255);
    SDL_FRect bar_bg = {(float)bar_x, (float)bar_y, (float)bar_width, 20};
    SDL_RenderFillRect(view->renderer, &bar_bg);

    // Filled portion
    SDL_SetRenderDrawColor(view->renderer, 0, 255, 100, 255);
    SDL_FRect bar_fill = {(float)bar_x, (float)bar_y,
                          bar_width * model->music_volume, 20};
    SDL_RenderFillRect(view->renderer, &bar_fill);

    draw_text_centered(view, "Use LEFT/RIGHT to adjust", bar_y + 40,
                       (SDL_Color){200, 200, 200, 255}, false);
  }

  // Back
  SDL_Color color2 = (model->menu_selection == 2)
                         ? (SDL_Color){255, 255, 0, 255}
                         : (SDL_Color){COLOR_TEXT_PRIMARY};
  draw_text_centered(view, model->menu_selection == 2 ? "> BACK" : "  BACK",
                     y_start + 2 * y_spacing + 80, color2, false);

  draw_text_centered(view, "ENTER to select, ESC to go back", view->height - 50,
                     (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

static const char *get_key_name(int keycode) {
  static char buf[32];
  switch (keycode) {
  case 1073741904:
    return "LEFT";
  case 1073741903:
    return "RIGHT";
  case 1073741906:
    return "UP";
  case 1073741905:
    return "DOWN";
  case 32:
    return "SPACE";
  case 1073742049:
    return "L-SHIFT";
  case 1073742048:
    return "L-CTRL";
  case 13:
    return "ENTER";
  case 27:
    return "ESC";
  case 97:
    return "A";
  case 98:
    return "B";
  case 99:
    return "C";
  case 100:
    return "D";
  case 101:
    return "E";
  case 102:
    return "F";
  case 103:
    return "G";
  case 104:
    return "H";
  case 105:
    return "I";
  case 106:
    return "J";
  case 107:
    return "K";
  case 108:
    return "L";
  case 109:
    return "M";
  case 110:
    return "N";
  case 111:
    return "O";
  case 112:
    return "P";
  case 113:
    return "Q";
  case 114:
    return "R";
  case 115:
    return "S";
  case 116:
    return "T";
  case 117:
    return "U";
  case 118:
    return "V";
  case 119:
    return "W";
  case 120:
    return "X";
  case 121:
    return "Y";
  case 122:
    return "Z";
  default:
    snprintf(buf, 32, "%d", keycode);
    return buf;
  }
}

static void sdl_view_render_controls_menu(SDLView *view,
                                          const GameModel *model) {
  // Background gradient (Black to Dark Blue)
  for (int i = 0; i < view->height; i++) {
    SDL_SetRenderDrawColor(view->renderer, 0, 0, i / 20, 255);
    SDL_FRect line = {0, (float)i, (float)view->width, 1};
    SDL_RenderFillRect(view->renderer, &line);
  }

  draw_text_centered(view, "CONTROLS", 60, (SDL_Color){COLOR_TEXT_HIGHLIGHT},
                     true);

  if (model->waiting_for_key) {
    draw_text_centered(view, "PRESS A KEY...", 100,
                       (SDL_Color){255, 255, 0, 255}, false);
  }

  SDL_Color wht = {255, 255, 255, 255};
  SDL_Color blu = {100, 150, 255, 255};
  SDL_Color red = {255, 100, 100, 255};
  SDL_Color sel = {255, 255, 0, 255};

  const char *actions[] = {"LEFT", "RIGHT", "UP", "DOWN", "SHOOT"};
  int y_start = 140;
  int y_spacing = 30;

  // Player 1 keybindings
  draw_text_centered(view, "PLAYER 1 (BLUE)", y_start, blu, false);
  for (int i = 0; i < 5; i++) {
    char buf[64];
    snprintf(buf, 64, "%s: %s", actions[i],
             get_key_name(model->keybinds_p1[i]));
    SDL_Color c = (model->menu_selection == i) ? sel : wht;
    if (model->waiting_for_key && model->editing_keybind == i)
      c = (SDL_Color){0, 255, 100, 255};
    draw_text_centered(view, buf, y_start + 30 + i * y_spacing, c, false);
  }

  // Player 2 keybindings
  draw_text_centered(view, "PLAYER 2 (RED)", y_start + 200, red, false);
  for (int i = 0; i < 5; i++) {
    char buf[64];
    snprintf(buf, 64, "%s: %s", actions[i],
             get_key_name(model->keybinds_p2[i]));
    SDL_Color c = (model->menu_selection == 5 + i) ? sel : wht;
    if (model->waiting_for_key && model->editing_keybind == 5 + i)
      c = (SDL_Color){0, 255, 100, 255};
    draw_text_centered(view, buf, y_start + 230 + i * y_spacing, c, false);
  }

  // Back button
  SDL_Color color = (model->menu_selection == 10) ? sel : wht;
  draw_text_centered(view, "> BACK <", view->height - 80, color, false);

  draw_text_centered(view, "ENTER to edit, ESC to go back", view->height - 40,
                     (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

void sdl_view_render_game_scene(SDLView *view, const GameModel *model) {
  if (!view || !view->renderer || !model)
    return;

  // Background
  SDL_SetRenderDrawColor(view->renderer, COLOR_SPACE_BG);
  SDL_FRect space_bg = {0.0f, 0.0f, 600.0f, 600.0f};
  SDL_RenderFillRect(view->renderer, &space_bg);

  SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 100);
  for (int i = 0; i < 50; i++) {
    int seed = (i * 137) % 1000;
    SDL_FRect star = {(float)((seed * 13) % 600), (float)((seed * 17) % 600),
                      2.0f, 2.0f};
    SDL_RenderFillRect(view->renderer, &star);
  }

  // Players
  for (int pIdx = 0; pIdx < 2; pIdx++) {
    if (!model->players[pIdx].is_active)
      continue;

    float p_scale = 2.0f;
    float p_w = (float)model->players[pIdx].hitbox.width * p_scale;
    float p_h = (float)model->players[pIdx].hitbox.height * p_scale;
    float p_x = (float)model->players[pIdx].hitbox.x -
                (p_w - (float)model->players[pIdx].hitbox.width) / 2.0f;
    float p_y = (float)model->players[pIdx].hitbox.y -
                (p_h - (float)model->players[pIdx].hitbox.height) / 2.0f;
    // Shield effect (Bubble)
    if (model->players[pIdx].active_powerup == PWR_SHIELD) {
      SDL_SetRenderDrawColor(view->renderer, 0, 200, 255, 100);
      float cx = (float)model->players[pIdx].hitbox.x +
                 (float)model->players[pIdx].hitbox.width / 2;
      float cy = (float)model->players[pIdx].hitbox.y +
                 (float)model->players[pIdx].hitbox.height / 2;
      float r = 35.0f + sinf(view->frame_count * 0.1f) * 3.0f;

      // Draw octagon as "bubble"
      SDL_FPoint points[9];
      for (int k = 0; k < 8; k++) {
        float angle = k * (3.14159f / 4.0f);
        points[k].x = cx + cosf(angle) * r;
        points[k].y = cy + sinf(angle) * r;
      }
      points[8] = points[0]; // Close loop
      SDL_RenderLines(view->renderer, points, 9);

      SDL_SetRenderDrawColor(view->renderer, 0, 100, 255, 50);
      // Inner bubble
      r -= 5.0f;
      for (int k = 0; k < 8; k++) {
        float angle = k * (3.14159f / 4.0f);
        points[k].x = cx + cosf(angle) * r;
        points[k].y = cy + sinf(angle) * r;
      }
      points[8] = points[0];
      SDL_RenderLines(view->renderer, points, 9);
    }

    // Triple shot indicator
    if (model->players[pIdx].active_powerup == PWR_TRIPLE_SHOT) {
      draw_particle_effect(view,
                           (float)model->players[pIdx].hitbox.x +
                               (float)model->players[pIdx].hitbox.width / 2,
                           (float)model->players[pIdx].hitbox.y +
                               (float)model->players[pIdx].hitbox.height / 2,
                           40.0f, 0x00FFFF);
    }

    // Power-up collection particles (if just collected, but we don't have a
    // timer for that here, so just subtle glow)
    if (model->players[pIdx].active_powerup != PWR_NONE) {
      draw_particle_effect(view,
                           (float)model->players[pIdx].hitbox.x +
                               (float)model->players[pIdx].hitbox.width / 2,
                           (float)model->players[pIdx].hitbox.y +
                               (float)model->players[pIdx].hitbox.height,
                           20.0f, 0xFFFFFF);
    }

    SDL_FRect p_dst = {p_x, p_y, p_w, p_h};

    int p_anim_idx = (view->frame_count / 15) % 2;
    if (view->player_tex[pIdx][p_anim_idx]) {
      SDL_RenderTexture(view->renderer, view->player_tex[pIdx][p_anim_idx],
                        NULL, &p_dst);
    } else if (view->player_tex[pIdx][0]) {
      SDL_RenderTexture(view->renderer, view->player_tex[pIdx][0], NULL,
                        &p_dst);
    } else {
      SDL_SetRenderDrawColor(view->renderer, COLOR_PLAYER);
      SDL_RenderFillRect(view->renderer, &p_dst);
    }

    // Combo text
    if (model->players[pIdx].combo_count >= 5) {
      char combo_buf[32];
      snprintf(combo_buf, 32, "x%d", model->players[pIdx].combo_count);
      // Use a smaller offset or different color to make it less intrusive
      draw_text(view, combo_buf, (int)p_x, (int)p_y - 15,
                (SDL_Color){255, 200, 0, 200});
    }
  }

  // Bullets
  for (int pIdx = 0; pIdx < 2; pIdx++) {
    for (int i = 0; i < PLAYER_BULLETS; i++) {
      if (model->player_bullets[pIdx][i].alive) {
        SDL_FRect b = {(float)model->player_bullets[pIdx][i].hitbox.x,
                       (float)model->player_bullets[pIdx][i].hitbox.y,
                       (float)model->player_bullets[pIdx][i].hitbox.width,
                       (float)model->player_bullets[pIdx][i].hitbox.height};
        if (view->bullet_player_tex)
          SDL_RenderTexture(view->renderer, view->bullet_player_tex, NULL, &b);
        else {
          SDL_SetRenderDrawColor(view->renderer, COLOR_BULLET_PLAYER);
          SDL_RenderFillRect(view->renderer, &b);
        }
      }
    }
  }
  for (int i = 0; i < ENEMY_BULLETS; i++)
    if (model->enemy_bullets[i].alive) {
      SDL_FRect b = {(float)model->enemy_bullets[i].hitbox.x,
                     (float)model->enemy_bullets[i].hitbox.y,
                     (float)model->enemy_bullets[i].hitbox.width,
                     (float)model->enemy_bullets[i].hitbox.height};
      // Different visuals for bullet types
      if (model->enemy_bullets[i].type == 2) { // Laser/Orb
        SDL_FRect orb = {b.x - 2, b.y, b.w + 6, b.h + 4};
        if (view->bullet_laser_tex) {
          SDL_RenderTexture(view->renderer, view->bullet_laser_tex, NULL, &orb);
        } else {
          SDL_SetRenderDrawColor(view->renderer, 255, 0, 255, 255);
          SDL_RenderFillRect(view->renderer, &orb);
        }
        draw_particle_effect(view, b.x + b.w / 2, b.y + b.h, 15.0f, 0xFF00FF);
      } else if (model->enemy_bullets[i].type == 1) { // ZigZag
        SDL_FRect zz = {b.x - 1, b.y, b.w + 4, b.h + 2};
        if (view->bullet_zigzag_tex) {
          SDL_RenderTexture(view->renderer, view->bullet_zigzag_tex, NULL, &zz);
        } else {
          SDL_SetRenderDrawColor(view->renderer, 255, 255, 0, 255); // Yellow
          SDL_RenderFillRect(view->renderer, &zz);
        }
      } else { // Standard
        if (view->bullet_enemy_tex)
          SDL_RenderTexture(view->renderer, view->bullet_enemy_tex, NULL, &b);
        else {
          SDL_SetRenderDrawColor(view->renderer, COLOR_BULLET_ENEMY);
          SDL_RenderFillRect(view->renderer, &b);
        }
      }
    }

  // Powerups
  for (int i = 0; i < 10; i++) {
    if (model->powerups[i].alive) {
      float scale = 1.5f;
      float pw_w = model->powerups[i].hitbox.width * scale;
      float pw_h = model->powerups[i].hitbox.height * scale;
      float pw_x = model->powerups[i].hitbox.x -
                   (pw_w - model->powerups[i].hitbox.width) / 2;
      float pw_y = model->powerups[i].hitbox.y -
                   (pw_h - model->powerups[i].hitbox.height) / 2;

      SDL_FRect p_dst = {pw_x, pw_y, pw_w, pw_h};
      int type = (int)model->powerups[i].type;
      if (type < 5 && view->pwr_tex[type]) {
        SDL_RenderTexture(view->renderer, view->pwr_tex[type], NULL, &p_dst);
      } else {
        SDL_SetRenderDrawColor(view->renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(view->renderer, &p_dst);
      }
    }
  }

  // Saucer
  if (model->saucer.alive) {
    float s_scale = 1.5f;
    SDL_FRect s_dst = {(float)model->saucer.hitbox.x -
                           (model->saucer.hitbox.width * (s_scale - 1) / 2),
                       (float)model->saucer.hitbox.y -
                           (model->saucer.hitbox.height * (s_scale - 1) / 2),
                       (float)model->saucer.hitbox.width * s_scale,
                       (float)model->saucer.hitbox.height * s_scale};
    int anim_state = model->invaders.state;
    if (view->saucer_tex[anim_state])
      SDL_RenderTexture(view->renderer, view->saucer_tex[anim_state], NULL,
                        &s_dst);
    else {
      SDL_SetRenderDrawColor(view->renderer, 255, 150, 0, 255);
      SDL_RenderFillRect(view->renderer, &s_dst);
    }
  }

  // Invaders / Boss
  if (model->boss.alive) {
    float b_scale = 2.0f; /* Boss is already big, but let's make it huge */
    SDL_FRect boss = {(float)model->boss.hitbox.x -
                          (model->boss.hitbox.width * (b_scale - 1) / 2),
                      (float)model->boss.hitbox.y -
                          (model->boss.hitbox.height * (b_scale - 1) / 2),
                      (float)model->boss.hitbox.width * b_scale,
                      (float)model->boss.hitbox.height * b_scale};
    int anim_state = model->boss.anim_frame; // Use boss's own animation frame
    if (view->boss_tex[anim_state])
      SDL_RenderTexture(view->renderer, view->boss_tex[anim_state], NULL,
                        &boss);
    else {
      SDL_SetRenderDrawColor(view->renderer, COLOR_ENEMY);
      SDL_RenderFillRect(view->renderer, &boss);
    }
    float pct = (float)model->boss.health / model->boss.max_health;
    SDL_FRect bg = {150.0f, 60.0f, 300.0f, 20.0f};
    SDL_SetRenderDrawColor(view->renderer, 50, 0, 0, 255);
    SDL_RenderFillRect(view->renderer, &bg);
    SDL_FRect fg = {150.0f, 60.0f, 300.0f * (pct > 0 ? pct : 0), 20.0f};
    SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(view->renderer, &fg);
    draw_text(view, "MOTHERSHIP", 150, 85, (SDL_Color){255, 100, 100, 255});
  } else {
    for (int i = 0; i < INVADER_ROWS; i++) {
      for (int j = 0; j < INVADER_COLS; j++) {
        const Invader *inv = &model->invaders.invaders[i][j];
        if (inv->alive) {
          float i_scale = 1.3f;
          SDL_FRect idst = {
              (float)inv->hitbox.x - (inv->hitbox.width * (i_scale - 1) / 2),
              (float)inv->hitbox.y - (inv->hitbox.height * (i_scale - 1) / 2),
              (float)inv->hitbox.width * i_scale,
              (float)inv->hitbox.height * i_scale};
          if (inv->dying_timer > 0) {
            if (view->explosion_tex)
              SDL_RenderTexture(view->renderer, view->explosion_tex, NULL,
                                &idst);
            else {
              SDL_SetRenderDrawColor(view->renderer, COLOR_EXPLOSION);
              SDL_RenderFillRect(view->renderer, &idst);
            }
            draw_particle_effect(view, idst.x + 15, idst.y + 15, 10, 0xFFAA00);
          } else {
            if (view->invader_tex[inv->type][model->invaders.state])
              SDL_RenderTexture(
                  view->renderer,
                  view->invader_tex[inv->type][model->invaders.state], NULL,
                  &idst);
            else {
              SDL_SetRenderDrawColor(view->renderer, 0, 255, 0, 255);
              SDL_RenderFillRect(view->renderer, &idst);
            }
          }
        }
      }
    }

    // Big Invader rendering
    if (model->invaders.big_invader.alive) {
      float bi_scale = 2.0f;
      const BigInvader *bi = &model->invaders.big_invader;
      SDL_FRect bi_dst = {(float)bi->hitbox.x, (float)bi->hitbox.y,
                          (float)bi->hitbox.width * bi_scale,
                          (float)bi->hitbox.height * bi_scale};

      // Draw big invader
      SDL_Texture *tex = view->big_invader_tex[model->invaders.state];
      if (tex) {
        SDL_RenderTexture(view->renderer, tex, NULL, &bi_dst);
      } else {
        SDL_SetRenderDrawColor(view->renderer, 180, 50, 255, 255); // Purple
        SDL_RenderFillRect(view->renderer, &bi_dst);
      }

      // Particle effect around big invader
      draw_particle_effect(view, bi_dst.x + bi_dst.w / 2,
                           bi_dst.y + bi_dst.h / 2, 30.0f, 0xFF00FF);

      // HP bar above big invader
      float hp_pct = (float)bi->health / bi->max_health;
      SDL_FRect hp_bg = {bi_dst.x, bi_dst.y - 10, bi_dst.w, 8};
      SDL_SetRenderDrawColor(view->renderer, 50, 0, 50, 255);
      SDL_RenderFillRect(view->renderer, &hp_bg);
      SDL_FRect hp_fill = {bi_dst.x, bi_dst.y - 10, bi_dst.w * hp_pct, 8};
      SDL_SetRenderDrawColor(view->renderer, 255, 0, 255, 255);
      SDL_RenderFillRect(view->renderer, &hp_fill);
    }
  }

  sdl_view_draw_hud(view, model);
}

void sdl_view_render(SDLView *view, const GameModel *model) {
  if (!view || !view->renderer || !model)
    return;

  // Apply music volume from settings
  ma_sound_set_volume(&view->music_game, model->music_volume);
  ma_sound_set_volume(&view->music_boss, model->music_volume);
  ma_sound_set_volume(&view->music_victory, model->music_volume);

  // Also apply volume to SFX for consistency (since there is no separate SFX slider yet)
  ma_sound_set_volume(&view->sfx_shoot, model->music_volume);
  ma_sound_set_volume(&view->sfx_death, model->music_volume);
  ma_sound_set_volume(&view->sfx_enemy_bullet, model->music_volume);
  ma_sound_set_volume(&view->sfx_gameover, model->music_volume);
  ma_sound_set_volume(&view->sfx_damage, model->music_volume);
  ma_sound_set_volume(&view->sfx_select, model->music_volume);

  // Reset audio tracking when starting new game from menu
  static GameState last_state = STATE_MENU;
  if (last_state == STATE_MENU && model->state == STATE_PLAYING) {
    view->last_shots_fired = 0;
    view->last_score = 0;
    view->last_enemy_bullet_count = 0;
    view->last_player_lives = model->players[0].lives;
  }

  // Menu Navigation Sound
  if (model->state == STATE_MENU) {
    if (model->menu_selection != view->last_menu_selection ||
        model->menu_state != view->last_menu_state) {
      ma_sound_seek_to_pcm_frame(&view->sfx_select, 0);
      ma_sound_start(&view->sfx_select);
      view->last_menu_selection = model->menu_selection;
      view->last_menu_state = model->menu_state;
    }
  }

  last_state = model->state;

  // --- AUDIO LOGIC ---
  // Only play sounds during active gameplay
  if (model->state == STATE_PLAYING) {
    // 1. Detect Shot (Rewind and Play)
    // 1. Detect Shot (Rewind and Play)
    int total_shots =
        model->players[0].shots_fired + model->players[1].shots_fired;
    if (total_shots > (int)view->last_shots_fired) {
      if (ma_sound_is_playing(&view->sfx_shoot)) {
        ma_sound_seek_to_pcm_frame(&view->sfx_shoot, 0);
      } else {
        ma_sound_start(&view->sfx_shoot);
      }
      view->last_shots_fired = total_shots;
    }

    // 2. Detect Enemy Death (Score change)
    // 2. Detect Enemy Death (Score change)
    int total_score = model->players[0].score + model->players[1].score;
    if (total_score > view->last_score) {
      if (ma_sound_is_playing(&view->sfx_death)) {
        ma_sound_seek_to_pcm_frame(&view->sfx_death, 0);
      } else {
        ma_sound_start(&view->sfx_death);
      }
      view->last_score = total_score;
    }
  }

  // 3. Detect Enemy Bullets (count active enemy bullets)
  int current_enemy_bullets = 0;
  for (int i = 0; i < ENEMY_BULLETS; i++) {
    if (model->enemy_bullets[i].alive)
      current_enemy_bullets++;
  }
  if (current_enemy_bullets > view->last_enemy_bullet_count) {
    if (ma_sound_is_playing(&view->sfx_enemy_bullet)) {
      ma_sound_seek_to_pcm_frame(&view->sfx_enemy_bullet, 0);
    } else {
      ma_sound_start(&view->sfx_enemy_bullet);
    }
  }
  view->last_enemy_bullet_count = current_enemy_bullets;

  // 4. Detect Player Damage (life decrease)
  int current_lives = model->players[0].lives + model->players[1].lives;
  if (current_lives < view->last_player_lives) {
    if (ma_sound_is_playing(&view->sfx_damage)) {
      ma_sound_seek_to_pcm_frame(&view->sfx_damage, 0);
    } else {
      ma_sound_start(&view->sfx_damage);
    }
    view->last_player_lives = current_lives;
  }
  if (current_lives > view->last_player_lives) {
    view->last_player_lives = current_lives;
  }

  // 5. Detect Game Over
  if (model->state == STATE_GAME_OVER && !view->game_over_played) {
    ma_sound_start(&view->sfx_gameover);
    view->game_over_played = true;
  }
  // Reset flag when not in game over state
  if (model->state != STATE_GAME_OVER) {
    view->game_over_played = false;
  }

  // 6. Music Switching Logic
  // Start menu music on initial load or when returning to menu
  if (model->state == STATE_MENU && view->current_music_track == 0) {
    printf("AUDIO: Starting game music on menu (track was 0)...\n");
    ma_result r = ma_sound_start(&view->music_game);
    if (r != MA_SUCCESS) {
      printf("AUDIO ERROR: ma_sound_start failed with code %d\n", r);
    }
    view->current_music_track = 1;
  }
  // Switch to boss music on level 4 or boss alive
  else if (model->state == STATE_PLAYING &&
           (model->players[0].level == 4 || model->boss.alive) &&
           model->boss.alive && view->current_music_track != 2) {
    ma_sound_stop(&view->music_game);
    ma_sound_start(&view->music_boss);
    view->current_music_track = 2;
  }
  // Switch back to game music when boss defeated
  else if (model->state == STATE_PLAYING && !model->boss.alive &&
           view->current_music_track == 2) {
    ma_sound_stop(&view->music_boss);
    ma_sound_start(&view->music_game);
    view->current_music_track = 1;
  }
  // Play victory music on win
  else if (model->state == STATE_WIN && view->current_music_track != 3) {
    ma_sound_stop(&view->music_game);
    ma_sound_stop(&view->music_boss);
    ma_sound_start(&view->music_victory);
    view->current_music_track = 3;
  }
  // Stop music on Game Over (play game over SFX instead)
  else if (model->state == STATE_GAME_OVER && view->current_music_track != 4) {
    ma_sound_stop(&view->music_game);
    ma_sound_stop(&view->music_boss);
    ma_sound_stop(&view->music_victory);
    view->current_music_track = 4; // Mark as "game over" state
  }
  // Resume game music when entering STATE_PLAYING from menu
  else if (model->state == STATE_PLAYING && view->current_music_track == 0) {
    ma_sound_start(&view->music_game);
    view->current_music_track = 1;
    view->last_player_lives =
        model->players[0].lives +
        model->players[1].lives; // Sync lives on game start
  }
  // Start/Resume menu music when in menu
  else if (model->state == STATE_MENU && view->current_music_track == 0) {
    printf("AUDIO: Starting menu music...\n");
    ma_sound_start(&view->music_game); // Play game music on menu too
    view->current_music_track = 1;
  }
  // Resume menu music when returning from victory or game over
  else if (model->state == STATE_MENU &&
           (view->current_music_track == 3 || view->current_music_track == 4)) {
    ma_sound_stop(&view->music_victory);
    ma_sound_stop(&view->music_boss);
    if (!ma_sound_is_playing(&view->music_game)) {
      ma_sound_start(&view->music_game);
    }
    view->current_music_track = 1;
  }

  // --- RENDER LOGIC ---
  SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
  SDL_RenderClear(view->renderer);

  // --- RENDER STARS (3D RADIAL WARP) ---
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
  float cx = view->width / 2.0f;
  float cy = view->height / 2.0f;
  float fov = 300.0f; // Field of View scale

  // Increase speed if Player moves? (Optional nuance)
  float speed_mult = 1.0f;

  for (int i = 0; i < 200; i++) {
    // Update Z (Move towards camera)
    view->stars[i].z -= view->stars[i].speed * speed_mult;

    // Reset if passed camera
    if (view->stars[i].z <= 1.0f) {
      view->stars[i].z = 1000.0f;
      view->stars[i].x = (float)((rand() % 2000) - 1000);
      view->stars[i].y = (float)((rand() % 2000) - 1000);
    }

    // Project to 2D
    float scale = fov / view->stars[i].z;
    float sx = view->stars[i].x * scale + cx;
    float sy = view->stars[i].y * scale + cy;

    // Previous position for trail (streak from center)
    // A simple cheat: trail points towards center
    float size = view->stars[i].size * (scale * 0.5f);
    if (size < 1.0f)
      size = 1.0f;
    if (size > 5.0f)
      size = 5.0f;

    // Only draw if on screen
    if (sx >= 0 && sx < view->width && sy >= 0 && sy < view->height) {
      // Brightness based on proximity
      int alpha = (int)(255 * (1.0f - view->stars[i].z / 1000.0f));
      if (alpha > 255)
        alpha = 255;
      if (alpha < 0)
        alpha = 0;

      SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, (uint8_t)alpha);

      // Draw small rect
      SDL_FRect r = {sx, sy, size, size};
      SDL_RenderFillRect(view->renderer, &r);
    }
  }
  SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_NONE); // Reset

  // Use {} for every case to prevent redeclaration errors
  switch (model->state) {
  case STATE_MENU: {
    //  Render the appropriate menu based on menu_state
    switch (model->menu_state) {
    case MENU_MAIN:
      sdl_view_render_main_menu(view, model);
      break;
    case MENU_DIFFICULTY:
      sdl_view_render_difficulty_menu(view, model);
      break;
    case MENU_SETTINGS:
      sdl_view_render_settings_menu(view, model);
      break;
    case MENU_CONTROLS:
      sdl_view_render_controls_menu(view, model);
      break;
    }
    break;
  }
  case STATE_LEVEL_TRANSITION: {
    sdl_view_render_game_scene(view, model);
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 150);
    SDL_FRect overlay = {0, 0, (float)view->width, (float)view->height};
    SDL_RenderFillRect(view->renderer, &overlay);
    char buf[64];
    snprintf(buf, 64, "LEVEL %d", model->players[0].level);
    draw_text_centered(view, buf, 280, (SDL_Color){0, 255, 0, 255}, true);
    draw_text_centered(view, "PRESS SPACE", 350,
                       (SDL_Color){255, 255, 255, 255}, false);
    break;
  }
  case STATE_WIN: {
    sdl_view_render_game_scene(view, model);
    draw_text_centered(view, "MISSION ACCOMPLISHED!", 250,
                       (SDL_Color){0, 255, 0, 255}, true);
    char buf[64];
    snprintf(buf, 64, "Final Score: %d",
             model->players[0].score + model->players[1].score);
    draw_text_centered(view, buf, 320, (SDL_Color){255, 255, 255, 255}, false);
    draw_text_centered(view, "Press any key for Menu", 380,
                       (SDL_Color){255, 255, 255, 255}, false);
    break;
  }
  case STATE_GAME_OVER: {
    sdl_view_render_game_scene(view, model);
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 50, 0, 0, 150);
    SDL_FRect overlay2 = {0, 0, (float)view->width, (float)view->height};
    SDL_RenderFillRect(view->renderer, &overlay2);
    draw_text_centered(view, "GAME OVER", 280, (SDL_Color){255, 0, 0, 255},
                       true);
    draw_text_centered(view, "Press any key for Menu", 350,
                       (SDL_Color){255, 255, 255, 255}, false);
    break;
  }
  case STATE_PAUSED: {
    sdl_view_render_game_scene(view, model);
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 150);
    SDL_FRect overlay3 = {0, 0, (float)view->width, (float)view->height};
    SDL_RenderFillRect(view->renderer, &overlay3);
    draw_text_centered(view, "PAUSED", 300, (SDL_Color){255, 255, 255, 255},
                       true);
    break;
  }
  default: {
    sdl_view_render_game_scene(view, model);
    break;
  }
  }

  SDL_RenderPresent(view->renderer);
  view->frame_count++;
}