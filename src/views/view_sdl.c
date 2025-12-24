#include "view_sdl.h"
#include "rect_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

SDLView* sdl_view_create(void) {
    SDLView* view = malloc(sizeof(SDLView));
    if (!view) return NULL;
    
    memset(view, 0, sizeof(SDLView));
    
    // Configuration par défaut (comme dans le code original)
    view->width = SCREEN_WIDTH;
    view->height = SCREEN_HEIGHT;
    view->title = "Space Invaders MVC";
    view->fullscreen = false;
    
    // Initialiser les surfaces (comme dans le code original)
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
    
    // Initialiser les textures
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
    
    // Configuration de l'interface
    view->interface.init = (bool (*)(void*, int, int))sdl_view_init;
    view->interface.destroy = (void (*)(void*))sdl_view_destroy;
    view->interface.render = (void (*)(void*, const GameModel*))sdl_view_render;
    view->interface.clear = NULL;
    view->interface.present = NULL;
    view->interface.poll_event = (bool (*)(void*, void*))sdl_view_poll_event;
    view->interface.load_texture = NULL;
    view->interface.load_font = NULL;
    view->interface.draw_rect = (void (*)(void*, int, int, int, int, 
                                         uint8_t, uint8_t, uint8_t, uint8_t))sdl_view_draw_rect;
    view->interface.draw_text = (void (*)(void*, const char*, int, int,
                                         uint8_t, uint8_t, uint8_t))sdl_view_draw_text;
    view->interface.get_size = NULL;
    view->interface.set_title = NULL;
    view->interface.delay = NULL;
    view->interface.get_ticks = NULL;
    
    return view;
}

void sdl_view_destroy(SDLView* view) {
    if (!view) return;
    
    // Libérer les textures SDL2
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
    
    // Libérer les surfaces SDL2 (comme dans le code original)
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
    
    // Libérer SDL2
    if (view->renderer) SDL_DestroyRenderer(view->renderer);
    if (view->window) SDL_DestroyWindow(view->window);
    
    free(view);
}

bool sdl_view_init(SDLView* view, int width, int height) {
    if (!view) return false;
    
    // Initialiser SDL2 (comme dans le code original)
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Créer la fenêtre SDL2
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
    
    // Créer le renderer SDL2
    view->renderer = SDL_CreateRenderer(view->window, -1, 
                                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!view->renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(view->window);
        SDL_Quit();
        return false;
    }
    
    // Créer la surface d'écran (comme dans le code original)
    view->screen_surface = SDL_CreateRGBSurface(0, width, height, 32,
                                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!view->screen_surface) {
        fprintf(stderr, "SDL_CreateRGBSurface Error: %s\n", SDL_GetError());
        sdl_view_destroy(view);
        return false;
    }
    
    // Créer la texture d'écran
    view->screen_texture = SDL_CreateTexture(view->renderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            width, height);
    if (!view->screen_texture) {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        sdl_view_destroy(view);
        return false;
    }
    
    // Définir la taille de la vue
    view->width = width;
    view->height = height;
    
    // Charger les ressources (comme dans le code original)
    if (!sdl_view_load_resources(view)) {
        fprintf(stderr, "Failed to load resources\n");
        sdl_view_destroy(view);
        return false;
    }
    
    // Initialiser le timer FPS
    view->last_frame_time = SDL_GetTicks();
    view->frame_count = 0;
    view->frame_timer = view->last_frame_time;
    view->fps = 0.0f;
    
    view->initialized = true;
    return true;
}

// Fonction load_image du code original adaptée
bool sdl_view_load_image(SDLView* view, const char* filename, SDL_Surface** surface, 
                        uint8_t r, uint8_t g, uint8_t b) {
    SDL_Surface* temp = SDL_LoadBMP(filename);
    if (temp == NULL) {
        fprintf(stderr, "Unable to load %s: %s\n", filename, SDL_GetError());
        return false;
    }

    // Définir la couleur clé
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, r, g, b));
    
    // Convertir au format d'écran
    *surface = SDL_ConvertSurface(temp, view->screen_surface->format, 0);
    if (*surface == NULL) {
        fprintf(stderr, "Unable to convert bitmap: %s\n", SDL_GetError());
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
        fprintf(stderr, "Failed to create texture from surface: %s\n", SDL_GetError());
        return false;
    }
    
    return true;
}

