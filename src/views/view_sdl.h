#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdbool.h>
#include "../core/model.h"
#include "view_base.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_ttf.h>
#include "font_manager.h"

// Structure pour une texture SDL2
typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
} SDLTexture;

// Structure principale de la vue SDL2
// In view_sdl.h, update the SDLView struct:
typedef struct {
    // SDL2 components
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Configuration
    int width;
    int height;
    bool fullscreen;
    const char* title;
    
    // Textures (remove unused ones)
    SDL_Surface* screen_surface;
    SDL_Surface* cmap;
    SDL_Surface* invadersmap;
    SDL_Surface* player_img;
    SDL_Surface* saucer_img;
    SDL_Surface* base_img[4];
    SDL_Surface* damage_img;
    SDL_Surface* damage_top_img;
    // REMOVED: title_screen and game_over_img
    
    // Textures converties
    SDL_Texture* screen_texture;
    SDL_Texture* font_texture;
    SDL_Texture* invaders_texture;
    SDL_Texture* player_texture;
    SDL_Texture* saucer_texture;
    SDL_Texture* base_textures[4];
    SDL_Texture* damage_texture;
    SDL_Texture* damage_top_texture;
    //title_texture and game_over_texture
    
    // TTF Fonts
    TTF_Font* big_font;
    TTF_Font* medium_font;
    TTF_Font* small_font;
    
    // État
    bool initialized;
    uint32_t last_frame_time;
    int frame_count;
    uint32_t frame_timer;
    float fps;
    
    // Interface
    ViewInterface interface;
} SDLView;

// Création et destruction
SDLView* sdl_view_create(void);
void sdl_view_destroy(SDLView* view);

// Initialisation
bool sdl_view_init(SDLView* view, int width, int height);
bool sdl_view_load_resources(SDLView* view);

// Rendu
void sdl_view_render(SDLView* view, const GameModel* model);
void sdl_view_render_game(SDLView* view, const GameModel* model);
void sdl_view_render_menu(SDLView* view, const GameModel* model);
void sdl_view_render_hud(SDLView* view, const GameModel* model);
void sdl_view_render_pause(SDLView* view);
void sdl_view_render_game_over(SDLView* view);

// Événements
bool sdl_view_poll_event(SDLView* view, SDL_Event* event);

// Utilitaires de dessin
void sdl_view_draw_rect(SDLView* view, int x, int y, int w, int h, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void sdl_view_draw_text(SDLView* view, const char* text, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b);
void sdl_view_draw_char(SDLView* view, char c, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b);

// Gestion des textures (comme dans le code original)
bool sdl_view_load_image(SDLView* view, const char* filename, SDL_Surface** surface, 
                        uint8_t r, uint8_t g, uint8_t b);
bool sdl_view_convert_surface_to_texture(SDLView* view, SDL_Surface* surface, 
                                        SDL_Texture** texture);

// Gestion des frames
void sdl_view_cap_framerate(SDLView* view, int target_fps);
float sdl_view_get_fps(SDLView* view);

// Fonctions utilitaires du code original
void sdl_view_draw_background(SDLView* view);
void sdl_view_draw_invaders(SDLView* view, const GameModel* model);
void sdl_view_draw_player(SDLView* view, const GameModel* model);
void sdl_view_draw_bases(SDLView* view, const GameModel* model);
void sdl_view_draw_saucer(SDLView* view, const GameModel* model);
void sdl_view_draw_bullets(SDLView* view, const GameModel* model);

#endif // VIEW_SDL_H