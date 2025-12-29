#include "game_state.h"
#include <stdlib.h>
#include <time.h>

/* Simple timer using standard C */
static uint32_t get_ticks(void) {
    return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

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
    context->last_update = get_ticks();
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
    uint32_t current_time = get_ticks();
    float delta_time = (current_time - context->last_update) / 1000.0f;
    context->last_update = current_time;
    
    /* Update model based on current state */
    switch (context->model->state) {
        case STATE_PLAYING:
            model_update(context->model, delta_time * context->time_scale);
            break;
        // STATE_LEVEL_TRANSITION is now handled by Controller input, not timer
        default:
            break;
    }
}

void game_context_render(GameContext* context) {
    if (context->model->needs_redraw) {
        context->model->needs_redraw = false;
    }
}

void transition_to_state(GameContext* context, GameState new_state) {
    context->model->state = new_state;
    context->model->game_time = get_ticks();
    context->model->needs_redraw = true;
}