bool sdl_view_load_resources(SDLView* view) {
    // Charger les images (comme dans le code original)
    if (!sdl_view_load_image(view, "assets/titlescreen.bmp", &view->title_screen, 255, 0, 255)) {
        fprintf(stderr, "Failed to load title screen\n");
        return false;
    }
    
    if (!sdl_view_load_image(view, "assets/cmap.bmp", &view->cmap, 255, 0, 255)) {
        fprintf(stderr, "Failed to load font map\n");
        return false;
    }
    
    if (!sdl_view_load_image(view, "assets/invaders.bmp", &view->invadersmap, 255, 0, 255)) {
        fprintf(stderr, "Failed to load invaders spritesheet\n");
        return false;
    }
    
    if (!sdl_view_load_image(view, "assets/player.bmp", &view->player_img, 255, 0, 255)) {
        fprintf(stderr, "Failed to load player sprite\n");
        return false;
    }
    
    if (!sdl_view_load_image(view, "assets/saucer.bmp", &view->saucer_img, 255, 0, 255)) {
        fprintf(stderr, "Failed to load saucer sprite\n");
        return false;
    }
    
    // Charger les bases
    for (int i = 0; i < 4; i++) {
        if (!sdl_view_load_image(view, "assets/base.bmp", &view->base_img[i], 255, 0, 255)) {
            fprintf(stderr, "Failed to load base sprite %d\n", i);
            return false;
        }
    }
    
    if (!sdl_view_load_image(view, "assets/gameover.bmp", &view->game_over_img, 255, 0, 255)) {
        fprintf(stderr, "Failed to load game over image\n");
        return false;
    }
    
    if (!sdl_view_load_image(view, "assets/damage.bmp", &view->damage_img, 0, 255, 0)) {
        fprintf(stderr, "Failed to load damage sprite\n");
        return false;
    }
    
    if (!sdl_view_load_image(view, "assets/damagetop.bmp", &view->damage_top_img, 0, 255, 0)) {
        fprintf(stderr, "Failed to load damage top sprite\n");
        return false;
    }
    
    // Convertir les surfaces en textures
    sdl_view_convert_surface_to_texture(view, view->title_screen, &view->title_texture);
    sdl_view_convert_surface_to_texture(view, view->cmap, &view->font_texture);
    sdl_view_convert_surface_to_texture(view, view->invadersmap, &view->invaders_texture);
    sdl_view_convert_surface_to_texture(view, view->player_img, &view->player_texture);
    sdl_view_convert_surface_to_texture(view, view->saucer_img, &view->saucer_texture);
    for (int i = 0; i < 4; i++) {
        sdl_view_convert_surface_to_texture(view, view->base_img[i], &view->base_textures[i]);
    }
    sdl_view_convert_surface_to_texture(view, view->game_over_img, &view->game_over_texture);
    sdl_view_convert_surface_to_texture(view, view->damage_img, &view->damage_texture);
    sdl_view_convert_surface_to_texture(view, view->damage_top_img, &view->damage_top_texture);
    
    view->initialized = true;
    return true;
}

void sdl_view_draw_background(SDLView* view) {
    if (!view || !view->screen_surface) return;
    
    SDL_Rect src = {0, 0, view->width, view->height};
    SDL_FillRect(view->screen_surface, &src, SDL_MapRGB(view->screen_surface->format, 0, 0, 0));
}

