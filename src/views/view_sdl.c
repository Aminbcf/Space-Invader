#include "view_sdl.h"
#include "rect_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Fallback Font --- */
void draw_fallback_text(SDLView* view, const char* text, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !view->renderer) return;
    SDL_SetRenderDrawColor(view->renderer, r, g, b, 255);
    int cursor = x;
    for(int i=0; text[i]; i++) {
        SDL_FRect rect = {(float)cursor, (float)y, 10.0f, 14.0f};
        SDL_RenderFillRect(view->renderer, &rect);
        cursor += 12;
    }
}

/* --- Core View Functions --- */

SDLView* sdl_view_create(void) {
    SDLView* view = malloc(sizeof(SDLView));
    if(view) memset(view, 0, sizeof(SDLView));
    return view;
}

void sdl_view_destroy(SDLView* view) {
    if (!view) return;
    if(view->player_tex) SDL_DestroyTexture(view->player_tex);
    if(view->boss_tex) SDL_DestroyTexture(view->boss_tex);
    if(view->explosion_tex) SDL_DestroyTexture(view->explosion_tex);
    if(view->bullet_player_tex) SDL_DestroyTexture(view->bullet_player_tex);
    if(view->bullet_enemy_tex) SDL_DestroyTexture(view->bullet_enemy_tex);
    if(view->saucer_tex) SDL_DestroyTexture(view->saucer_tex);
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
    const char* font_path = "fonts/venite-adoremus-font/VeniteAdoremus-rgRBA.ttf";
    view->font_large = TTF_OpenFont(font_path, 48);
    view->font_small = TTF_OpenFont(font_path, 24);
    
    if(!view->font_large) printf("Warning: Font not found at %s. Using fallback.\n", font_path);

    #define LOAD(path, dest) { \
        SDL_Surface* s = IMG_Load(path); \
        if(s) { dest = SDL_CreateTextureFromSurface(view->renderer, s); SDL_DestroySurface(s); } \
        else printf("Failed to load %s\n", path); \
    }

    LOAD("src/pictures/player.bmp", view->player_tex);
    LOAD("src/pictures/boss.bmp", view->boss_tex);
    LOAD("src/pictures/explosion.bmp", view->explosion_tex);
    LOAD("src/pictures/bullet_player.bmp", view->bullet_player_tex);
    LOAD("src/pictures/bullet_enemy.bmp", view->bullet_enemy_tex);
    LOAD("src/pictures/saucer.bmp", view->saucer_tex);
    LOAD("Oldassets/damage.bmp", view->damage_tex);
    
    LOAD("src/pictures/invader1_1.bmp", view->invader_tex[0][0]);
    LOAD("src/pictures/invader1_2.bmp", view->invader_tex[0][1]);
    LOAD("src/pictures/invader2_1.bmp", view->invader_tex[1][0]);
    LOAD("src/pictures/invader2_2.bmp", view->invader_tex[1][1]);
    LOAD("src/pictures/invader3_1.bmp", view->invader_tex[2][0]);
    LOAD("src/pictures/invader3_2.bmp", view->invader_tex[2][1]);

    return true;
}

bool sdl_view_init(SDLView* view, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() < 0) return false;

    view->window = SDL_CreateWindow("Space Invaders", width, height, 0);
    view->renderer = SDL_CreateRenderer(view->window, NULL);
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

void draw_text(SDLView* view, const char* text, int x, int y, SDL_Color col) {
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
        draw_fallback_text(view, text, (view->width - (int)strlen(text)*12)/2, y, col.r, col.g, col.b);
    }
}

