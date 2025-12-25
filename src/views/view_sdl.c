#include "view_sdl.h"
#include "rect_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>


/* Simple ASCII bitmap font - 5x7 pixels per character */
static const unsigned char font_data[95][7] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* Space */
    {0x04,0x04,0x04,0x04,0x00,0x04,0x00}, /* ! */
    {0x0A,0x0A,0x0A,0x00,0x00,0x00,0x00}, /* " */
    {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00}, /* # */
    {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04}, /* $ */
    {0x18,0x19,0x02,0x04,0x08,0x13,0x03}, /* % */
    {0x0C,0x12,0x14,0x08,0x15,0x12,0x0D}, /* & */
    {0x0C,0x04,0x08,0x00,0x00,0x00,0x00}, /* ' */
    {0x02,0x04,0x08,0x08,0x08,0x04,0x02}, /* ( */
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08}, /* ) */
    {0x00,0x04,0x15,0x0E,0x15,0x04,0x00}, /* * */
    {0x00,0x04,0x04,0x1F,0x04,0x04,0x00}, /* + */
    {0x00,0x00,0x00,0x00,0x0C,0x04,0x08}, /* , */
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}, /* - */
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}, /* . */
    {0x00,0x01,0x02,0x04,0x08,0x10,0x00}, /* / */
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, /* 0 */
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, /* 1 */
    {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}, /* 2 */
    {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}, /* 3 */
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, /* 4 */
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, /* 5 */
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, /* 6 */
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, /* 7 */
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, /* 8 */
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}, /* 9 */
    {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00}, /* : */
    {0x00,0x0C,0x0C,0x00,0x0C,0x04,0x08}, /* ; */
    {0x02,0x04,0x08,0x10,0x08,0x04,0x02}, /* < */
    {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00}, /* = */
    {0x08,0x04,0x02,0x01,0x02,0x04,0x08}, /* > */
    {0x0E,0x11,0x01,0x02,0x04,0x00,0x04}, /* ? */
    {0x0E,0x11,0x01,0x0D,0x15,0x15,0x0E}, /* @ */
    {0x0E,0x11,0x11,0x11,0x1F,0x11,0x11}, /* A */
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, /* B */
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, /* C */
    {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}, /* D */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, /* E */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, /* F */
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}, /* G */
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, /* H */
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, /* I */
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}, /* J */
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, /* K */
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, /* L */
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, /* M */
    {0x11,0x11,0x19,0x15,0x13,0x11,0x11}, /* N */
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, /* O */
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, /* P */
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, /* Q */
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, /* R */
    {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}, /* S */
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, /* T */
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, /* U */
    {0x11,0x11,0x11,0x11,0x11,0x0A,0x04}, /* V */
    {0x11,0x11,0x11,0x15,0x15,0x15,0x0A}, /* W */
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, /* X */
    {0x11,0x11,0x11,0x0A,0x04,0x04,0x04}, /* Y */
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, /* Z */
};

/* Improved text rendering using bitmap font */
static void draw_string_simple(SDLView* view, const char* text, int x, int y, 
                               uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !text || !view->renderer) return;
    
    SDL_SetRenderDrawColor(view->renderer, r, g, b, 255);
    
    int char_width = 6;  /* 5 pixels + 1 spacing */
    int char_height = 7;
    int scale = 2;  /* 2x scaling for better visibility */
    
    for (int i = 0; text[i] != '\0'; i++) {
        int char_index = text[i] - 32;  /* ASCII offset */
        
        /* Skip if out of range */
        if (char_index < 0 || char_index >= 95) {
            continue;
        }
        
        /* Draw each pixel of the character */
        for (int row = 0; row < char_height; row++) {
            unsigned char line = font_data[char_index][row];
            for (int col = 0; col < 5; col++) {
                if (line & (1 << (4 - col))) {
                    /* Draw scaled pixel */
                    SDL_Rect pixel = {
                        x + i * char_width * scale + col * scale,
                        y + row * scale,
                        scale,
                        scale
                    };
                    SDL_RenderFillRect(view->renderer, &pixel);
                }
            }
        }
    }
}

