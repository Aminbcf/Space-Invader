// At the top of view_sdl.c, add:
#include <SDL2/SDL_ttf.h>
#include "font_manager.h"
#include "view_sdl.h"
#include "rect_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL_image.h> 


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
    // REMOVED: view->title_screen = NULL;
    view->cmap = NULL;
    view->invadersmap = NULL;
    view->player_img = NULL;
    view->saucer_img = NULL;
    for (int i = 0; i < 4; i++) view->base_img[i] = NULL;
    view->damage_img = NULL;
    view->damage_top_img = NULL;
    // REMOVED: view->game_over_img = NULL;
    
    view->screen_texture = NULL;
    // REMOVED: view->title_texture = NULL;
    view->font_texture = NULL;
    view->invaders_texture = NULL;
    view->player_texture = NULL;
    view->saucer_texture = NULL;
    for (int i = 0; i < 4; i++) view->base_textures[i] = NULL;
    view->damage_texture = NULL;
    view->damage_top_texture = NULL;
    // REMOVED: view->game_over_texture = NULL;
    
    // Font pointers
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
    
    /* Free textures */
    if (view->screen_texture) {
        SDL_DestroyTexture(view->screen_texture);
        view->screen_texture = NULL;
    }
    // REMOVED: title_texture cleanup
    if (view->font_texture) {
        SDL_DestroyTexture(view->font_texture);
        view->font_texture = NULL;
    }
    if (view->invaders_texture) {
        SDL_DestroyTexture(view->invaders_texture);
        view->invaders_texture = NULL;
    }
    if (view->player_texture) {
        SDL_DestroyTexture(view->player_texture);
        view->player_texture = NULL;
    }
    if (view->saucer_texture) {
        SDL_DestroyTexture(view->saucer_texture);
        view->saucer_texture = NULL;
    }
    for (int i = 0; i < 4; i++) {
        if (view->base_textures[i]) {
            SDL_DestroyTexture(view->base_textures[i]);
            view->base_textures[i] = NULL;
        }
    }
    if (view->damage_texture) {
        SDL_DestroyTexture(view->damage_texture);
        view->damage_texture = NULL;
    }
    if (view->damage_top_texture) {
        SDL_DestroyTexture(view->damage_top_texture);
        view->damage_top_texture = NULL;
    }
    // REMOVED: game_over_texture cleanup
    
    /* Free surfaces */
    if (view->screen_surface) {
        SDL_FreeSurface(view->screen_surface);
        view->screen_surface = NULL;
    }
    // REMOVED: title_screen cleanup
    if (view->cmap) {
        SDL_FreeSurface(view->cmap);
        view->cmap = NULL;
    }
    if (view->invadersmap) {
        SDL_FreeSurface(view->invadersmap);
        view->invadersmap = NULL;
    }
    if (view->player_img) {
        SDL_FreeSurface(view->player_img);
        view->player_img = NULL;
    }
    if (view->saucer_img) {
        SDL_FreeSurface(view->saucer_img);
        view->saucer_img = NULL;
    }
    for (int i = 0; i < 4; i++) {
        if (view->base_img[i]) {
            SDL_FreeSurface(view->base_img[i]);
            view->base_img[i] = NULL;
        }
    }
    if (view->damage_img) {
        SDL_FreeSurface(view->damage_img);
        view->damage_img = NULL;
    }
    if (view->damage_top_img) {
        SDL_FreeSurface(view->damage_top_img);
        view->damage_top_img = NULL;
    }
    // REMOVED: game_over_img cleanup
    
    /* Free fonts */
    if (view->big_font) TTF_CloseFont(view->big_font);
    if (view->medium_font) TTF_CloseFont(view->medium_font);
    if (view->small_font) TTF_CloseFont(view->small_font);
    
    /* Free SDL objects */
    if (view->renderer) {
        SDL_DestroyRenderer(view->renderer);
        view->renderer = NULL;
    }
    if (view->window) {
        SDL_DestroyWindow(view->window);
        view->window = NULL;
    }
    
    /* Quit SDL subsystems */
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
    free(view);
}

