#include "test_utils.h"
#include "mock_platform.h"
#include "../core/model.h"
#include <string.h>
#include <stdio.h>

const char* TEST_HIGHSCORE_FILENAME = "test_highscore.dat";

bool test_model_init(void) {
    GameModel model;
    
    // Initialize model
    model_init(&model);
    
    // Test initial values
    TEST_ASSERT_EQ(model.player.lives, 3);
    TEST_ASSERT_EQ(model.player.score, 0);
    TEST_ASSERT_EQ(model.player.level, 1);
    TEST_ASSERT_EQ(model.state, STATE_MENU);
    
    // Test player position
    TEST_ASSERT_EQ(model.player.hitbox.x, (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2));
    TEST_ASSERT_EQ(model.player.hitbox.y, SCREEN_HEIGHT - (PLAYER_HEIGHT + 20));
    
    // Test all invaders are alive
    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            TEST_ASSERT(model.invaders.invaders[i][j].alive == true);
        }
    }
    
    // Test bases
    for (int i = 0; i < BASE_COUNT; i++) {
        TEST_ASSERT(model.bases[i].alive == true);
        TEST_ASSERT_EQ(model.bases[i].health, 100);
    }
    
    // Test bullets are not alive
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        TEST_ASSERT(model.player_bullets[i].alive == false);
    }
    
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        TEST_ASSERT(model.enemy_bullets[i].alive == false);
    }
    
    return true;
}

bool test_model_player_movement(void) {
    GameModel model;
    model_init(&model);
    model.state = STATE_PLAYING;
    
    int initial_x = model.player.hitbox.x;
    
    // Move left
    model_move_player(&model, DIR_LEFT);
    TEST_ASSERT_LT(model.player.hitbox.x, initial_x);
    
    // Move right
    int new_x = model.player.hitbox.x;
    model_move_player(&model, DIR_RIGHT);
    TEST_ASSERT_GT(model.player.hitbox.x, new_x);
    
    // Test boundary - left
    model.player.hitbox.x = 0;
    model_move_player(&model, DIR_LEFT);
    TEST_ASSERT_EQ(model.player.hitbox.x, 0); // Should not go below 0
    
    // Test boundary - right
    model.player.hitbox.x = GAME_AREA_WIDTH - PLAYER_WIDTH;
    model_move_player(&model, DIR_RIGHT);
    TEST_ASSERT_EQ(model.player.hitbox.x, GAME_AREA_WIDTH - PLAYER_WIDTH);
    
    return true;
}

bool test_model_shooting(void) {
    GameModel model;
    model_init(&model);
    model.state = STATE_PLAYING;
    
    unsigned int initial_shots = model.player.shots_fired;
    
    // Fire first bullet
    model_player_shoot(&model);
    TEST_ASSERT_EQ(model.player.shots_fired, initial_shots + 1);
    
    // Check one bullet is alive
    bool bullet_found = false;
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model.player_bullets[i].alive) {
            bullet_found = true;
            TEST_ASSERT_EQ(model.player_bullets[i].hitbox.x, 
                          model.player.hitbox.x + (PLAYER_WIDTH/2) - (BULLET_WIDTH/2));
            TEST_ASSERT_EQ(model.player_bullets[i].hitbox.y, model.player.hitbox.y);
            break;
        }
    }
    TEST_ASSERT(bullet_found);
    
    // Fire all bullets
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        model_player_shoot(&model);
    }
    
    // All bullets should be alive now
    int alive_bullets = 0;
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model.player_bullets[i].alive) alive_bullets++;
    }
    TEST_ASSERT_EQ(alive_bullets, PLAYER_BULLETS);
    
    return true;
}

bool test_model_collisions(void) {
    // Test collision detection
    Rect rect1 = {0, 0, 10, 10};
    Rect rect2 = {5, 5, 10, 10};
    Rect rect3 = {20, 20, 10, 10};
    
    TEST_ASSERT(model_check_collision(rect1, rect2) == true);
    TEST_ASSERT(model_check_collision(rect1, rect3) == false);
    
    // Test non-overlapping
    Rect rect4 = {0, 0, 5, 5};
    Rect rect5 = {10, 10, 5, 5};
    TEST_ASSERT(model_check_collision(rect4, rect5) == false);
    
    // Test edge touching
    Rect rect6 = {0, 0, 5, 5};
    Rect rect7 = {5, 0, 5, 5};
    TEST_ASSERT(model_check_collision(rect6, rect7) == false); // Touching but not overlapping
    
    return true;
}

bool test_model_level_transition(void) {
    GameModel model;
    model_init(&model);
    model.state = STATE_PLAYING;
    
    // Test level 1 to 2 transition
    int initial_level = model.player.level;
    model_next_level(&model);
    
    TEST_ASSERT_EQ(model.player.level, initial_level + 1);
    TEST_ASSERT_EQ(model.state, STATE_LEVEL_TRANSITION);
    
    // Test player position reset
    TEST_ASSERT_EQ(model.player.hitbox.x, (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2));
    TEST_ASSERT_EQ(model.player.hitbox.y, SCREEN_HEIGHT - (PLAYER_HEIGHT + 20));
    
    // Test bullets are cleared
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        TEST_ASSERT(model.player_bullets[i].alive == false);
    }
    
    return true;
}