SDLView* sdl_view_create(void) {
    SDLView* view = malloc(sizeof(SDLView));
    if (!view) return NULL;
    
    memset(view, 0, sizeof(SDLView));
    
    view->width = SCREEN_WIDTH;
    view->height = SCREEN_HEIGHT;
    view->title = "Space Invaders MVC";
    view->fullscreen = false;
    
    /* Initialize all pointers to NULL */
    view->window = NULL;
    view->renderer = NULL;
    view->screen_surface = NULL;
    view->title_screen = NULL;
    view->cmap = NULL;
    view->invadersmap = NULL;
    view->player_img = NULL;
    view->saucer_img = NULL;
    for (int i = 0; i < 4; i++) view->base_img[i] = NULL;
    view->damage_img = NULL;
    view->damage_top_img = NULL;
    view->game_over_img = NULL;
    
    view->screen_texture = NULL;
    view->title_texture = NULL;
    view->font_texture = NULL;
    view->invaders_texture = NULL;
    view->player_texture = NULL;
    view->saucer_texture = NULL;
    for (int i = 0; i < 4; i++) view->base_textures[i] = NULL;
    view->damage_texture = NULL;
    view->damage_top_texture = NULL;
    view->game_over_texture = NULL;
    
    view->initialized = false;
    view->last_frame_time = 0;
    view->frame_count = 0;
    view->frame_timer = 0;
    view->fps = 0.0f;
    
    return view;
}

void sdl_view_destroy(SDLView* view) {
    if (!view) return;
    
    /* Free textures */
    if (view->screen_texture) SDL_DestroyTexture(view->screen_texture);
    if (view->title_texture) SDL_DestroyTexture(view->title_texture);
    if (view->font_texture) SDL_DestroyTexture(view->font_texture);
    if (view->invaders_texture) SDL_DestroyTexture(view->invaders_texture);
    if (view->player_texture) SDL_DestroyTexture(view->player_texture);
    if (view->saucer_texture) SDL_DestroyTexture(view->saucer_texture);
    for (int i = 0; i < 4; i++) {
        if (view->base_textures[i]) SDL_DestroyTexture(view->base_textures[i]);
    }
    if (view->damage_texture) SDL_DestroyTexture(view->damage_texture);
    if (view->damage_top_texture) SDL_DestroyTexture(view->damage_top_texture);
    if (view->game_over_texture) SDL_DestroyTexture(view->game_over_texture);
    
    /* Free surfaces */
    if (view->screen_surface) SDL_FreeSurface(view->screen_surface);
    if (view->title_screen) SDL_FreeSurface(view->title_screen);
    if (view->cmap) SDL_FreeSurface(view->cmap);
    if (view->invadersmap) SDL_FreeSurface(view->invadersmap);
    if (view->player_img) SDL_FreeSurface(view->player_img);
    if (view->saucer_img) SDL_FreeSurface(view->saucer_img);
    for (int i = 0; i < 4; i++) {
        if (view->base_img[i]) SDL_FreeSurface(view->base_img[i]);
    }
    if (view->damage_img) SDL_FreeSurface(view->damage_img);
    if (view->damage_top_img) SDL_FreeSurface(view->damage_top_img);
    if (view->game_over_img) SDL_FreeSurface(view->game_over_img);
    
    /* Free SDL objects */
    if (view->renderer) SDL_DestroyRenderer(view->renderer);
    if (view->window) SDL_DestroyWindow(view->window);
    
    free(view);
}

bool sdl_view_init(SDLView* view, int width, int height) {
    if (!view) return false;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }
    
    view->window = SDL_CreateWindow(view->title,
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   width, height,
                                   SDL_WINDOW_SHOWN);
    if (!view->window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    
    view->renderer = SDL_CreateRenderer(view->window, -1, 
                                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!view->renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(view->window);
        SDL_Quit();
        return false;
    }
    
    view->screen_surface = SDL_CreateRGBSurface(0, width, height, 32,
                                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!view->screen_surface) {
        fprintf(stderr, "SDL_CreateRGBSurface Error: %s\n", SDL_GetError());
        sdl_view_destroy(view);
        return false;
    }
    
    view->screen_texture = SDL_CreateTexture(view->renderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            width, height);
    if (!view->screen_texture) {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        sdl_view_destroy(view);
        return false;
    }
    
    view->width = width;
    view->height = height;
    
    /* Load resources (optional - game can work without them) */
    sdl_view_load_resources(view);
    
    view->last_frame_time = SDL_GetTicks();
    view->frame_count = 0;
    view->frame_timer = view->last_frame_time;
    view->fps = 0.0f;
    
    view->initialized = true;
    return true;
}