void sdl_view_draw_hud(SDLView* view, const GameModel* model) {
    SDL_SetRenderDrawColor(view->renderer, 20, 20, 20, 255);
    SDL_FRect hud_bg = {600.0f, 0.0f, 200.0f, 600.0f};
    SDL_RenderFillRect(view->renderer, &hud_bg);
    
    char buf[64];
    SDL_Color white = {255,255,255,255};
    
    snprintf(buf, 64, "SCORE"); draw_text(view, buf, 620, 50, white);
    snprintf(buf, 64, "%06d", model->player.score); draw_text(view, buf, 620, 80, white);
    
    snprintf(buf, 64, "LEVEL"); draw_text(view, buf, 620, 150, white);
    snprintf(buf, 64, "%d", model->player.level); draw_text(view, buf, 620, 180, white);
    
    snprintf(buf, 64, "LIVES"); draw_text(view, buf, 620, 250, white);
    for(int i=0; i<model->player.lives; i++) {
        SDL_FRect life = { (float)(620 + i*35), 280.0f, 30.0f, 20.0f };
        if(view->player_tex) SDL_RenderTexture(view->renderer, view->player_tex, NULL, &life);
        else { SDL_SetRenderDrawColor(view->renderer, 0, 255, 255, 255); SDL_RenderFillRect(view->renderer, &life); }
    }
}

