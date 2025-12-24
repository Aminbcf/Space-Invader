#ifndef VIEW_BASE_H
#define VIEW_BASE_H

#include <stdint.h>
#include <stdbool.h>
#include "../core/model.h"

// Interface de base pour toutes les vues
typedef struct {
    // Initialisation/Destruction
    bool (*init)(void* view, int width, int height);
    void (*destroy)(void* view);
    
    // Rendering
    void (*render)(void* view, const GameModel* model);
    void (*clear)(void* view);
    void (*present)(void* view);
    
    // Gestion des événements
    bool (*poll_event)(void* view, void* event);
    
    // Gestion des ressources
    bool (*load_texture)(void* view, const char* filename, int* texture_id);
    bool (*load_font)(void* view, const char* filename, int size);
    
    // Utilitaires de rendu
    void (*draw_rect)(void* view, int x, int y, int w, int h, 
                      uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void (*draw_text)(void* view, const char* text, int x, int y,
                      uint8_t r, uint8_t g, uint8_t b);
    
    // Gestion de la fenêtre
    void (*get_size)(void* view, int* width, int* height);
    void (*set_title)(void* view, const char* title);
    
    // Synchronisation
    void (*delay)(void* view, uint32_t ms);
    uint32_t (*get_ticks)(void* view);
} ViewInterface;

// Types de vues disponibles
typedef enum {
    VIEW_TYPE_SDL,
    VIEW_TYPE_NCURSES,
    VIEW_TYPE_NONE
} ViewType;

// Structure pour la configuration de la vue
typedef struct {
    ViewType type;
    int width;
    int height;
    bool fullscreen;
    const char* title;
    int frame_rate;
} ViewConfig;

// Fonctions utilitaires
ViewInterface* view_create(ViewType type);
void view_destroy(ViewInterface* view);

#endif // VIEW_BASE_H