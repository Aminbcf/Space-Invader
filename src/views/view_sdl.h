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
    SDL_Surface* screen_surface;
    
    // Assets
    SDL_Surface* invadersmap;
    SDL_Surface* player_img;
    SDL_Surface* saucer_img;
    SDL_Surface* base_img[4];
    SDL_Surface* damage_img;
    SDL_Surface* damage_top_img;
    
    // Textures
    SDL_Texture* screen_texture;
    SDL_Texture* invaders_texture;
    SDL_Texture* player_texture;
    SDL_Texture* saucer_texture;
    SDL_Texture* base_textures[4];
    SDL_Texture* damage_texture;
    SDL_Texture* damage_top_texture;
    
    // Fonts
    TTF_Font* big_font;
    TTF_Font* medium_font;
    TTF_Font* small_font;
    
    // Display
    int width;
    int height;
    const char* title;
    bool fullscreen;
    bool initialized;
    
    // Timing
    uint32_t last_frame_time;
    uint32_t frame_count;
    uint32_t frame_timer;
    float fps;
} SDLView;

// Creation/destruction
SDLView* sdl_view_create(void);
void sdl_view_destroy(SDLView* view);
bool sdl_view_init(SDLView* view, int width, int height);

// Resource loading
bool sdl_view_load_resources(SDLView* view);
bool sdl_view_load_image(SDLView* view, const char* filename, SDL_Surface** surface, 
                         uint8_t r, uint8_t g, uint8_t b);
bool sdl_view_convert_surface_to_texture(SDLView* view, SDL_Surface* surface, 
                                         SDL_Texture** texture);
bool sdl_view_load_fonts(SDLView* view);

// Rendering
void sdl_view_render(SDLView* view, const GameModel* model);
void sdl_view_render_game(SDLView* view, const GameModel* model);
void sdl_view_render_menu(SDLView* view, const GameModel* model);
void sdl_view_render_pause(SDLView* view);
void sdl_view_render_game_over(SDLView* view);

// Drawing primitives
void sdl_view_draw_background(SDLView* view);
void sdl_view_draw_invaders(SDLView* view, const GameModel* model);
void sdl_view_draw_player(SDLView* view, const GameModel* model);
void sdl_view_draw_bases(SDLView* view, const GameModel* model);
void sdl_view_draw_saucer(SDLView* view, const GameModel* model);
void sdl_view_draw_bullets(SDLView* view, const GameModel* model);
void sdl_view_draw_hud(SDLView* view, const GameModel* model);
void sdl_view_draw_debug(SDLView* view, const GameModel* model);

void sdl_view_draw_rect(SDLView* view, int x, int y, int w, int h, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void sdl_view_draw_text(SDLView* view, const char* text, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b);
void sdl_view_draw_char(SDLView* view, char c, int x, int y,
                       uint8_t r, uint8_t g, uint8_t b);

// Events
bool sdl_view_poll_event(SDLView* view, SDL_Event* event);

// Utilities
void sdl_view_cap_framerate(SDLView* view, int target_fps);
float sdl_view_get_fps(SDLView* view);

#endif // VIEW_SDL_H