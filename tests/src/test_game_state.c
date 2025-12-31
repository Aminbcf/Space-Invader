#include "test_utils.h"
#include "mock_platform.h"
#include "../core/game_state.h"
#include <string.h>

bool test_game_state_creation(void) {
    GameContext* context = game_context_create();
    TEST_ASSERT(context != NULL);
    TEST_ASSERT(context->model != NULL);
    TEST_ASSERT(context->running == true);
    TEST_ASSERT_EQ(context->time_scale, 1.0f);
    
    // Test model initialization via context
    TEST_ASSERT_EQ(context->model->player.lives, 3);
    TEST_ASSERT_EQ(context->model->player.score, 0);
    TEST_ASSERT_EQ(context->model->player.level, 1);
    
    game_context_destroy(context);
    return true;
}

bool test_game_state_transitions(void) {
    GameContext* context = game_context_create();
    TEST_ASSERT(context != NULL);
    
    // Test initial state is MENU
    TEST_ASSERT_EQ(context->model->state, STATE_MENU);
    
    // Test transition to PLAYING
    transition_to_state(context, STATE_PLAYING);
    TEST_ASSERT_EQ(context->model->state, STATE_PLAYING);
    TEST_ASSERT(context->model->needs_redraw == true);
    
    // Test transition to PAUSED
    transition_to_state(context, STATE_PAUSED);
    TEST_ASSERT_EQ(context->model->state, STATE_PAUSED);
    
    // Test transition to GAME_OVER
    transition_to_state(context, STATE_GAME_OVER);
    TEST_ASSERT_EQ(context->model->state, STATE_GAME_OVER);
    
    // Test transition to WIN
    transition_to_state(context, STATE_WIN);
    TEST_ASSERT_EQ(context->model->state, STATE_WIN);
    
    // Test transition to LEVEL_TRANSITION
    transition_to_state(context, STATE_LEVEL_TRANSITION);
    TEST_ASSERT_EQ(context->model->state, STATE_LEVEL_TRANSITION);
    
    game_context_destroy(context);
    return true;
}