void sdl_view_draw_invaders(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->invadersmap) return;
    
    SDL_Rect src, dest;
    
    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            if (model->invaders.invaders[i][j].alive) {
                // Position source dans la spritesheet
                src.w = INVADER_WIDTH;
                src.h = INVADER_HEIGHT;
                
                // Déterminer la ligne de la spritesheet en fonction de la couleur
                if (i == 0) { // purple
                    src.y = 0;
                } else if (i < 3) { // green
                    src.y = INVADER_HEIGHT;
                } else { // red
                    src.y = INVADER_HEIGHT * 2;
                }
                
                // Colonne de la spritesheet en fonction de l'état d'animation
                src.x = model->invaders.state == 0 ? 0 : INVADER_WIDTH;
                
                // Position destination
                dest.x = model->invaders.invaders[i][j].hitbox.x;
                dest.y = model->invaders.invaders[i][j].hitbox.y;
                dest.w = model->invaders.invaders[i][j].hitbox.width;
                dest.h = model->invaders.invaders[i][j].hitbox.height;
                
                // Blit sur la surface d'écran
                SDL_BlitSurface(view->invadersmap, &src, view->screen_surface, &dest);
            }
        }
    }
}

void sdl_view_draw_player(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->player_img) return;
    
    SDL_Rect src = {0, 0, model->player.hitbox.width, model->player.hitbox.height};
    SDL_Rect dst = rect_to_sdl_rect(&model->player.hitbox);
    SDL_BlitSurface(view->player_img, &src, view->screen_surface, &dst);
}

void sdl_view_draw_bases(SDLView* view, const GameModel* model) {
    if (!view || !model) return;
    
    SDL_Rect src = {0, 0, BASE_WIDTH, BASE_HEIGHT};
    
    for (int i = 0; i < BASE_COUNT; i++) {
        if (view->base_img[i]) {
            SDL_Rect dst = rect_to_sdl_rect(&model->bases[i].hitbox);
            SDL_BlitSurface(view->base_img[i], &src, view->screen_surface, &dst);
        }
    }
}

void sdl_view_draw_saucer(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->saucer_img) return;
    
    if (model->saucer.alive) {
        SDL_Rect src = {0, 0, model->saucer.hitbox.width, model->saucer.hitbox.height};
        SDL_Rect dst = rect_to_sdl_rect(&model->saucer.hitbox);
        SDL_BlitSurface(view->saucer_img, &src, view->screen_surface, &dst);
    }
}

void sdl_view_draw_bullets(SDLView* view, const GameModel* model) {
    if (!view || !model) return;
    
    // Bullets du joueur (blanches)
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model->player_bullets[i].alive) {
            SDL_Rect dst = rect_to_sdl_rect(&model->player_bullets[i].hitbox);
            SDL_FillRect(view->screen_surface, &dst, 
                        SDL_MapRGB(view->screen_surface->format, 255, 255, 255));
        }
    }
    // Bullets ennemies (rouges)
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        if (model->enemy_bullets[i].alive) {
            SDL_Rect dst = rect_to_sdl_rect(&model->enemy_bullets[i].hitbox);
            SDL_FillRect(view->screen_surface, &dst, 
                        SDL_MapRGB(view->screen_surface->format, 255, 0, 0));
        }
    }
}

void sdl_view_draw_hud(SDLView* view, const GameModel* model) {
    if (!view || !model) return;
    
    // Zone HUD (comme dans le code original)
    SDL_Rect r = {GAME_AREA_WIDTH, 0, view->width - GAME_AREA_WIDTH, view->height};
    SDL_FillRect(view->screen_surface, &r, SDL_MapRGB(view->screen_surface->format, 41, 41, 41));
    
    // Dessiner le texte HUD (simplifié - dans le code original on utilisait draw_string)
    // Pour l'instant, on laisse cette partie pour plus tard
    
    // Dans une vraie implémentation, on utiliserait la fonction draw_string du code original
}

void sdl_view_render_game(SDLView* view, const GameModel* model) {
    if (!view || !model) return;
    
    // Dessiner l'arrière-plan
    sdl_view_draw_background(view);
    
    // Dessiner les éléments du jeu
    sdl_view_draw_bases(view, model);
    sdl_view_draw_invaders(view, model);
    sdl_view_draw_saucer(view, model);
    sdl_view_draw_player(view, model);
    sdl_view_draw_bullets(view, model);
    
    // Dessiner le HUD
    sdl_view_draw_hud(view, model);
}

