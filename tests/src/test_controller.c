#include "test_utils.h"
#include "mock_platform.h"
#include "../core/model.h"
#include "../controller/controller.h"
#include "../controller/input_handler.h"
#include <string.h>

bool test_controller_creation(void) {
    GameModel model;
    model_init(&model);
    
    // Create controller
    Controller* controller = controller_create(&model);
    TEST_ASSERT(controller != NULL);
    TEST_ASSERT(controller->model == &model);
    TEST_ASSERT(controller->input_handler != NULL);
    TEST_ASSERT(controller->quit_requested == false);
    TEST_ASSERT(controller->paused == false);
    
    // Clean up
    controller_destroy(controller);
    
    return true;
}

bool test_controller_commands(void) {
    GameModel model;
    model_init(&model);
    
    Controller* controller = controller_create(&model);
    TEST_ASSERT(controller != NULL);
    
    // Test START_GAME command from menu
    model.state = STATE_MENU;
    InputEvent event = {0};
    event.type = INPUT_KEYBOARD;
    event.key = ' ';
    
    Command cmd = controller_translate_input(controller, &event);
    TEST_ASSERT_EQ(cmd, CMD_SHOOT); // Space should translate to SHOOT
    
    controller_execute_command(controller, CMD_START_GAME); // To Difficulty Menu
    controller_execute_command(controller, CMD_START_GAME); // To Playing State
    TEST_ASSERT_EQ(model.state, STATE_PLAYING);
    
    // Test movement commands
    controller_execute_command(controller, CMD_MOVE_LEFT);
    TEST_ASSERT_LT(model.players[0].hitbox.x, (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2));
    
    int current_x = model.players[0].hitbox.x;
    controller_execute_command(controller, CMD_MOVE_RIGHT);
    TEST_ASSERT_GT(model.players[0].hitbox.x, current_x);
    
    // Test pause command
    controller_execute_command(controller, CMD_PAUSE);
    TEST_ASSERT_EQ(model.state, STATE_PAUSED);
    
    controller_execute_command(controller, CMD_PAUSE);
    TEST_ASSERT_EQ(model.state, STATE_PLAYING);
    
    // Test quit command
    controller_execute_command(controller, CMD_QUIT);
    TEST_ASSERT(controller_is_quit_requested(controller) == true);
    
    // Test reset command
    model.state = STATE_GAME_OVER;
    model.players[0].lives = 0;
    controller_execute_command(controller, CMD_RESET_GAME);
    TEST_ASSERT_EQ(model.players[0].lives, 3);
    TEST_ASSERT_EQ(model.state, STATE_PLAYING);
    
    controller_destroy(controller);
    return true;
}