bool sdl_view_init(SDLView* view, int width, int height) {
    if (!view) return false;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return false;
    }

      // Initialize SDL_ttf for font support
    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
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
    
    (void)view; //masked the warning
    // Check file extension
    const char* ext = strrchr(filename, '.');
    SDL_Surface* temp = NULL;
    
    if (ext && (strcmp(ext, ".png") == 0 || strcmp(ext, ".PNG") == 0)) {
        // Use IMG_Load for PNG files
        temp = IMG_Load(filename);
        if (temp == NULL) {
            fprintf(stderr, "IMG_Load Error for %s: %s\n", filename, IMG_GetError());
            return false;
        }
    } else {
        // Fall back to SDL_LoadBMP for BMP files
        temp = SDL_LoadBMP(filename);
        if (temp == NULL) {
            fprintf(stderr, "SDL_LoadBMP Error for %s: %s\n", filename, SDL_GetError());
            return false;
        }
    }
    
    // Set color key for transparency (magenta: 255, 0, 255)
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, r, g, b));
    
    // Convert to consistent format
    *surface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(temp);  // Free the temp surface
    
    if (*surface == NULL) {
        fprintf(stderr, "Warning: Unable to convert surface: %s\n", SDL_GetError());
        return false;
    }
    
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
    // Try to load the TTF font from the provided folder
    // Check which font files actually exist in your fonts/venite-adoremus-font folder
    const char* font_paths[] = {
        "fonts/venite-adoremus-font/VeniteAdoremus-rgRBA.ttf",
        "fonts/venite-adoremus-font/VeniteAdoremusStraight-Yzo6v.ttf",
        "fonts/venite-adoremus-font/Aa VeniteAdoremus-rgRBA.ttf",
        "fonts/venite-adoremus-font/Aa VeniteAdoremusStraight-Yzo6v.ttf",
        NULL
    };
    
    // Try each font path
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
        view->big_font = TTF_OpenFont(successful_path, 62);
        if (view->big_font) {
            // Successfully loaded, try loading smaller sizes
            view->medium_font = TTF_OpenFont(successful_path, 26);
            view->small_font = TTF_OpenFont(successful_path, 14);
            printf("Loaded font: %s\n", successful_path);
            return true;
        }
    }
    
    // Fallback to default SDL font if custom font not found
    fprintf(stderr, "Custom font not found, using fallback font\n");
    
    // Try to load a default system font as fallback
    const char* system_fonts[] = {
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        NULL
    };
    
    for (int i = 0; system_fonts[i] != NULL; i++) {
        view->big_font = TTF_OpenFont(system_fonts[i], 72);
        if (view->big_font) {
            view->medium_font = TTF_OpenFont(system_fonts[i], 36);
            view->small_font = TTF_OpenFont(system_fonts[i], 24);
            printf("Using system font: %s\n", system_fonts[i]);
            return true;
        }
    }
    
    return false;
}