bool sdl_view_load_image(SDLView* view, const char* filename, SDL_Surface** surface, 
                        uint8_t r, uint8_t g, uint8_t b) {
    SDL_Surface* temp = SDL_LoadBMP(filename);
    if (temp == NULL) {
        fprintf(stderr, "Warning: Unable to load %s: %s\n", filename, SDL_GetError());
        return false;
    }

    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, r, g, b));
    
    *surface = SDL_ConvertSurface(temp, view->screen_surface->format, 0);
    if (*surface == NULL) {
        fprintf(stderr, "Warning: Unable to convert bitmap: %s\n", SDL_GetError());
        SDL_FreeSurface(temp);
        return false;
    }
    
    SDL_FreeSurface(temp);
    return true;
}

bool sdl_view_convert_surface_to_texture(SDLView* view, SDL_Surface* surface, 
                                        SDL_Texture** texture) {
    if (!surface) return false;
    
    *texture = SDL_CreateTextureFromSurface(view->renderer, surface);
    if (!*texture) {
        fprintf(stderr, "Warning: Failed to create texture from surface: %s\n", SDL_GetError());
        return false;
    }
    
    return true;
}

bool sdl_view_load_resources(SDLView* view) {
    bool all_loaded = true;
    
    /* Try to load images (non-critical) */
    if (sdl_view_load_image(view, "assets/titlescreen.bmp", &view->title_screen, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->title_screen, &view->title_texture);
    } else {
        all_loaded = false;
    }
    
    /* Font map is optional now - we use simple rendering */
    if (sdl_view_load_image(view, "assets/cmap.bmp", &view->cmap, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->cmap, &view->font_texture);
    } else {
        fprintf(stderr, "Note: Using fallback text rendering\n");
    }
    
    if (sdl_view_load_image(view, "assets/invaders.bmp", &view->invadersmap, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->invadersmap, &view->invaders_texture);
    } else {
        all_loaded = false;
    }
    
    if (sdl_view_load_image(view, "assets/player.bmp", &view->player_img, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->player_img, &view->player_texture);
    } else {
        all_loaded = false;
    }
    
    if (sdl_view_load_image(view, "assets/saucer.bmp", &view->saucer_img, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->saucer_img, &view->saucer_texture);
    } else {
        all_loaded = false;
    }
    
    for (int i = 0; i < 4; i++) {
        if (sdl_view_load_image(view, "assets/base.bmp", &view->base_img[i], 255, 0, 255)) {
            sdl_view_convert_surface_to_texture(view, view->base_img[i], &view->base_textures[i]);
        } else {
            all_loaded = false;
        }
    }
    
    if (sdl_view_load_image(view, "assets/gameover.bmp", &view->game_over_img, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->game_over_img, &view->game_over_texture);
    }
    
    if (sdl_view_load_image(view, "assets/damage.bmp", &view->damage_img, 0, 255, 0)) {
        sdl_view_convert_surface_to_texture(view, view->damage_img, &view->damage_texture);
    }
    
    if (sdl_view_load_image(view, "assets/damagetop.bmp", &view->damage_top_img, 0, 255, 0)) {
        sdl_view_convert_surface_to_texture(view, view->damage_top_img, &view->damage_top_texture);
    }
    
    view->initialized = true;
    return all_loaded;
}

void sdl_view_draw_background(SDLView* view) {
    if (!view || !view->renderer) return;
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
    SDL_RenderClear(view->renderer);
}

void sdl_view_draw_invaders(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            if (model->invaders.invaders[i][j].alive) {
                SDL_Rect dest = rect_to_sdl_rect(&model->invaders.invaders[i][j].hitbox);
                
                if (view->invaders_texture) {
                    /* Use texture if available */
                    SDL_Rect src;
                    src.w = INVADER_WIDTH;
                    src.h = INVADER_HEIGHT;
                    
                    if (i == 0) {
                        src.y = 0;
                    } else if (i < 3) {
                        src.y = INVADER_HEIGHT;
                    } else {
                        src.y = INVADER_HEIGHT * 2;
                    }
                    
                    src.x = model->invaders.state == 0 ? 0 : INVADER_WIDTH;
                    SDL_RenderCopy(view->renderer, view->invaders_texture, &src, &dest);
                } else {
                    /* Fallback: draw colored rectangles */
                    if (i == 0) {
                        SDL_SetRenderDrawColor(view->renderer, 128, 0, 128, 255); /* Purple */
                    } else if (i < 3) {
                        SDL_SetRenderDrawColor(view->renderer, 0, 255, 0, 255); /* Green */
                    } else {
                        SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255); /* Red */
                    }
                    SDL_RenderFillRect(view->renderer, &dest);
                }
            }
        }
    }
}

