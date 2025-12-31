#include "test_utils.h"
#include <stdio.h>

// Test declarations
bool test_model_init(void);
bool test_model_player_movement(void);
bool test_model_shooting(void);
bool test_model_collisions(void);
bool test_model_level_transition(void);
bool test_controller_creation(void);
bool test_controller_commands(void);
bool test_input_handler_creation(void);
bool test_input_handler_keybindings(void);
bool test_game_state_creation(void);
bool test_game_state_transitions(void);

// Test suite
test_case_t model_tests[] = {
    {"model_init", test_model_init},
    {"model_player_movement", test_model_player_movement},
    {"model_shooting", test_model_shooting},
    {"model_collisions", test_model_collisions},
    {"model_level_transition", test_model_level_transition},
};

test_case_t controller_tests[] = {
    {"controller_creation", test_controller_creation},
    {"controller_commands", test_controller_commands},
};

test_case_t input_tests[] = {
    {"input_handler_creation", test_input_handler_creation},
    {"input_handler_keybindings", test_input_handler_keybindings},
};

test_case_t game_state_tests[] = {
    {"game_state_creation", test_game_state_creation},
    {"game_state_transitions", test_game_state_transitions},
};

int main(void) {
    int total_failed = 0;
    int total_passed = 0;
    
    printf("Space Invaders MVC - Unit Tests\n");
    printf("================================\n\n");
    
    // Run model tests
    printf("=== Model Tests ===\n");
    total_failed += run_test_suite("Model", model_tests, 
                                   sizeof(model_tests) / sizeof(test_case_t));
    total_passed += sizeof(model_tests) / sizeof(test_case_t) - total_failed;
    
    // Run controller tests
    printf("\n=== Controller Tests ===\n");
    int controller_failed = run_test_suite("Controller", controller_tests, 
                                         sizeof(controller_tests) / sizeof(test_case_t));
    total_failed += controller_failed;
    total_passed += sizeof(controller_tests) / sizeof(test_case_t) - controller_failed;
    
    // Run input handler tests
    printf("\n=== Input Handler Tests ===\n");
    int input_failed = run_test_suite("Input Handler", input_tests, 
                                    sizeof(input_tests) / sizeof(test_case_t));
    total_failed += input_failed;
    total_passed += sizeof(input_tests) / sizeof(test_case_t) - input_failed;
    
    // Run game state tests
    printf("\n=== Game State Tests ===\n");
    int state_failed = run_test_suite("Game State", game_state_tests, 
                                    sizeof(game_state_tests) / sizeof(test_case_t));
    total_failed += state_failed;
    total_passed += sizeof(game_state_tests) / sizeof(test_case_t) - state_failed;
    
    // Summary
    printf("\n=== Test Summary ===\n");
    printf("Total Tests: %d\n", total_passed + total_failed);
    printf("Passed: %d\n", total_passed);
    printf("Failed: %d\n", total_failed);
    
    return total_failed > 0 ? 1 : 0;
}

// Test runner implementation
int run_test_suite(const char* suite_name, test_case_t* tests, int num_tests) {
    int failed = 0;
    
    for (int i = 0; i < num_tests; i++) {
        printf("  %-30s ", tests[i].name);
        if (tests[i].function()) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL]\n");
            failed++;
        }
    }
    
    return failed;
}