void sdl_view_render(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    // Effacer le renderer
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 255);
    SDL_RenderClear(view->renderer);
    
    // Rendre en fonction de l'état du jeu
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
    
    // Mettre à jour la texture avec le contenu de la surface
    SDL_UpdateTexture(view->screen_texture, NULL, 
                     view->screen_surface->pixels, 
                     view->screen_surface->pitch);
    
    // Copier la texture sur le renderer
    SDL_RenderCopy(view->renderer, view->screen_texture, NULL, NULL);
    
    // Présenter le renderer
    SDL_RenderPresent(view->renderer);
    
    // Calculer FPS
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
    if (!view || !view->screen_surface) return;
    
    // Effacer l'écran
    SDL_FillRect(view->screen_surface, NULL, SDL_MapRGB(view->screen_surface->format, 0, 0, 0));
    
    // Afficher l'écran titre (comme dans le code original)
    if (view->title_screen) {
        SDL_Rect src = {0, 0, view->title_screen->w, view->title_screen->h};
        SDL_Rect dest = {(view->width - view->title_screen->w) / 2, 0, 
                         view->title_screen->w, view->title_screen->h};
        SDL_BlitSurface(view->title_screen, &src, view->screen_surface, &dest);
    }
    
    // Texte d'instruction
    // (À implémenter avec draw_string comme dans le code original)
}

void sdl_view_render_pause(SDLView* view) {
    if (!view || !view->screen_surface) return;
    
    // Créer un rectangle semi-transparent pour l'overlay
    SDL_Surface* overlay = SDL_CreateRGBSurface(0, view->width, view->height, 32,
                                               0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (overlay) {
        SDL_FillRect(overlay, NULL, SDL_MapRGBA(overlay->format, 0, 0, 0, 180));
        SDL_BlitSurface(overlay, NULL, view->screen_surface, NULL);
        SDL_FreeSurface(overlay);
    }
    
    // Texte PAUSE
    // (À implémenter avec draw_string)
}

void sdl_view_render_game_over(SDLView* view) {
    if (!view || !view->game_over_img) return;
    
    // Afficher l'image Game Over (comme dans le code original)
    SDL_Rect src = {0, 0, view->game_over_img->w, view->game_over_img->h};
    SDL_Rect dest = {(view->width - view->game_over_img->w) / 2,
                    (view->height - view->game_over_img->h) / 2,
                    view->game_over_img->w, view->game_over_img->h};
    SDL_BlitSurface(view->game_over_img, &src, view->screen_surface, &dest);
}

bool sdl_view_poll_event(SDLView* view, SDL_Event* event) {
    if (!view || !event) return false;
    return SDL_PollEvent(event) != 0;
}

void sdl_view_draw_rect(SDLView* view, int x, int y, int w, int h, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!view || !view->screen_surface) return;
    
    SDL_Rect rect = {x, y, w, h};
    SDL_FillRect(view->screen_surface, &rect, SDL_MapRGBA(view->screen_surface->format, r, g, b, a));
}

void sdl_view_draw_text(SDLView* view, const char* text, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !text || !view->cmap) return;
    
    // Implémentation simplifiée de draw_string du code original
    for (int i = 0; text[i] != '\0'; i++) {
        sdl_view_draw_char(view, text[i], x + i * 20, y, r, g, b);
    }
}

void sdl_view_draw_char(SDLView* view, char c, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !view->cmap) return;
    
    // Implémentation simplifiée de draw_char du code original
    SDL_Rect src, dest;
    char* map[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                  "abcdefghijklmnopqrstuvwxyz",
                  "!@#$%^&*()_+{}|:\"<>?,.;'-=",
                  "0123456789"};

    src.x = 0;
    src.y = 0;
    src.w = 20;
    src.h = 20;
    
    dest.x = x;
    dest.y = y;
    dest.w = 20;
    dest.h = 20;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < strlen(map[i]); j++) {
            if (c == map[i][j]) {
                SDL_BlitSurface(view->cmap, &src, view->screen_surface, &dest);
                return;
            }
            src.x += 20;
        }
        src.y += 20;
        src.x = 0;
    }
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