void sdl_view_draw_player(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    SDL_Rect dst = rect_to_sdl_rect(&model->player.hitbox);
    
    if (view->player_texture) {
        SDL_Rect src = {0, 0, model->player.hitbox.width, model->player.hitbox.height};
        SDL_RenderCopy(view->renderer, view->player_texture, &src, &dst);
    } else {
        /* Fallback: draw white rectangle */
        SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(view->renderer, &dst);
    }
}

void sdl_view_draw_bases(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    for (int i = 0; i < BASE_COUNT; i++) {
        if (model->bases[i].alive) {
            SDL_Rect dst = rect_to_sdl_rect(&model->bases[i].hitbox);
            
            if (view->base_textures[i]) {
                SDL_Rect src = {0, 0, BASE_WIDTH, BASE_HEIGHT};
                SDL_RenderCopy(view->renderer, view->base_textures[i], &src, &dst);
            } else {
                /* Fallback: draw cyan rectangles */
                SDL_SetRenderDrawColor(view->renderer, 0, 255, 255, 255);
                SDL_RenderFillRect(view->renderer, &dst);
            }
        }
    }
}

void sdl_view_draw_saucer(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    if (model->saucer.alive) {
        SDL_Rect dst = rect_to_sdl_rect(&model->saucer.hitbox);
        
        if (view->saucer_texture) {
            SDL_Rect src = {0, 0, model->saucer.hitbox.width, model->saucer.hitbox.height};
            SDL_RenderCopy(view->renderer, view->saucer_texture, &src, &dst);
        } else {
            /* Fallback: draw magenta rectangle */
            SDL_SetRenderDrawColor(view->renderer, 255, 0, 255, 255);
            SDL_RenderFillRect(view->renderer, &dst);
        }
    }
}

void sdl_view_draw_bullets(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    /* Player bullets (white) */
    SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model->player_bullets[i].alive) {
            SDL_Rect dst = rect_to_sdl_rect(&model->player_bullets[i].hitbox);
            SDL_RenderFillRect(view->renderer, &dst);
        }
    }
    
    /* Enemy bullets (red) */
    SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255);
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        if (model->enemy_bullets[i].alive) {
            SDL_Rect dst = rect_to_sdl_rect(&model->enemy_bullets[i].hitbox);
            SDL_RenderFillRect(view->renderer, &dst);
        }
    }
}

void sdl_view_draw_hud(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    /* Draw HUD background */
    SDL_Rect hud_rect = {GAME_AREA_WIDTH, 0, view->width - GAME_AREA_WIDTH, view->height};
    SDL_SetRenderDrawColor(view->renderer, 41, 41, 41, 255);
    SDL_RenderFillRect(view->renderer, &hud_rect);
    
    /* Draw HUD text using simple rendering */
    char score_text[32];
    char lives_text[32];
    char level_text[32];
    
    snprintf(score_text, sizeof(score_text), "SCORE: %d", model->player.score);
    snprintf(lives_text, sizeof(lives_text), "LIVES: %d", model->player.lives);
    snprintf(level_text, sizeof(level_text), "LEVEL: %d", model->player.level);
    
    draw_string_simple(view, score_text, GAME_AREA_WIDTH + 10, 20, 255, 255, 255);
    draw_string_simple(view, lives_text, GAME_AREA_WIDTH + 10, 50, 255, 255, 255);
    draw_string_simple(view, level_text, GAME_AREA_WIDTH + 10, 80, 255, 255, 255);
}

void sdl_view_render_game(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    sdl_view_draw_background(view);
    sdl_view_draw_bases(view, model);
    sdl_view_draw_invaders(view, model);
    sdl_view_draw_saucer(view, model);
    sdl_view_draw_player(view, model);
    sdl_view_draw_bullets(view, model);
    sdl_view_draw_hud(view, model);
}

void sdl_view_render(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
    SDL_RenderClear(view->renderer);
    
    switch (model->state) {
        case STATE_MENU:
            sdl_view_render_menu(view, model);
            break;
        case STATE_PLAYING:
        case STATE_PAUSED:
        case STATE_LEVEL_TRANSITION:
            sdl_view_render_game(view, model);
            if (model->state == STATE_PAUSED) {
                sdl_view_render_pause(view);
            }
            break;
        case STATE_GAME_OVER:
            sdl_view_render_game(view, model);
            sdl_view_render_game_over(view);
            break;
    }
    
    SDL_RenderPresent(view->renderer);
    
    /* Calculate FPS */
    view->frame_count++;
    uint32_t current_time = SDL_GetTicks();
    if (current_time - view->frame_timer >= 1000) {
        view->fps = view->frame_count * 1000.0f / (current_time - view->frame_timer);
        view->frame_count = 0;
        view->frame_timer = current_time;
    }
    
    view->last_frame_time = current_time;
}

