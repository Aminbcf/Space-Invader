#include "view_sdl.h"
#include "rect_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- MINIAUDIO IMPLEMENTATION ---
#define MINIAUDIO_IMPLEMENTATION
#include "../utils/miniaudio.h"

// Includes for SDL3
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

/* --- Color Constants --- */
#define COLOR_DARK_SPACE 5, 10, 25, 255
#define COLOR_SPACE_BG 10, 15, 30, 255
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
void draw_fallback_text(SDLView* view, const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !view->renderer) return;
    SDL_SetRenderDrawColor(view->renderer, r, g, b, 255);
    int cursor = x;
    for(int i=0; text[i]; i++) {
        SDL_FRect rect = {(float)cursor, (float)y, 8.0f, 12.0f};
        SDL_RenderFillRect(view->renderer, &rect);
        cursor += 10;
    }
}

/* --- Core View Functions --- */

SDLView* sdl_view_create(void) {
    SDLView* view = malloc(sizeof(SDLView));
    if(view) {
        memset(view, 0, sizeof(SDLView));
        view->last_frame_time = SDL_GetTicks();
        view->frame_count = 0;
        view->fps = 0;
        view->last_shots_fired = 0;
        view->last_score = 0;
        view->last_enemy_bullet_count = 0;
        view->last_player_lives = 3;  // Initialize with starting lives
        view->game_over_played = false;
        view->current_music_track = 0;
    }
    return view;
}

void sdl_view_destroy(SDLView* view) {
    if (!view) return;
    
    // 1. Cleanup Audio
    ma_sound_uninit(&view->sfx_shoot);
    ma_sound_uninit(&view->sfx_death);
    ma_sound_uninit(&view->sfx_enemy_bullet);
    ma_sound_uninit(&view->sfx_gameover);
    ma_sound_uninit(&view->sfx_damage);
    ma_sound_uninit(&view->music_game);
    ma_sound_uninit(&view->music_boss);
    ma_sound_uninit(&view->music_victory);
    ma_engine_uninit(&view->audio_engine);

    // 2. Cleanup Textures
    if(view->player_tex[0]) SDL_DestroyTexture(view->player_tex[0]);
    if(view->player_tex[1]) SDL_DestroyTexture(view->player_tex[1]);

    for(int j=0; j<2; j++) {
        if(view->boss_tex[j]) SDL_DestroyTexture(view->boss_tex[j]);
        if(view->saucer_tex[j]) SDL_DestroyTexture(view->saucer_tex[j]);
    }
    
    if(view->explosion_tex) SDL_DestroyTexture(view->explosion_tex);
    if(view->bullet_player_tex) SDL_DestroyTexture(view->bullet_player_tex);
    if(view->bullet_enemy_tex) SDL_DestroyTexture(view->bullet_enemy_tex);
    if(view->damage_tex) SDL_DestroyTexture(view->damage_tex);
    
    for(int i=0; i<3; i++) {
        for(int j=0; j<2; j++) {
            if(view->invader_tex[i][j]) SDL_DestroyTexture(view->invader_tex[i][j]);
        }
    }
    
    if(view->font_large) TTF_CloseFont(view->font_large);
    if(view->font_small) TTF_CloseFont(view->font_small);
    if(view->renderer) SDL_DestroyRenderer(view->renderer);
    if(view->window) SDL_DestroyWindow(view->window);
    
    TTF_Quit();
    SDL_Quit();
    free(view);
}

