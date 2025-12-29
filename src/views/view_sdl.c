#include "view_sdl.h"
#include "rect_utils.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
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
    {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E}, /* [ */
    {0x00,0x10,0x08,0x04,0x02,0x01,0x00}, /* \ */
    {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E}, /* ] */
    {0x04,0x0A,0x11,0x00,0x00,0x00,0x00}, /* ^ */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x1F}, /* _ */
    {0x08,0x04,0x02,0x00,0x00,0x00,0x00}, /* ` */
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F}, /* a */
    {0x10,0x10,0x16,0x19,0x11,0x11,0x1E}, /* b */
    {0x00,0x00,0x0E,0x10,0x10,0x11,0x0E}, /* c */
    {0x01,0x01,0x0D,0x13,0x11,0x11,0x0F}, /* d */
    {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E}, /* e */
    {0x06,0x09,0x08,0x1C,0x08,0x08,0x08}, /* f */
    {0x00,0x0F,0x11,0x11,0x0F,0x01,0x0E}, /* g */
    {0x10,0x10,0x16,0x19,0x11,0x11,0x11}, /* h */
    {0x04,0x00,0x0C,0x04,0x04,0x04,0x0E}, /* i */
    {0x02,0x00,0x06,0x02,0x02,0x12,0x0C}, /* j */
    {0x10,0x10,0x12,0x14,0x18,0x14,0x12}, /* k */
    {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E}, /* l */
    {0x00,0x00,0x1A,0x15,0x15,0x11,0x11}, /* m */
    {0x00,0x00,0x16,0x19,0x11,0x11,0x11}, /* n */
    {0x00,0x00,0x0E,0x11,0x11,0x11,0x0E}, /* o */
    {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10}, /* p */
    {0x00,0x00,0x0D,0x13,0x0F,0x01,0x01}, /* q */
    {0x00,0x00,0x16,0x19,0x10,0x10,0x10}, /* r */
    {0x00,0x00,0x0E,0x10,0x0E,0x01,0x1E}, /* s */
    {0x08,0x08,0x1C,0x08,0x08,0x09,0x06}, /* t */
    {0x00,0x00,0x11,0x11,0x11,0x13,0x0D}, /* u */
    {0x00,0x00,0x11,0x11,0x11,0x0A,0x04}, /* v */
    {0x00,0x00,0x11,0x11,0x15,0x15,0x0A}, /* w */
    {0x00,0x00,0x11,0x0A,0x04,0x0A,0x11}, /* x */
    {0x00,0x00,0x11,0x11,0x0F,0x01,0x0E}, /* y */
    {0x00,0x00,0x1F,0x02,0x04,0x08,0x1F}, /* z */
    {0x02,0x04,0x04,0x08,0x04,0x04,0x02}, /* { */
    {0x04,0x04,0x04,0x04,0x04,0x04,0x04}, /* | */
    {0x08,0x04,0x04,0x02,0x04,0x04,0x08}, /* } */
    {0x00,0x00,0x08,0x15,0x02,0x00,0x00}, /* ~ */
};

/* Helper function for SDL3 text rendering */
static SDL_Surface* render_text_sdl3(TTF_Font* font, const char* text, SDL_Color color) {
    if (!font || !text) return NULL;
    // SDL3 TTF_RenderText_Blended takes the text and color
    return TTF_RenderText_Blended(font, text, sizeof(text) , color);
}