void sdl_view_render_menu(SDLView* view, const GameModel* model) {
    if (!view || !view->renderer) return;
    
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
    SDL_RenderClear(view->renderer);
    
    if (view->title_texture) {
        int tex_w, tex_h;
        SDL_QueryTexture(view->title_texture, NULL, NULL, &tex_w, &tex_h);
        SDL_Rect dest = {(view->width - tex_w) / 2, 50, tex_w, tex_h};
        SDL_RenderCopy(view->renderer, view->title_texture, NULL, &dest);
    } else {
        draw_string_simple(view, "SPACE INVADERS", view->width / 2 - 70, 50, 255, 255, 255);
    }
    
    draw_string_simple(view, "Press SPACE to Start", view->width / 2 - 80, view->height / 2, 255, 255, 255);
    draw_string_simple(view, "ESC to Quit", view->width / 2 - 50, view->height / 2 + 30, 255, 255, 255);
    
    char highscore[32];
    snprintf(highscore, sizeof(highscore), "High Score: %d", model->high_score);
    draw_string_simple(view, highscore, view->width / 2 - 70, view->height / 2 + 60, 255, 255, 0);
}

void sdl_view_render_pause(SDLView* view) {
    if (!view || !view->renderer) return;
    
    /* Semi-transparent overlay */
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, view->width, view->height};
    SDL_RenderFillRect(view->renderer, &overlay);
    
    draw_string_simple(view, "PAUSED", view->width / 2 - 30, view->height / 2 - 20, 255, 255, 255);
    draw_string_simple(view, "Press P to Resume", view->width / 2 - 70, view->height / 2 + 10, 255, 255, 255);
}

void sdl_view_render_game_over(SDLView* view) {
    if (!view || !view->renderer) return;
    
    /* Semi-transparent overlay */
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, view->width, view->height};
    SDL_RenderFillRect(view->renderer, &overlay);
    
    if (view->game_over_texture) {
        int tex_w, tex_h;
        SDL_QueryTexture(view->game_over_texture, NULL, NULL, &tex_w, &tex_h);
        SDL_Rect dest = {(view->width - tex_w) / 2, view->height / 2 - tex_h / 2, tex_w, tex_h};
        SDL_RenderCopy(view->renderer, view->game_over_texture, NULL, &dest);
    } else {
        draw_string_simple(view, "GAME OVER", view->width / 2 - 45, view->height / 2 - 20, 255, 0, 0);
    }
    
    draw_string_simple(view, "Press R to Restart", view->width / 2 - 70, view->height / 2 + 20, 255, 255, 255);
    draw_string_simple(view, "ESC to Quit", view->width / 2 - 50, view->height / 2 + 50, 255, 255, 255);
}

bool sdl_view_poll_event(SDLView* view, SDL_Event* event) {
    (void)view;  /* Suppress unused parameter warning */
    if (!event) return false;
    return SDL_PollEvent(event) != 0;
}

void sdl_view_draw_rect(SDLView* view, int x, int y, int w, int h, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!view || !view->renderer) return;
    
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(view->renderer, r, g, b, a);
    SDL_RenderFillRect(view->renderer, &rect);
}

void sdl_view_draw_text(SDLView* view, const char* text, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !text) return;
    draw_string_simple(view, text, x, y, r, g, b);
}

void sdl_view_draw_char(SDLView* view, char c, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b) {
    if (!view) return;
    char str[2] = {c, '\0'};
    draw_string_simple(view, str, x, y, r, g, b);
}

void sdl_view_cap_framerate(SDLView* view, int target_fps) {
    if (!view || target_fps <= 0) return;
    
    uint32_t frame_delay = 1000 / target_fps;
    uint32_t frame_time = SDL_GetTicks() - view->last_frame_time;
    
    if (frame_time < frame_delay) {
        SDL_Delay(frame_delay - frame_time);
    }
}

float sdl_view_get_fps(SDLView* view) {
    return view ? view->fps : 0.0f;
}