void sdl_view_render_game_scene(SDLView* view, const GameModel* model) {
    // Draw Player
    SDL_FRect p_dst = {(float)model->player.hitbox.x, (float)model->player.hitbox.y, (float)model->player.hitbox.width, (float)model->player.hitbox.height};
    if(view->player_tex) SDL_RenderTexture(view->renderer, view->player_tex, NULL, &p_dst);
    
    // Draw Bullets
    for(int i=0; i<PLAYER_BULLETS; i++) if(model->player_bullets[i].alive) {
        SDL_FRect b = {(float)model->player_bullets[i].hitbox.x, (float)model->player_bullets[i].hitbox.y, (float)model->player_bullets[i].hitbox.width, (float)model->player_bullets[i].hitbox.height};
        if(view->bullet_player_tex) SDL_RenderTexture(view->renderer, view->bullet_player_tex, NULL, &b);
    }
    for(int i=0; i<ENEMY_BULLETS; i++) if(model->enemy_bullets[i].alive) {
        SDL_FRect b = {(float)model->enemy_bullets[i].hitbox.x, (float)model->enemy_bullets[i].hitbox.y, (float)model->enemy_bullets[i].hitbox.width, (float)model->enemy_bullets[i].hitbox.height};
        if(view->bullet_enemy_tex) SDL_RenderTexture(view->renderer, view->bullet_enemy_tex, NULL, &b);
    }

    // Draw Saucer
    if (model->saucer.alive) {
        SDL_FRect s_dst = {(float)model->saucer.hitbox.x, (float)model->saucer.hitbox.y, (float)model->saucer.hitbox.width, (float)model->saucer.hitbox.height};
        if(view->saucer_tex) SDL_RenderTexture(view->renderer, view->saucer_tex, NULL, &s_dst);
    }

    // Draw Boss (Level 4)
    if (model->player.level == 4 && model->boss.alive) {
        SDL_FRect boss = {(float)model->boss.hitbox.x, (float)model->boss.hitbox.y, (float)model->boss.hitbox.width, (float)model->boss.hitbox.height};
        if(view->boss_tex) SDL_RenderTexture(view->renderer, view->boss_tex, NULL, &boss);
        else { SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255); SDL_RenderFillRect(view->renderer, &boss); }
        
        // Health Bar
        SDL_FRect bg = {150.0f, 10.0f, 300.0f, 20.0f};
        SDL_SetRenderDrawColor(view->renderer, 50, 0, 0, 255); SDL_RenderFillRect(view->renderer, &bg);
        float pct = (float)model->boss.health / model->boss.max_health;
        if (pct < 0) pct = 0;
        SDL_FRect fg = {150.0f, 10.0f, 300.0f * pct, 20.0f};
        SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255); SDL_RenderFillRect(view->renderer, &fg);
        draw_text(view, "BOSS", 150, 35, (SDL_Color){255,0,0,255});
    }
    else {
        // Draw Invaders
        for(int i=0; i<INVADER_ROWS; i++) {
            for(int j=0; j<INVADER_COLS; j++) {
                const Invader* inv = &model->invaders.invaders[i][j];
                if(inv->alive) {
                    SDL_FRect idst = {(float)inv->hitbox.x, (float)inv->hitbox.y, (float)inv->hitbox.width, (float)inv->hitbox.height};
                    if(inv->dying_timer > 0) {
                            if(view->explosion_tex) SDL_RenderTexture(view->renderer, view->explosion_tex, NULL, &idst);
                            else { SDL_SetRenderDrawColor(view->renderer, 255, 128, 0, 255); SDL_RenderFillRect(view->renderer, &idst); }
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
    if (!view || !view->renderer) return;

    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
    SDL_RenderClear(view->renderer);

    if (model->state == STATE_MENU) {
        // Updated Menu Design
        draw_text_centered(view, "SPACE INVADERS", 150, (SDL_Color){0, 255, 0, 255}, true);
        draw_text_centered(view, "by Amine Boucif", 210, (SDL_Color){0, 255, 255, 255}, false);
        
        draw_text_centered(view, "Press SPACE to Start", 300, (SDL_Color){255, 255, 255, 255}, false);
        
        // Controls Box
        int box_w = 400;
        int box_h = 200;
        int box_x = (view->width - box_w) / 2;
        int box_y = 350;
        
        // Semi-transparent background
        SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(view->renderer, 30, 30, 30, 200);
        SDL_FRect bg = {(float)box_x, (float)box_y, (float)box_w, (float)box_h};
        SDL_RenderFillRect(view->renderer, &bg);
        
        // Border
        SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
        SDL_RenderRect(view->renderer, &bg);
        
        // Controls Text
        SDL_Color yellow = {255, 255, 0, 255};
        draw_text(view, "CONTROLS:", box_x + 20, box_y + 20, yellow);
        draw_text(view, "WASD / ARROWS : Move", box_x + 40, box_y + 60, yellow);
        draw_text(view, "SPACE : Shoot", box_x + 40, box_y + 90, yellow);
        draw_text(view, "P : Pause", box_x + 40, box_y + 120, yellow);
        draw_text(view, "R : Reset (Game Over)", box_x + 40, box_y + 150, yellow);
    }
    else if (model->state == STATE_WIN) {
        draw_text_centered(view, "YOU DEFEATED THE INVADERS!", 250, (SDL_Color){0, 255, 0, 255}, true);
        char buf[64]; snprintf(buf, 64, "Final Score: %d", model->player.score);
        draw_text_centered(view, buf, 320, (SDL_Color){255, 255, 255, 255}, false);
    }
    else if (model->state == STATE_LEVEL_TRANSITION) {
        // SMOOTHER TRANSITION: Draw the game frozen, then overlay text
        sdl_view_render_game_scene(view, model);
        
        SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 150);
        SDL_FRect overlay = {0, 0, (float)view->width, (float)view->height};
        SDL_RenderFillRect(view->renderer, &overlay);
        
        char buf[32]; snprintf(buf, 32, "LEVEL %d", model->player.level);
        draw_text_centered(view, buf, 280, (SDL_Color){0, 255, 0, 255}, true);
        draw_text_centered(view, "PRESS SPACE", 350, (SDL_Color){255, 255, 255, 255}, false);
    }
    else {
        // Standard Playing / Paused / Game Over
        sdl_view_render_game_scene(view, model);
        
        if (model->state == STATE_GAME_OVER) {
             draw_text_centered(view, "GAME OVER", 280, (SDL_Color){255, 0, 0, 255}, true);
             draw_text_centered(view, "Press R to Restart", 350, (SDL_Color){255, 255, 255, 255}, false);
        }
        else if (model->state == STATE_PAUSED) {
             draw_text_centered(view, "PAUSED", 300, (SDL_Color){255, 255, 255, 255}, true);
        }
    }

    SDL_RenderPresent(view->renderer);
}