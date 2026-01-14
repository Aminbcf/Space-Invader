#include "test_utils.h"
#include "../controller/input_handler.h"
#include <string.h>

bool test_input_handler_creation(void) {
    InputHandler* handler = input_handler_create();
    TEST_ASSERT(handler != NULL);
    
    // Test default state
    TEST_ASSERT(handler->joystick_count == 0);
    TEST_ASSERT(handler->joystick_states == NULL);
    TEST_ASSERT_EQ(handler->keyboard_state.initial_delay, 300);
    TEST_ASSERT_EQ(handler->keyboard_state.repeat_delay, 50);
    
    input_handler_destroy(handler);
    return true;
}

bool test_input_handler_keybindings(void) {
    InputHandler* handler = input_handler_create();
    TEST_ASSERT(handler != NULL);
    
    // Test default keybindings (should be set in create)
    // These depend on USE_SDL_VIEW definition
    
    // Test setting custom keybindings
#ifdef USE_SDL_VIEW
    input_handler_set_keybindings(handler, 
                                  SDLK_a, SDLK_d, 
                                  SDLK_SPACE, SDLK_p, 
                                  SDLK_ESCAPE);
#else
    input_handler_set_keybindings(handler, 
                                  'a', 'd', 
                                  ' ', 'p', 
                                  27);
#endif
    
    // Test processing input (simulate ncurses input)
    input_handler_process_ncurses_input(handler, 'a');
    TEST_ASSERT(handler->keyboard_state.left_pressed == true);
    
    input_handler_process_ncurses_input(handler, 'd');
    TEST_ASSERT(handler->keyboard_state.right_pressed == true);
    
    input_handler_process_ncurses_input(handler, ' ');
    TEST_ASSERT(handler->keyboard_state.shoot_pressed == true);
    
    input_handler_process_ncurses_input(handler, 'p');
    TEST_ASSERT(handler->keyboard_state.pause_pressed == true);
    
    input_handler_process_ncurses_input(handler, 'q');
    TEST_ASSERT(handler->keyboard_state.quit_pressed == true);
    
    input_handler_destroy(handler);
    return true;
}