bool sdl_view_load_resources(SDLView* view) {
    // --- LOAD AUDIO (Miniaudio) ---
    // Using MA_SOUND_FLAG_DECODE for SFX to prevent latency
    if (ma_sound_init_from_file(&view->audio_engine, "assets/shooting_improved.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &view->sfx_shoot) != MA_SUCCESS) {
        printf("Failed to load assets/shooting_improved.wav\n");
    }
    
    if (ma_sound_init_from_file(&view->audio_engine, "assets/explosion.mp3", MA_SOUND_FLAG_DECODE, NULL, NULL, &view->sfx_death) != MA_SUCCESS) {
        printf("Failed to load assets/explosion.mp3\n");
    }
    
    if (ma_sound_init_from_file(&view->audio_engine, "assets/enemy_bullet.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &view->sfx_enemy_bullet) != MA_SUCCESS) {
        printf("Failed to load assets/enemy_bullet.wav\n");
    }
    
    if (ma_sound_init_from_file(&view->audio_engine, "assets/gameover.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &view->sfx_gameover) != MA_SUCCESS) {
        printf("Failed to load assets/gameover.wav\n");
    }
    
    if (ma_sound_init_from_file(&view->audio_engine, "assets/damage.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &view->sfx_damage) != MA_SUCCESS) {
        printf("Failed to load assets/damage.wav\n");
    }
    
    // Music - Game music (Stream from disk, Loop)
    if (ma_sound_init_from_file(&view->audio_engine, "assets/music_game.wav", MA_SOUND_FLAG_STREAM, NULL, NULL, &view->music_game) == MA_SUCCESS) {
        ma_sound_set_looping(&view->music_game, MA_TRUE);
        ma_sound_set_volume(&view->music_game, 0.5f); 
        ma_sound_start(&view->music_game);
        view->current_music_track = 1;  // Game music playing
    } else {
        printf("Failed to load assets/music_game.wav\n");
    }
    
    // Boss fight music (loop)
    if (ma_sound_init_from_file(&view->audio_engine, "assets/music_boss.wav", MA_SOUND_FLAG_STREAM, NULL, NULL, &view->music_boss) == MA_SUCCESS) {
        ma_sound_set_looping(&view->music_boss, MA_TRUE);
        ma_sound_set_volume(&view->music_boss, 0.6f);
    } else {
        printf("Failed to load assets/music_boss.wav\n");
    }
    
    // Victory music
    if (ma_sound_init_from_file(&view->audio_engine, "bin/assets/music_victory.wav", 0, NULL, NULL, &view->music_victory) != MA_SUCCESS) {
        fprintf(stderr, "Failed to load victory music\n");
    } else {
        ma_sound_set_volume(&view->music_victory, 0.7f);
        ma_sound_set_looping(&view->music_victory, MA_TRUE);  // FIXED: Added looping
    }

    // --- LOAD FONTS ---
    const char* font_paths[] = {
        "fonts/venite-adoremus-font/VeniteAdoremus-rgRBA.ttf",
        "assets/font.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        NULL
    };
    
    for(int i = 0; font_paths[i]; i++) {
        view->font_large = TTF_OpenFont(font_paths[i], 48);
        if(view->font_large) {
            view->font_small = TTF_OpenFont(font_paths[i], 18);
            printf("Loaded font: %s\n", font_paths[i]);
            break;
        }
    }

    // --- LOAD TEXTURES ---
    #define LOAD_TEXTURE(path, dest) { \
        SDL_Surface* s = IMG_Load(path); \
        if(s) { \
            dest = SDL_CreateTextureFromSurface(view->renderer, s); \
            SDL_DestroySurface(s); \
            if(dest) { \
                SDL_SetTextureScaleMode(dest, SDL_SCALEMODE_NEAREST); \
                SDL_SetTextureBlendMode(dest, SDL_BLENDMODE_BLEND); \
            } \
        } \
    }

    LOAD_TEXTURE("pictures/player.bmp", view->player_tex[0]);
    LOAD_TEXTURE("pictures/player2.bmp", view->player_tex[1]);
    LOAD_TEXTURE("pictures/explosion.bmp", view->explosion_tex);
    LOAD_TEXTURE("pictures/bullet_player.bmp", view->bullet_player_tex);
    LOAD_TEXTURE("pictures/bullet_enemy.bmp", view->bullet_enemy_tex);

    const char* boss_paths[2] = { "pictures/boss_dreadnought_f1.bmp", "pictures/boss_dreadnought_f2.bmp" };
    const char* saucer_paths[2] = { "pictures/bonus_saucer_f1.bmp", "pictures/bonus_saucer_f2.bmp" };
    for(int j=0; j<2; j++) {
        LOAD_TEXTURE(boss_paths[j], view->boss_tex[j]);
        LOAD_TEXTURE(saucer_paths[j], view->saucer_tex[j]);
    }
    
    const char* invader_paths[3][2] = {
        {"pictures/invader1_1.bmp", "pictures/invader1_2.bmp"},
        {"pictures/invader2_1.bmp", "pictures/invader2_2.bmp"},
        {"pictures/invader3_1.bmp", "pictures/invader3_2.bmp"}
    };
    for(int i=0; i<3; i++) {
        for(int j=0; j<2; j++) {
            LOAD_TEXTURE(invader_paths[i][j], view->invader_tex[i][j]);
        }
    }

    return true;
}

bool sdl_view_init(SDLView* view, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init Failed: %s\n", SDL_GetError());
        return false;
    }
    if (!TTF_Init()) return false;

    // --- INIT MINIAUDIO ---
    ma_result result = ma_engine_init(NULL, &view->audio_engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine. Game will play without sound.\n");
    }

    view->window = SDL_CreateWindow("Space Invaders Ultimate", width, height, 0);
    if (!view->window) return false;
    
    view->renderer = SDL_CreateRenderer(view->window, NULL);
    if (!view->renderer) return false;
    
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    view->width = width;
    view->height = height;
    
    sdl_view_load_resources(view);
    view->initialized = true;
    
    return true;
}

bool sdl_view_poll_event(SDLView* view, SDL_Event* event) {
    (void)view;
    return SDL_PollEvent(event);
}

// ... (Helper Functions) ...
void draw_text(SDLView* view, const char* text, int x, int y, SDL_Color col) {
    if(!view || !view->renderer) return;
    if(view->font_small) {
        SDL_Surface* s = TTF_RenderText_Blended(view->font_small, text, 0, col);
        if(s) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(view->renderer, s);
            SDL_FRect dst = {(float)x, (float)y, (float)s->w, (float)s->h};
            SDL_RenderTexture(view->renderer, t, NULL, &dst);
            SDL_DestroySurface(s);
            SDL_DestroyTexture(t);
        }
    } else {
        draw_fallback_text(view, text, x, y, col.r, col.g, col.b);
    }
}
void draw_text_centered(SDLView* view, const char* text, int y, SDL_Color col, bool large) {
    if(!view || !view->renderer) return;
    TTF_Font* font = large ? view->font_large : view->font_small;
    if(font) {
        SDL_Surface* s = TTF_RenderText_Blended(font, text, 0, col);
        if(s) {
            int x = (view->width - s->w) / 2;
            SDL_Texture* t = SDL_CreateTextureFromSurface(view->renderer, s);
            SDL_FRect dst = {(float)x, (float)y, (float)s->w, (float)s->h};
            SDL_RenderTexture(view->renderer, t, NULL, &dst);
            SDL_DestroySurface(s);
            SDL_DestroyTexture(t);
        }
    } else {
        draw_fallback_text(view, text, (view->width - (int)strlen(text)*10)/2, y, col.r, col.g, col.b);
    }
}
void draw_particle_effect(SDLView* view, float x, float y, float size, Uint32 color) {
    if(!view || !view->renderer) return;
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(view->renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 100);
    for(int i = 0; i < 4; i++) {
        float angle = (view->frame_count * 5 + i * 90) * 3.14159f / 180.0f;
        float px = x + cosf(angle) * size * 0.5f;
        float py = y + sinf(angle) * size * 0.5f;
        SDL_FRect particle = {px - 1, py - 1, 2.0f, 2.0f};
        SDL_RenderFillRect(view->renderer, &particle);
    }
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
}

void sdl_view_draw_hud(SDLView* view, const GameModel* model) {
    SDL_SetRenderDrawColor(view->renderer, 20, 25, 45, 220);
    SDL_FRect hud_bg = {600.0f, 0.0f, 200.0f, 600.0f};
    SDL_RenderFillRect(view->renderer, &hud_bg);
    SDL_SetRenderDrawColor(view->renderer, 60, 100, 150, 255);
    SDL_FRect border = {600.0f, 0.0f, 200.0f, 600.0f};
    SDL_RenderRect(view->renderer, &border);
    
    char buf[64];
    SDL_Color title_col = {COLOR_TEXT_HIGHLIGHT};
    SDL_Color val_col = {COLOR_TEXT_SECONDARY};
    
    snprintf(buf, 64, "SCORE"); draw_text(view, buf, 620, 50, title_col);
    snprintf(buf, 64, "%06d", model->player.score); draw_text(view, buf, 620, 80, val_col);
    snprintf(buf, 64, "LEVEL"); draw_text(view, buf, 620, 150, title_col);
    snprintf(buf, 64, "%d", model->player.level); draw_text(view, buf, 620, 180, val_col);
    snprintf(buf, 64, "LIVES"); draw_text(view, buf, 620, 250, title_col);
    
    for(int i = 0; i < model->player.lives && i < 5; i++) {
        SDL_FRect life = {(float)(620 + i * 32), 280.0f, 28.0f, 20.0f};
        if(view->player_tex[0]) SDL_RenderTexture(view->renderer, view->player_tex[0], NULL, &life);
        else { SDL_SetRenderDrawColor(view->renderer, COLOR_PLAYER); SDL_RenderFillRect(view->renderer, &life); }
    }
}

// Menu rendering helper functions
static void sdl_view_render_main_menu(SDLView* view, const GameModel* model) {
    // Background gradient
    for(int i=0; i<view->height; i++) {
        SDL_SetRenderDrawColor(view->renderer, 10, 15 + i/20, 30 + i/10, 255);
        SDL_FRect line = {0, (float)i, (float)view->width, 1};
        SDL_RenderFillRect(view->renderer, &line);
    }
    
    draw_text_centered(view, "SPACE INVADERS", 100, (SDL_Color){COLOR_TEXT_HIGHLIGHT}, true);
    draw_text_centered(view, "by Amine Boucif", 160, (SDL_Color){COLOR_TEXT_SECONDARY}, false);
    
    // Menu items
    const char* items[] = {"START", "SETTINGS", "QUIT"};
    int y_start = 250;
    int y_spacing = 60;
    
    for (int i = 0; i < 3; i++) {
        SDL_Color color = (model->menu_selection == i) ? 
            (SDL_Color){255, 255, 0, 255} : (SDL_Color){COLOR_TEXT_PRIMARY};
        
        const char* prefix = (model->menu_selection == i) ? "> " : "  ";
        char text[64];
        snprintf(text, sizeof(text), "%s%s", prefix, items[i]);
        draw_text_centered(view, text, y_start + i * y_spacing, color, false);
    }
    
    draw_text_centered(view, "Use UP/DOWN arrows to navigate, ENTER to select", 
                      view->height - 50, (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

static void sdl_view_render_difficulty_menu(SDLView* view, const GameModel* model) {
    // Background gradient  
    for(int i=0; i<view->height; i++) {
        SDL_SetRenderDrawColor(view->renderer, 10, 15 + i/20, 30 + i/10, 255);
        SDL_FRect line = {0, (float)i, (float)view->width, 1};
        SDL_RenderFillRect(view->renderer, &line);
    }
    
    draw_text_centered(view, "SELECT DIFFICULTY", 100, (SDL_Color){COLOR_TEXT_HIGHLIGHT}, true);
    
    // Difficulty items with descriptions
    const char* items[] = {"EASY   - 3 Levels Only", "NORMAL - 4 Levels with Boss", "HARD   - Faster & Harder"};
    int y_start = 250;
    int y_spacing = 70;
    
    for (int i = 0; i < 3; i++) {
        SDL_Color color = (model->menu_selection == i) ? 
            (SDL_Color){255, 255, 0, 255} : (SDL_Color){COLOR_TEXT_PRIMARY};
        
        const char* prefix = (model->menu_selection == i) ? "> " : "  ";
        char text[128];
        snprintf(text, sizeof(text), "%s%s", prefix, items[i]);
        draw_text_centered(view, text, y_start + i * y_spacing, color, false);
    }
    
    draw_text_centered(view, "ENTER to select, ESC to go back", 
                      view->height - 50, (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

static void sdl_view_render_settings_menu(SDLView* view, const GameModel* model) {
    // Background gradient
    for(int i=0; i<view->height; i++) {
        SDL_SetRenderDrawColor(view->renderer, 10, 15 + i/20, 30 + i/10, 255);
        SDL_FRect line = {0, (float)i, (float)view->width, 1};
        SDL_RenderFillRect(view->renderer, &line);
    }
    
    draw_text_centered(view, "SETTINGS", 100, (SDL_Color){COLOR_TEXT_HIGHLIGHT}, true);
    
    // Menu items
    int y_start = 230;
    int y_spacing = 70;
    
    // Controls
    SDL_Color color0 = (model->menu_selection == 0) ? 
        (SDL_Color){255, 255, 0, 255} : (SDL_Color){COLOR_TEXT_PRIMARY};
    draw_text_centered(view, model->menu_selection == 0 ? "> CONTROLS" : "  CONTROLS", 
                      y_start, color0, false);
    
    // Music Volume with slider
    SDL_Color color1 = (model->menu_selection == 1) ? 
        (SDL_Color){255, 255, 0, 255} : (SDL_Color){COLOR_TEXT_PRIMARY};
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
        SDL_FRect bar_fill = {(float)bar_x, (float)bar_y, bar_width * model->music_volume, 20};
        SDL_RenderFillRect(view->renderer, &bar_fill);
        
        draw_text_centered(view, "Use LEFT/RIGHT to adjust", 
                          bar_y + 40, (SDL_Color){200, 200, 200, 255}, false);
    }
    
    // Back
    SDL_Color color2 = (model->menu_selection == 2) ? 
        (SDL_Color){255, 255, 0, 255} : (SDL_Color){COLOR_TEXT_PRIMARY};
    draw_text_centered(view, model->menu_selection == 2 ? "> BACK" : "  BACK", 
                      y_start + 2 * y_spacing + 80, color2, false);
    
    draw_text_centered(view, "ENTER to select, ESC to go back", 
                      view->height - 50, (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

static void sdl_view_render_controls_menu(SDLView* view, const GameModel* model) {
    (void)model;
    
    // Background gradient
    for(int i=0; i<view->height; i++) {
        SDL_SetRenderDrawColor(view->renderer, 10, 15 + i/20, 30 + i/10, 255);
        SDL_FRect line = {0, (float)i, (float)view->width, 1};
        SDL_RenderFillRect(view->renderer, &line);
    }
    
    draw_text_centered(view, "CONTROLS", 80, (SDL_Color){COLOR_TEXT_HIGHLIGHT}, true);
    
    // Controls box
    int box_x = (view->width - 500)/2;
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 30, 30, 30, 220);
    SDL_FRect bg = {(float)box_x, 150.0f, 500.0f, 350.0f};
    SDL_RenderFillRect(view->renderer, &bg);
    SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
    SDL_RenderRect(view->renderer, &bg);
    
    SDL_Color yel = {255, 255, 100, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    draw_text(view, "MOVEMENT:", box_x + 30, 180, yel);
    draw_text(view, "W / UP ARROW    - Move Up (menus)", box_x + 50, 220, white);
    draw_text(view, "A / LEFT ARROW  - Move Left", box_x + 50, 250, white);
    draw_text(view, "S / DOWN ARROW  - Move Down (menus)", box_x + 50, 280, white);
    draw_text(view, "D / RIGHT ARROW - Move Right", box_x + 50, 310, white);
    
    draw_text(view, "ACTIONS:", box_x + 30, 360, yel);
    draw_text(view, "SPACE - Shoot", box_x + 50, 400, white);
    draw_text(view, "ENTER - Select (menus)", box_x + 50, 430, white);
    draw_text(view, "P     - Pause", box_x + 50, 460, white);
    
    draw_text_centered(view, "Press ESC or ENTER to return", 
                      view->height - 50, (SDL_Color){COLOR_TEXT_SECONDARY}, false);
}

void sdl_view_render_game_scene(SDLView* view, const GameModel* model) {
    if(!view || !view->renderer || !model) return;
    
    // Background
    SDL_SetRenderDrawColor(view->renderer, COLOR_SPACE_BG);
    SDL_FRect space_bg = {0.0f, 0.0f, 600.0f, 600.0f};
    SDL_RenderFillRect(view->renderer, &space_bg);
    
    SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 100);
    for(int i = 0; i < 50; i++) {
        int seed = (i * 137) % 1000;
        SDL_FRect star = {(float)((seed * 13) % 600), (float)((seed * 17) % 600), 2.0f, 2.0f};
        SDL_RenderFillRect(view->renderer, &star);
    }
    
    // Player
    float p_scale = 2.0f;
    float p_w = (float)model->player.hitbox.width * p_scale;
    float p_h = (float)model->player.hitbox.height * p_scale;
    float p_x = (float)model->player.hitbox.x - (p_w - (float)model->player.hitbox.width) / 2.0f;
    float p_y = (float)model->player.hitbox.y - (p_h - (float)model->player.hitbox.height) / 2.0f;
    SDL_FRect p_dst = {p_x, p_y, p_w, p_h};
    int p_anim_idx = (view->frame_count / 15) % 2; 
    if(view->player_tex[p_anim_idx]) SDL_RenderTexture(view->renderer, view->player_tex[p_anim_idx], NULL, &p_dst);
    else if (view->player_tex[0]) SDL_RenderTexture(view->renderer, view->player_tex[0], NULL, &p_dst);
    else { SDL_SetRenderDrawColor(view->renderer, COLOR_PLAYER); SDL_RenderFillRect(view->renderer, &p_dst); }
    
    // Bullets
    for(int i=0; i<PLAYER_BULLETS; i++) if(model->player_bullets[i].alive) {
        SDL_FRect b = {(float)model->player_bullets[i].hitbox.x, (float)model->player_bullets[i].hitbox.y, (float)model->player_bullets[i].hitbox.width, (float)model->player_bullets[i].hitbox.height};
        if(view->bullet_player_tex) SDL_RenderTexture(view->renderer, view->bullet_player_tex, NULL, &b);
        else { SDL_SetRenderDrawColor(view->renderer, COLOR_BULLET_PLAYER); SDL_RenderFillRect(view->renderer, &b); }
    }
    for(int i=0; i<ENEMY_BULLETS; i++) if(model->enemy_bullets[i].alive) {
        SDL_FRect b = {(float)model->enemy_bullets[i].hitbox.x, (float)model->enemy_bullets[i].hitbox.y, (float)model->enemy_bullets[i].hitbox.width, (float)model->enemy_bullets[i].hitbox.height};
        if(view->bullet_enemy_tex) SDL_RenderTexture(view->renderer, view->bullet_enemy_tex, NULL, &b);
        else { SDL_SetRenderDrawColor(view->renderer, COLOR_BULLET_ENEMY); SDL_RenderFillRect(view->renderer, &b); }
    }

    // Saucer
    if(model->saucer.alive) {
        SDL_FRect s_dst = {(float)model->saucer.hitbox.x, (float)model->saucer.hitbox.y, (float)model->saucer.hitbox.width, (float)model->saucer.hitbox.height};
        int anim_state = model->invaders.state;
        if(view->saucer_tex[anim_state]) SDL_RenderTexture(view->renderer, view->saucer_tex[anim_state], NULL, &s_dst);
        else { SDL_SetRenderDrawColor(view->renderer, 255, 150, 0, 255); SDL_RenderFillRect(view->renderer, &s_dst); }
    }

    // Invaders / Boss
    if(model->player.level == 4 && model->boss.alive) {
        SDL_FRect boss = {(float)model->boss.hitbox.x, (float)model->boss.hitbox.y, (float)model->boss.hitbox.width, (float)model->boss.hitbox.height};
        int anim_state = model->boss.anim_frame;  // Use boss's own animation frame
        if(view->boss_tex[anim_state]) SDL_RenderTexture(view->renderer, view->boss_tex[anim_state], NULL, &boss);
        else { SDL_SetRenderDrawColor(view->renderer, COLOR_ENEMY); SDL_RenderFillRect(view->renderer, &boss); }
        float pct = (float)model->boss.health / model->boss.max_health;
        SDL_FRect bg = {150.0f, 10.0f, 300.0f, 20.0f};
        SDL_SetRenderDrawColor(view->renderer, 50, 0, 0, 255); SDL_RenderFillRect(view->renderer, &bg);
        SDL_FRect fg = {150.0f, 10.0f, 300.0f * (pct > 0 ? pct : 0), 20.0f};
        SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255); SDL_RenderFillRect(view->renderer, &fg);
        draw_text(view, "BOSS", 150, 35, (SDL_Color){255, 100, 100, 255});
    } else {
        for(int i = 0; i < INVADER_ROWS; i++) {
            for(int j = 0; j < INVADER_COLS; j++) {
                const Invader* inv = &model->invaders.invaders[i][j];
                if(inv->alive) {
                    SDL_FRect idst = {(float)inv->hitbox.x, (float)inv->hitbox.y, (float)inv->hitbox.width, (float)inv->hitbox.height};
                    if(inv->dying_timer > 0) {
                        if(view->explosion_tex) SDL_RenderTexture(view->renderer, view->explosion_tex, NULL, &idst);
                        else { SDL_SetRenderDrawColor(view->renderer, COLOR_EXPLOSION); SDL_RenderFillRect(view->renderer, &idst); }
                        draw_particle_effect(view, idst.x+15, idst.y+15, 10, 0xFFAA00);
                    } else {
                        if(view->invader_tex[inv->type][model->invaders.state])
                            SDL_RenderTexture(view->renderer, view->invader_tex[inv->type][model->invaders.state], NULL, &idst);
                        else { SDL_SetRenderDrawColor(view->renderer, 0, 255, 0, 255); SDL_RenderFillRect(view->renderer, &idst); }
                    }
                }
            }
        }
    }
    sdl_view_draw_hud(view, model);
}

void sdl_view_render(SDLView* view, const GameModel* model) {
    if(!view || !view->renderer || !model) return;
    
    // Apply music volume from settings
    ma_sound_set_volume(&view->music_game, model->music_volume * 0.5f);
    ma_sound_set_volume(&view->music_boss, model->music_volume * 0.6f);
    ma_sound_set_volume(&view->music_victory, model->music_volume * 0.7f);
    
    // Reset audio tracking when starting new game from menu
    static GameState last_state = STATE_MENU;
    if (last_state == STATE_MENU && model->state == STATE_PLAYING) {
        view->last_shots_fired = 0;
        view->last_score = 0;
        view->last_enemy_bullet_count = 0;
        view->last_player_lives = model->player.lives;
    }
    last_state = model->state;
    
    // --- AUDIO LOGIC ---
    // Only play sounds during active gameplay
    if (model->state == STATE_PLAYING) {
        // 1. Detect Shot (Rewind and Play)
        if (model->player.shots_fired > view->last_shots_fired) {
            if(ma_sound_is_playing(&view->sfx_shoot)) {
                ma_sound_seek_to_pcm_frame(&view->sfx_shoot, 0);
            } else {
                ma_sound_start(&view->sfx_shoot);
            }
            view->last_shots_fired = model->player.shots_fired;
        }
        
        // 2. Detect Enemy Death (Score change)
        if (model->player.score > view->last_score) {
            if(ma_sound_is_playing(&view->sfx_death)) {
                ma_sound_seek_to_pcm_frame(&view->sfx_death, 0);
            } else {
                ma_sound_start(&view->sfx_death);
            }
            view->last_score = model->player.score;
        }
    }
    
    // 3. Detect Enemy Bullets (count active enemy bullets)
    int current_enemy_bullets = 0;
    for(int i = 0; i < ENEMY_BULLETS; i++) {
        if(model->enemy_bullets[i].alive) current_enemy_bullets++;
    }
    if (current_enemy_bullets > view->last_enemy_bullet_count) {
        if(ma_sound_is_playing(&view->sfx_enemy_bullet)) {
            ma_sound_seek_to_pcm_frame(&view->sfx_enemy_bullet, 0);
        } else {
            ma_sound_start(&view->sfx_enemy_bullet);
        }
    }
    view->last_enemy_bullet_count = current_enemy_bullets;
    
    // 4. Detect Player Damage (life decrease)
    if (model->player.lives < view->last_player_lives) {
        if(ma_sound_is_playing(&view->sfx_damage)) {
            ma_sound_seek_to_pcm_frame(&view->sfx_damage, 0);
        } else {
            ma_sound_start(&view->sfx_damage);
        }
        view->last_player_lives = model->player.lives;
    }
    // Update lives if they increased (shouldn't happen, but for safety)
    if (model->player.lives > view->last_player_lives) {
        view->last_player_lives = model->player.lives;
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
        ma_sound_start(&view->music_game);
        view->current_music_track = 1;
    }
    // Switch to boss music on level 4
    else if (model->state == STATE_PLAYING && model->player.level == 4 && model->boss.alive && view->current_music_track != 2) {
        ma_sound_stop(&view->music_game);
        ma_sound_start(&view->music_boss);
        view->current_music_track = 2;
    }
    // Switch back to game music when leaving level 4 or boss defeated
    else if (model->state == STATE_PLAYING && (model->player.level != 4 || !model->boss.alive) && view->current_music_track == 2) {
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
    // Resume game music when entering STATE_PLAYING from menu
    else if (model->state == STATE_PLAYING && view->current_music_track == 0) {
        ma_sound_start(&view->music_game);
        view->current_music_track = 1;
        view->last_player_lives = model->player.lives;  // Sync lives on game start
    }
    // Resume menu music when returning from victory or game over
    else if (model->state == STATE_MENU && (view->current_music_track == 3 || view->current_music_track != 1)) {
        ma_sound_stop(&view->music_victory);
        ma_sound_stop(&view->music_boss);
        if (!ma_sound_is_playing(&view->music_game)) {
            ma_sound_start(&view->music_game);
        }
        view->current_music_track = 1;
    }

    // --- RENDER LOGIC ---
    SDL_SetRenderDrawColor(view->renderer, COLOR_DARK_SPACE);
    SDL_RenderClear(view->renderer);
    
    // Use {} for every case to prevent redeclaration errors
    switch(model->state) {
        case STATE_MENU: {
            //  Render the appropriate menu based on menu_state
            switch(model->menu_state) {
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
            char buf[64]; snprintf(buf, 64, "LEVEL %d", model->player.level);
            draw_text_centered(view, buf, 280, (SDL_Color){0, 255, 0, 255}, true);
            draw_text_centered(view, "PRESS SPACE", 350, (SDL_Color){255, 255, 255, 255}, false);
            break;
        }
        case STATE_WIN: {
            sdl_view_render_game_scene(view, model);
            draw_text_centered(view, "MISSION ACCOMPLISHED!", 250, (SDL_Color){0, 255, 0, 255}, true);
            char buf[64]; snprintf(buf, 64, "Final Score: %d", model->player.score);
            draw_text_centered(view, buf, 320, (SDL_Color){255, 255, 255, 255}, false);
            break;
        }
        case STATE_GAME_OVER: {
            sdl_view_render_game_scene(view, model);
            SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(view->renderer, 50, 0, 0, 150);
            SDL_FRect overlay2 = {0, 0, (float)view->width, (float)view->height};
            SDL_RenderFillRect(view->renderer, &overlay2);
            draw_text_centered(view, "GAME OVER", 280, (SDL_Color){255, 0, 0, 255}, true);
            draw_text_centered(view, "Press R to Restart", 350, (SDL_Color){255, 255, 255, 255}, false);
            break;
        }
        case STATE_PAUSED: {
            sdl_view_render_game_scene(view, model);
            SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 150);
            SDL_FRect overlay3 = {0, 0, (float)view->width, (float)view->height};
            SDL_RenderFillRect(view->renderer, &overlay3);
            draw_text_centered(view, "PAUSED", 300, (SDL_Color){255, 255, 255, 255}, true);
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