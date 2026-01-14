cmake_minimum_required(VERSION 3.10)
project(space_invaders_tests)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../controller
    ${CMAKE_CURRENT_SOURCE_DIR}/../views
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
)

# Source files
set(TEST_SOURCES
    src/test_main.c
    src/test_model.c
    src/test_controller.c
    src/test_input_handler.c
    src/test_game_state.c
    src/mock_platform.c
)

# Mock platform definitions
add_definitions(-DTEST_BUILD)
add_definitions(-DPLATFORM_MOCK)

# Create test executable
add_executable(run_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(run_tests)

# Add test
add_test(NAME space_invaders_tests COMMAND run_tests)