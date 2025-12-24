#include "game_state.h"
#include <stdlib.h>
#include <SDL2/SDL.h>

GameContext* game_context_create(void) {
    GameContext* context = malloc(sizeof(GameContext));
    if (!context) return NULL;
    
    context->model = malloc(sizeof(GameModel));
    if (!context->model) {
        free(context);
        return NULL;
    }
    
    model_init(context->model);
    
    context->view_data = NULL;
    context->controller_data = NULL;
    context->running = true;
    context->last_update = SDL_GetTicks();
    context->time_scale = 1.0f;
    
    return context;
}

void game_context_destroy(GameContext* context) {
    if (context) {
        if (context->model) {
            free(context->model);
        }
        free(context);
    }
}

void game_context_update(GameContext* context) {
    Uint32 current_time = SDL_GetTicks();
    float delta_time = (current_time - context->last_update) / 1000.0f;
    context->last_update = current_time;
    
    // Update model based on current state
    switch (context->model->state) {
        case STATE_PLAYING:
            model_update(context->model, delta_time * context->time_scale);
            break;
        case STATE_LEVEL_TRANSITION:
            // Wait 2 seconds before next level
            if (current_time > context->model->game_time + 2000) {
                context->model->state = STATE_PLAYING;
                context->model->game_time = current_time;
                context->model->needs_redraw = true;
            }
            break;
        default:
            break;
    }
}

void game_context_render(GameContext* context) {
    // This function just sets the redraw flag
    // Actual rendering is done by the view
    if (context->model->needs_redraw) {
        context->model->needs_redraw = false;
    }
}

void transition_to_state(GameContext* context, GameState new_state) {
    context->model->state = new_state;
    context->model->game_time = SDL_GetTicks();
    context->model->needs_redraw = true;
}