/* Improved text rendering using bitmap font */
static void draw_string_simple(SDLView* view, const char* text, int x, int y, 
                               uint8_t r, uint8_t g, uint8_t b) {
    if (!view || !text || !view->renderer) return;
    
    SDL_SetRenderDrawColor(view->renderer, r, g, b, 255);
    
    int char_width = 6;
    int char_height = 7;
    int scale = 2;
    
    for (int i = 0; text[i] != '\0'; i++) {
        int char_index = text[i] - 32;
        
        if (char_index < 0 || char_index >= 95) {
            continue;
        }
        
        for (int row = 0; row < char_height; row++) {
            unsigned char line = font_data[char_index][row];
            for (int col = 0; col < 5; col++) {
                if (line & (1 << (4 - col))) {
                    SDL_FRect pixel = {
                        (float)(x + i * char_width * scale + col * scale),
                        (float)(y + row * scale),
                        (float)scale,
                        (float)scale
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
    
    view->window = NULL;
    view->renderer = NULL;
    view->screen_surface = NULL;
    
    view->invadersmap = NULL;
    view->player_img = NULL;
    view->saucer_img = NULL;
    for (int i = 0; i < 4; i++) view->base_img[i] = NULL;
    view->damage_img = NULL;
    view->damage_top_img = NULL;
    
    view->screen_texture = NULL;
    view->invaders_texture = NULL;
    view->player_texture = NULL;
    view->saucer_texture = NULL;
    for (int i = 0; i < 4; i++) view->base_textures[i] = NULL;
    view->damage_texture = NULL;
    view->damage_top_texture = NULL;
    
    view->big_font = NULL;
    view->medium_font = NULL;
    view->small_font = NULL;
    
    view->initialized = false;
    view->last_frame_time = 0;
    view->frame_count = 0;
    view->frame_timer = 0;
    view->fps = 0.0f;
    
    return view;
}

void sdl_view_destroy(SDLView* view) {
    if (!view) return;
    
    if (view->screen_texture) SDL_DestroyTexture(view->screen_texture);
    if (view->invaders_texture) SDL_DestroyTexture(view->invaders_texture);
    if (view->player_texture) SDL_DestroyTexture(view->player_texture);
    if (view->saucer_texture) SDL_DestroyTexture(view->saucer_texture);
    for (int i = 0; i < 4; i++) {
        if (view->base_textures[i]) SDL_DestroyTexture(view->base_textures[i]);
    }
    if (view->damage_texture) SDL_DestroyTexture(view->damage_texture);
    if (view->damage_top_texture) SDL_DestroyTexture(view->damage_top_texture);
    
    if (view->screen_surface) SDL_DestroySurface(view->screen_surface);
    if (view->invadersmap) SDL_DestroySurface(view->invadersmap);
    if (view->player_img) SDL_DestroySurface(view->player_img);
    if (view->saucer_img) SDL_DestroySurface(view->saucer_img);
    for (int i = 0; i < 4; i++) {
        if (view->base_img[i]) SDL_DestroySurface(view->base_img[i]);
    }
    if (view->damage_img) SDL_DestroySurface(view->damage_img);
    if (view->damage_top_img) SDL_DestroySurface(view->damage_top_img);
    
    if (view->big_font) TTF_CloseFont(view->big_font);
    if (view->medium_font) TTF_CloseFont(view->medium_font);
    if (view->small_font) TTF_CloseFont(view->small_font);
    
    if (view->renderer) SDL_DestroyRenderer(view->renderer);
    if (view->window) SDL_DestroyWindow(view->window);
    
    TTF_Quit();
    // SDL3_image no longer needs IMG_Quit() - removed
    SDL_Quit();
    
    free(view);
}

bool sdl_view_init(SDLView* view, int width, int height) {
    if (!view) return false;
    
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    if (!TTF_Init()) {
        fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    
    // SDL3_image no longer needs IMG_Init() - automatic initialization
    
    view->window = SDL_CreateWindow(view->title, width, height, SDL_WINDOW_RESIZABLE);
    if (!view->window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    
    view->renderer = SDL_CreateRenderer(view->window, NULL);
    if (!view->renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(view->window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    
    view->screen_surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB8888);
    if (!view->screen_surface) {
        fprintf(stderr, "SDL_CreateSurface Error: %s\n", SDL_GetError());
        sdl_view_destroy(view);
        return false;
    }
    
    view->screen_texture = SDL_CreateTexture(view->renderer,
                                            SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_TARGET,
                                            width, height);
    if (!view->screen_texture) {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        sdl_view_destroy(view);
        return false;
    }
    
    view->width = width;
    view->height = height;
    
    if (!sdl_view_load_resources(view)) {
        fprintf(stderr, "Warning: Failed to load some resources\n");
    }
    
    view->last_frame_time = SDL_GetTicks();
    view->frame_count = 0;
    view->frame_timer = view->last_frame_time;
    view->fps = 0.0f;
    
    view->initialized = true;
    return true;
}

bool sdl_view_load_image(SDLView* view, const char* filename, SDL_Surface** surface, 
                        uint8_t r, uint8_t g, uint8_t b) {
    (void)view;
    
    SDL_Surface* temp = IMG_Load(filename);
    if (temp == NULL) {
        fprintf(stderr, "IMG_Load Error for %s: %s\n", filename, SDL_GetError());
        return false;
    }
    
    // SDL3: Use SDL_ConvertSurface instead of SDL_ConvertSurfaceFormat
    SDL_Surface* converted = SDL_ConvertSurface(temp, SDL_PIXELFORMAT_ARGB8888);
    SDL_DestroySurface(temp);
    
    if (converted == NULL) {
        fprintf(stderr, "Warning: Unable to convert surface: %s\n", SDL_GetError());
        return false;
    }
    
    // If you need to set transparency based on a specific color,
    // you can manually process the pixels
    if (r != 255 || g != 0 || b != 255) {  // Only if not magenta
        // Process pixels to set transparency
        SDL_LockSurface(converted);
        Uint32* pixels = (Uint32*)converted->pixels;
        int pixel_count = converted->w * converted->h;
        Uint32 transparent_color = (r << 16) | (g << 8) | b | 0xFF000000;
        
        for (int i = 0; i < pixel_count; i++) {
            if ((pixels[i] & 0x00FFFFFF) == (transparent_color & 0x00FFFFFF)) {
                pixels[i] = 0x00000000;  // Fully transparent
            }
        }
        SDL_UnlockSurface(converted);
    }
    
    *surface = converted;
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

bool sdl_view_load_fonts(SDLView* view) {
    const char* font_paths[] = {
        "fonts/venite-adoremus-font/VeniteAdoremus-rgRBA.ttf",
        "fonts/venite-adoremus-font/VeniteAdoremusStraight-Yzo6v.ttf",
        "fonts/venite-adoremus-font/Aa VeniteAdoremus-rgRBA.ttf",
        "fonts/venite-adoremus-font/Aa VeniteAdoremusStraight-Yzo6v.ttf",
        NULL
    };
    
    const char* successful_path = NULL;
    for (int i = 0; font_paths[i] != NULL; i++) {
        FILE* test = fopen(font_paths[i], "r");
        if (test) {
            fclose(test);
            successful_path = font_paths[i];
            break;
        }
    }
    
    if (successful_path) {
        /* USE THE WORKING FONT PATH FOR ALL SIZES */
        view->big_font = TTF_OpenFont(successful_path, 62);
        view->medium_font = TTF_OpenFont(successful_path, 26);
        view->small_font = TTF_OpenFont(successful_path, 14);
        
        printf("Loaded fonts from: %s\n", successful_path);
        
        /* Note: We return true even if some failed, to allow partial loading */
        return true;
    }
    
    fprintf(stderr, "Custom font not found, using fallback font\n");
    
    const char* system_fonts[] = {
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        NULL
    };
    
    for (int i = 0; system_fonts[i] != NULL; i++) {
        view->big_font = TTF_OpenFont(system_fonts[i], 72);
        view->medium_font = TTF_OpenFont(system_fonts[i], 36);
        view->small_font = TTF_OpenFont(system_fonts[i], 24);
        
        if (view->big_font) {
            printf("Using system font: %s\n", system_fonts[i]);
            return true;
        }
    }
    
    return false;
}

bool sdl_view_load_resources(SDLView* view) {
    bool all_loaded = true;
    
    if (!sdl_view_load_fonts(view)) {
        fprintf(stderr, "Warning: Could not load custom fonts\n");
    }
    
    if (sdl_view_load_image(view, "assets/invaders.png", &view->invadersmap, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->invadersmap, &view->invaders_texture);
    } else {
        fprintf(stderr, "Failed to load invaders.png\n");
        all_loaded = false;
    }
    
    if (sdl_view_load_image(view, "assets/player.bmp", &view->player_img, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->player_img, &view->player_texture);
    } else {
        fprintf(stderr, "Failed to load player.bmp\n");
        all_loaded = false;
    }
    
    if (sdl_view_load_image(view, "assets/saucer.png", &view->saucer_img, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->saucer_img, &view->saucer_texture);
        printf("Loaded saucer texture\n");
    } else {
        printf("Note: Saucer image not found, using fallback rectangle\n");
        all_loaded = false;
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
                SDL_FRect dest = {
                    (float)model->invaders.invaders[i][j].hitbox.x,
                    (float)model->invaders.invaders[i][j].hitbox.y,
                    (float)model->invaders.invaders[i][j].hitbox.width,
                    (float)model->invaders.invaders[i][j].hitbox.height
                };
                
                if (view->invaders_texture) {
                    SDL_FRect src = {
                        0.0f,
                        (float)(i == 0 ? 0 : (i < 3 ? INVADER_HEIGHT : INVADER_HEIGHT * 2)),
                        (float)INVADER_WIDTH,
                        (float)INVADER_HEIGHT
                    };
                    
                    src.x = model->invaders.state == 0 ? 0.0f : (float)INVADER_WIDTH;
                    SDL_RenderTexture(view->renderer, view->invaders_texture, &src, &dest);
                } else {
                    if (i == 0) {
                        SDL_SetRenderDrawColor(view->renderer, 128, 0, 128, 255);
                    } else if (i < 3) {
                        SDL_SetRenderDrawColor(view->renderer, 0, 255, 0, 255);
                    } else {
                        SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255);
                    }
                    SDL_RenderFillRect(view->renderer, &dest);
                }
            }
        }
    }
}

void sdl_view_draw_player(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    SDL_FRect dst = {
        (float)model->player.hitbox.x,
        (float)model->player.hitbox.y,
        (float)model->player.hitbox.width,
        (float)model->player.hitbox.height
    };
    
    if (view->player_texture) {
        SDL_FRect src = {0.0f, 0.0f, (float)model->player.hitbox.width, (float)model->player.hitbox.height};
        SDL_RenderTexture(view->renderer, view->player_texture, &src, &dst);
    } else {
        SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(view->renderer, &dst);
    }
}

void sdl_view_draw_bases(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    for (int i = 0; i < BASE_COUNT; i++) {
        if (model->bases[i].alive) {
            SDL_FRect dst = {
                (float)model->bases[i].hitbox.x,
                (float)model->bases[i].hitbox.y,
                (float)model->bases[i].hitbox.width,
                (float)model->bases[i].hitbox.height
            };
            
            if (view->base_textures[i]) {
                SDL_FRect src = {0.0f, 0.0f, (float)BASE_WIDTH, (float)BASE_HEIGHT};
                SDL_RenderTexture(view->renderer, view->base_textures[i], &src, &dst);
            } else {
                SDL_SetRenderDrawColor(view->renderer, 0, 255, 255, 255);
                SDL_RenderFillRect(view->renderer, &dst);
            }
        }
    }
}

void sdl_view_draw_saucer(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    if (model->saucer.alive) {
        SDL_FRect dst = {
            (float)model->saucer.hitbox.x,
            (float)model->saucer.hitbox.y,
            (float)model->saucer.hitbox.width,
            (float)model->saucer.hitbox.height
        };
        
        if (view->saucer_texture) {
            SDL_FRect src = {0.0f, 0.0f, (float)model->saucer.hitbox.width, (float)model->saucer.hitbox.height};
            SDL_RenderTexture(view->renderer, view->saucer_texture, &src, &dst);
        } else {
            SDL_SetRenderDrawColor(view->renderer, 255, 0, 255, 255);
            SDL_RenderFillRect(view->renderer, &dst);
        }
    }
}

void sdl_view_draw_bullets(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model->player_bullets[i].alive) {
            SDL_FRect dst = {
                (float)model->player_bullets[i].hitbox.x,
                (float)model->player_bullets[i].hitbox.y,
                (float)model->player_bullets[i].hitbox.width,
                (float)model->player_bullets[i].hitbox.height
            };
            SDL_RenderFillRect(view->renderer, &dst);
        }
    }
    
    SDL_SetRenderDrawColor(view->renderer, 255, 0, 0, 255);
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        if (model->enemy_bullets[i].alive) {
            SDL_FRect dst = {
                (float)model->enemy_bullets[i].hitbox.x,
                (float)model->enemy_bullets[i].hitbox.y,
                (float)model->enemy_bullets[i].hitbox.width,
                (float)model->enemy_bullets[i].hitbox.height
            };
            SDL_RenderFillRect(view->renderer, &dst);
        }
    }
}

void sdl_view_draw_hud(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    SDL_FRect hud_rect = {(float)GAME_AREA_WIDTH, 0.0f, 
                         (float)(view->width - GAME_AREA_WIDTH), (float)view->height};
    SDL_SetRenderDrawColor(view->renderer, 41, 41, 41, 255);
    SDL_RenderFillRect(view->renderer, &hud_rect);
    
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

void sdl_view_draw_debug(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    char debug_text[100];
    snprintf(debug_text, sizeof(debug_text), 
             "Shots: %d, Saucer alive: %d, X: %d, Y: %d", 
             model->player.shots_fired,
             model->saucer.alive,
             model->saucer.hitbox.x,
             model->saucer.hitbox.y);
    
    if (view->small_font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* debug_surface = render_text_sdl3(view->small_font, debug_text, white);
        if (debug_surface) {
            SDL_Texture* debug_texture = SDL_CreateTextureFromSurface(view->renderer, debug_surface);
            SDL_FRect debug_rect = {10.0f, 10.0f, (float)debug_surface->w, (float)debug_surface->h};
            SDL_RenderTexture(view->renderer, debug_texture, NULL, &debug_rect);
            SDL_DestroySurface(debug_surface);
            SDL_DestroyTexture(debug_texture);
        }
    } else {
        draw_string_simple(view, debug_text, 10, 10, 255, 255, 255);
    }
}

void sdl_view_render_game(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    sdl_view_draw_background(view);
    sdl_view_draw_invaders(view, model);
    sdl_view_draw_saucer(view, model);  
    sdl_view_draw_player(view, model);
    sdl_view_draw_bullets(view, model);
    sdl_view_draw_hud(view, model);
    sdl_view_draw_debug(view, model);
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
    
    int center_x = view->width / 2;
    int center_y = 100;
    
    /* Draw Title */
    if (view->big_font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* title_surface = render_text_sdl3(view->big_font, "SPACE INVADERS", white);
        if (title_surface) {
            SDL_Texture* title_texture = SDL_CreateTextureFromSurface(view->renderer, title_surface);
            SDL_FRect title_rect = {
                (float)(center_x - title_surface->w / 2),
                (float)center_y,
                (float)title_surface->w,
                (float)title_surface->h
            };
            SDL_RenderTexture(view->renderer, title_texture, NULL, &title_rect);
            SDL_DestroySurface(title_surface);
            SDL_DestroyTexture(title_texture);
            center_y += title_surface->h + 30;
        }
    } else {
        /* Fallback if big font failed */
        draw_string_simple(view, "SPACE INVADERS", center_x - 70, center_y, 255, 255, 255);
        center_y += 60;
    }

    /* Draw Author */
    if (view->medium_font) {
        SDL_Color cyan = {0, 255, 255, 255};
        SDL_Surface* author_surface = render_text_sdl3(view->medium_font, "by Amine Boucif", cyan);
        if (author_surface) {
            SDL_Texture* author_texture = SDL_CreateTextureFromSurface(view->renderer, author_surface);
            SDL_FRect author_rect = {
                (float)(center_x - author_surface->w / 2),
                (float)center_y,
                (float)author_surface->w,
                (float)author_surface->h
            };
            SDL_RenderTexture(view->renderer, author_texture, NULL, &author_rect);
            SDL_DestroySurface(author_surface);
            SDL_DestroyTexture(author_texture);
            center_y += author_surface->h + 40;
        }
    } else {
        draw_string_simple(view, "by Amine Boucif", center_x - 40, center_y, 0, 255, 255);
        center_y += 40;
    }

    /* Draw High Score */
    char highscore_text[50];
    snprintf(highscore_text, sizeof(highscore_text), "HIGH SCORE: %d", model->high_score);

    if (view->medium_font) {
        SDL_Color yellow = {255, 255, 0, 255};
        SDL_Surface* score_surface = render_text_sdl3(view->medium_font, highscore_text, yellow);
        if (score_surface) {
            SDL_Texture* score_texture = SDL_CreateTextureFromSurface(view->renderer, score_surface);
            SDL_FRect score_rect = {
                (float)(center_x - score_surface->w / 2),
                (float)center_y,
                (float)score_surface->w,
                (float)score_surface->h
            };
            SDL_RenderTexture(view->renderer, score_texture, NULL, &score_rect);
            SDL_DestroySurface(score_surface);
            SDL_DestroyTexture(score_texture);
            center_y += score_surface->h + 40;
        }
    } else {
        draw_string_simple(view, highscore_text, center_x - 70, center_y, 255, 255, 0);
        center_y += 40;
    }

    /* Draw Start Prompt */
    if (view->medium_font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* start_surface = render_text_sdl3(view->medium_font, "Press SPACE to Start", white);
        if (start_surface) {
            SDL_Texture* start_texture = SDL_CreateTextureFromSurface(view->renderer, start_surface);
            SDL_FRect start_rect = {
                (float)(center_x - start_surface->w / 2),
                (float)center_y,
                (float)start_surface->w,
                (float)start_surface->h
            };
            SDL_RenderTexture(view->renderer, start_texture, NULL, &start_rect);
            SDL_DestroySurface(start_surface);
            SDL_DestroyTexture(start_texture);
            center_y += start_surface->h + 60;
        }
    } else {
        draw_string_simple(view, "Press SPACE to Start", center_x - 80, center_y, 255, 255, 255);
        center_y += 40;
    }
    
    SDL_FRect controls_bg = {(float)(center_x - 200), (float)center_y, 400.0f, 180.0f};
    SDL_SetRenderDrawColor(view->renderer, 41, 41, 41, 200);
    SDL_RenderFillRect(view->renderer, &controls_bg);
    SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
    SDL_RenderRect(view->renderer, &controls_bg);
    
    const char* controls_lines[] = {
        "CONTROLS:",
        "Left/Right Arrow or A/D: Move",
        "Space: Shoot",
        "P: Pause",
        "R: Restart (when game over)",
        "ESC: Quit"
    };
    
    int line_y = center_y + 10;
    if (view->small_font) {
        SDL_Color white2 = {255, 255, 255, 255};
        for (int i = 0; i < 6; i++) {
            SDL_Surface* line_surface = render_text_sdl3(view->small_font, controls_lines[i], white2);
            if (line_surface) {
                SDL_Texture* line_texture = SDL_CreateTextureFromSurface(view->renderer, line_surface);
                SDL_FRect line_rect = {
                    (float)(center_x - line_surface->w / 2),
                    (float)line_y,
                    (float)line_surface->w,
                    (float)line_surface->h
                };
                SDL_RenderTexture(view->renderer, line_texture, NULL, &line_rect);
                SDL_DestroySurface(line_surface);
                SDL_DestroyTexture(line_texture);
                line_y += line_surface->h + 5;
            }
        }
    } else {
         for (int i = 0; i < 6; i++) {
            draw_string_simple(view, controls_lines[i], center_x - 100, line_y, 255, 255, 255);
            line_y += 15;
         }
    }
}

void sdl_view_render_pause(SDLView* view) {
    if (!view || !view->renderer) return;
    
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 180);
    SDL_FRect overlay = {0.0f, 0.0f, (float)view->width, (float)view->height};
    SDL_RenderFillRect(view->renderer, &overlay);
    
    draw_string_simple(view, "PAUSED", view->width / 2 - 30, view->height / 2 - 20, 255, 255, 255);
    draw_string_simple(view, "Press P to Resume", view->width / 2 - 70, view->height / 2 + 10, 255, 255, 255);
}

void sdl_view_render_game_over(SDLView* view) {
    if (!view || !view->renderer) return;
    
    SDL_SetRenderDrawBlendMode(view->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 180);
    SDL_FRect overlay = {0.0f, 0.0f, (float)view->width, (float)view->height};
    SDL_RenderFillRect(view->renderer, &overlay);
    
    int center_x = view->width / 2;
    int center_y = view->height / 2 - 50;
    
    if (view->big_font) {
        SDL_Color red = {255, 0, 0, 255};
        SDL_Surface* gameover_surface = render_text_sdl3(view->big_font, "GAME OVER", red);
        if (gameover_surface) {
            SDL_Texture* gameover_texture = SDL_CreateTextureFromSurface(view->renderer, gameover_surface);
            SDL_FRect gameover_rect = {
                (float)(center_x - gameover_surface->w / 2),
                (float)center_y,
                (float)gameover_surface->w,
                (float)gameover_surface->h
            };
            SDL_RenderTexture(view->renderer, gameover_texture, NULL, &gameover_rect);
            SDL_DestroySurface(gameover_surface);
            SDL_DestroyTexture(gameover_texture);
            center_y += gameover_surface->h + 30;
        }
    } else {
        draw_string_simple(view, "GAME OVER", center_x - 45, center_y, 255, 0, 0);
        center_y += 30;
    }

    if (view->medium_font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};

        SDL_Surface* restart_surface = render_text_sdl3(view->medium_font, "Press R to Restart", white);
        if (restart_surface) {
            SDL_Texture* restart_texture = SDL_CreateTextureFromSurface(view->renderer, restart_surface);
            SDL_FRect restart_rect = {
                (float)(center_x - restart_surface->w / 2),
                (float)center_y,
                (float)restart_surface->w,
                (float)restart_surface->h
            };
            SDL_RenderTexture(view->renderer, restart_texture, NULL, &restart_rect);
            SDL_DestroySurface(restart_surface);
            SDL_DestroyTexture(restart_texture);
            center_y += restart_surface->h + 20;
        }
        
        SDL_Surface* quit_surface = render_text_sdl3(view->medium_font, "ESC to Quit", yellow);
        if (quit_surface) {
            SDL_Texture* quit_texture = SDL_CreateTextureFromSurface(view->renderer, quit_surface);
            SDL_FRect quit_rect = {
                (float)(center_x - quit_surface->w / 2),
                (float)center_y,
                (float)quit_surface->w,
                (float)quit_surface->h
            };
            SDL_RenderTexture(view->renderer, quit_texture, NULL, &quit_rect);
            SDL_DestroySurface(quit_surface);
            SDL_DestroyTexture(quit_texture);
        }
    } else {
        draw_string_simple(view, "Press R to Restart", center_x - 70, center_y, 255, 255, 255);
        center_y += 20;
        draw_string_simple(view, "ESC to Quit", center_x - 50, center_y, 255, 255, 255);
    }
}

bool sdl_view_poll_event(SDLView* view, SDL_Event* event) {
    (void)view;
    if (!event) return false;
    return SDL_PollEvent(event) != 0;
}

void sdl_view_draw_rect(SDLView* view, int x, int y, int w, int h, 
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!view || !view->renderer) return;
    
    SDL_FRect rect = {(float)x, (float)y, (float)w, (float)h};
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