bool sdl_view_load_resources(SDLView* view) {
    bool all_loaded = true;
    
    // Load fonts first
    if (!sdl_view_load_fonts(view)) {
        fprintf(stderr, "Warning: Could not load custom fonts\n");
    }
    
    /* Font map is optional */
    if (sdl_view_load_image(view, "assets/font.png", &view->cmap, 255, 0, 255)) {
        sdl_view_convert_surface_to_texture(view, view->cmap, &view->font_texture);
    } else {
        fprintf(stderr, "Note: Using fallback text rendering\n");
    }
    
    if (sdl_view_load_image(view, "assets/invaders.png", &view->invadersmap, 255, 0, 255)) {
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
        printf("Loaded saucer texture\n");
    } else {
        printf("Note: Saucer image not found, using fallback rectangle\n");
        all_loaded = false;
    }
    
    // REMOVED: titlescreen.png and gameover.png loading
    
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
// In view_sdl.c, add a debug function:
void sdl_view_draw_debug(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
    char debug_text[100];
    snprintf(debug_text, sizeof(debug_text), 
             "Shots: %d, Saucer alive: %d, X: %d, Y: %d", 
             model->player.shots_fired,
             model->saucer.alive,
             model->saucer.hitbox.x,
             model->saucer.hitbox.y);
    
    // Draw in top-left corner
    if (view->small_font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* debug_surface = TTF_RenderText_Blended(view->small_font, debug_text, white);
        if (debug_surface) {
            SDL_Texture* debug_texture = SDL_CreateTextureFromSurface(view->renderer, debug_surface);
            SDL_Rect debug_rect = {10, 10, debug_surface->w, debug_surface->h};
            SDL_RenderCopy(view->renderer, debug_texture, NULL, &debug_rect);
            SDL_FreeSurface(debug_surface);
            SDL_DestroyTexture(debug_texture);
        }
    } else {
        draw_string_simple(view, debug_text, 10, 10, 255, 255, 255);
    }
}



void sdl_view_render_game(SDLView* view, const GameModel* model) {
    if (!view || !model || !view->renderer) return;
    
   sdl_view_draw_background(view);
    //sdl_view_draw_bases(view, model);
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
    
    int center_x = view->width / 2;
    int center_y = 100; // Start position
    
    // Render "SPACE INVADERS" in big font
    if (view->big_font) {
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};
        SDL_Color cyan = {0, 255, 255, 255};
        
        // "SPACE INVADERS" - big text in center
        SDL_Surface* title_surface = TTF_RenderText_Blended(view->big_font, "SPACE INVADERS", white);
        if (title_surface) {
            SDL_Texture* title_texture = SDL_CreateTextureFromSurface(view->renderer, title_surface);
            SDL_Rect title_rect = {
                center_x - title_surface->w / 2,
                center_y,
                title_surface->w,
                title_surface->h
            };
            SDL_RenderCopy(view->renderer, title_texture, NULL, &title_rect);
            SDL_FreeSurface(title_surface);
            SDL_DestroyTexture(title_texture);
            center_y += title_surface->h + 30;
        }
        
        // "by amine" - smaller text
        if (view->medium_font) {
            SDL_Surface* author_surface = TTF_RenderText_Blended(view->medium_font, "by Amine Boucif", cyan);
            if (author_surface) {
                SDL_Texture* author_texture = SDL_CreateTextureFromSurface(view->renderer, author_surface);
                SDL_Rect author_rect = {
                    center_x - author_surface->w / 2,
                    center_y,
                    author_surface->w,
                    author_surface->h
                };
                SDL_RenderCopy(view->renderer, author_texture, NULL, &author_rect);
                SDL_FreeSurface(author_surface);
                SDL_DestroyTexture(author_texture);
                center_y += author_surface->h + 40;
            }
        }
        
        // High score in yellow
        if (view->medium_font) {
            char highscore_text[50];
            snprintf(highscore_text, sizeof(highscore_text), "HIGH SCORE: %d", model->high_score);
            
            SDL_Surface* score_surface = TTF_RenderText_Blended(view->medium_font, highscore_text, yellow);
            if (score_surface) {
                SDL_Texture* score_texture = SDL_CreateTextureFromSurface(view->renderer, score_surface);
                SDL_Rect score_rect = {
                    center_x - score_surface->w / 2,
                    center_y,
                    score_surface->w,
                    score_surface->h
                };
                SDL_RenderCopy(view->renderer, score_texture, NULL, &score_rect);
                SDL_FreeSurface(score_surface);
                SDL_DestroyTexture(score_texture);
                center_y += score_surface->h + 40;
            }
        }
        
        // "Press SPACE to Start"
        if (view->medium_font) {
            SDL_Surface* start_surface = TTF_RenderText_Blended(view->medium_font, "Press SPACE to Start", white);
            if (start_surface) {
                SDL_Texture* start_texture = SDL_CreateTextureFromSurface(view->renderer, start_surface);
                SDL_Rect start_rect = {
                    center_x - start_surface->w / 2,
                    center_y,
                    start_surface->w,
                    start_surface->h
                };
                SDL_RenderCopy(view->renderer, start_texture, NULL, &start_rect);
                SDL_FreeSurface(start_surface);
                SDL_DestroyTexture(start_texture);
                center_y += start_surface->h + 60;
            }
        }
        
        // Draw controls window
        SDL_Rect controls_bg = {center_x - 200, center_y, 400, 180};
        SDL_SetRenderDrawColor(view->renderer, 41, 41, 41, 200);
        SDL_RenderFillRect(view->renderer, &controls_bg);
        
        // Draw border
        SDL_SetRenderDrawColor(view->renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(view->renderer, &controls_bg);
        
        // Draw controls text
        if (view->small_font) {
            const char* controls_lines[] = {
                "CONTROLS:",
                "Left/Right Arrow or A/D: Move",
                "Space: Shoot",
                "P: Pause",
                "R: Restart (when game over)",
                "ESC: Quit"
            };
            
            int line_y = center_y + 10;
            for (int i = 0; i < 6; i++) {
                SDL_Surface* line_surface = TTF_RenderText_Blended(view->small_font, controls_lines[i], white);
                if (line_surface) {
                    SDL_Texture* line_texture = SDL_CreateTextureFromSurface(view->renderer, line_surface);
                    SDL_Rect line_rect = {
                        center_x - line_surface->w / 2,
                        line_y,
                        line_surface->w,
                        line_surface->h
                    };
                    SDL_RenderCopy(view->renderer, line_texture, NULL, &line_rect);
                    SDL_FreeSurface(line_surface);
                    SDL_DestroyTexture(line_texture);
                    line_y += line_surface->h + 5;
                }
            }
        }
    } else {
        // Fallback to simple text rendering if font not available
        draw_string_simple(view, "SPACE INVADERS", center_x - 70, center_y, 255, 255, 255);
        center_y += 40;
        draw_string_simple(view, "by amine", center_x - 40, center_y, 0, 255, 255);
        center_y += 30;
        
        char highscore[32];
        snprintf(highscore, sizeof(highscore), "High Score: %d", model->high_score);
        draw_string_simple(view, highscore, center_x - 70, center_y, 255, 255, 0);
        center_y += 40;
        
        draw_string_simple(view, "Press SPACE to Start", center_x - 80, center_y, 255, 255, 255);
        center_y += 40;
        
        // Controls window
        draw_string_simple(view, "CONTROLS:", center_x - 40, center_y, 255, 255, 255);
        center_y += 20;
        draw_string_simple(view, "Left/Right or A/D: Move", center_x - 100, center_y, 255, 255, 255);
        center_y += 15;
        draw_string_simple(view, "Space: Shoot", center_x - 50, center_y, 255, 255, 255);
        center_y += 15;
        draw_string_simple(view, "P: Pause", center_x - 40, center_y, 255, 255, 255);
        center_y += 15;
        draw_string_simple(view, "R: Restart (game over)", center_x - 90, center_y, 255, 255, 255);
        center_y += 15;
        draw_string_simple(view, "ESC: Quit", center_x - 40, center_y, 255, 255, 255);
    }
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
    
    int center_x = view->width / 2;
    int center_y = view->height / 2 - 50;
    
    // Render "GAME OVER" using custom font
    if (view->big_font) {
        SDL_Color red = {255, 0, 0, 255};
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};
        
        // "GAME OVER" in big red font
        SDL_Surface* gameover_surface = TTF_RenderText_Blended(view->big_font, "GAME OVER", red);
        if (gameover_surface) {
            SDL_Texture* gameover_texture = SDL_CreateTextureFromSurface(view->renderer, gameover_surface);
            SDL_Rect gameover_rect = {
                center_x - gameover_surface->w / 2,
                center_y,
                gameover_surface->w,
                gameover_surface->h
            };
            SDL_RenderCopy(view->renderer, gameover_texture, NULL, &gameover_rect);
            SDL_FreeSurface(gameover_surface);
            SDL_DestroyTexture(gameover_texture);
            center_y += gameover_surface->h + 30;
        }
        
        // Instructions in smaller font
        if (view->medium_font) {
            // "Press R to Restart"
            SDL_Surface* restart_surface = TTF_RenderText_Blended(view->medium_font, "Press R to Restart", white);
            if (restart_surface) {
                SDL_Texture* restart_texture = SDL_CreateTextureFromSurface(view->renderer, restart_surface);
                SDL_Rect restart_rect = {
                    center_x - restart_surface->w / 2,
                    center_y,
                    restart_surface->w,
                    restart_surface->h
                };
                SDL_RenderCopy(view->renderer, restart_texture, NULL, &restart_rect);
                SDL_FreeSurface(restart_surface);
                SDL_DestroyTexture(restart_texture);
                center_y += restart_surface->h + 20;
            }
            
            // "ESC to Quit"
            SDL_Surface* quit_surface = TTF_RenderText_Blended(view->medium_font, "ESC to Quit", yellow);
            if (quit_surface) {
                SDL_Texture* quit_texture = SDL_CreateTextureFromSurface(view->renderer, quit_surface);
                SDL_Rect quit_rect = {
                    center_x - quit_surface->w / 2,
                    center_y,
                    quit_surface->w,
                    quit_surface->h
                };
                SDL_RenderCopy(view->renderer, quit_texture, NULL, &quit_rect);
                SDL_FreeSurface(quit_surface);
                SDL_DestroyTexture(quit_texture);
            }
        }
    } else {
        // Fallback to simple text rendering
        draw_string_simple(view, "GAME OVER", view->width / 2 - 45, view->height / 2 - 20, 255, 0, 0);
        draw_string_simple(view, "Press R to Restart", view->width / 2 - 70, view->height / 2 + 20, 255, 255, 255);
        draw_string_simple(view, "ESC to Quit", view->width / 2 - 50, view->height / 2 + 50, 255, 255, 255);
    }
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