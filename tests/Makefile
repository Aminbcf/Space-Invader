CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11 -I./include -I../include -I../core -I../controller -I../views -I../utils -DTEST_BUILD -DPLATFORM_MOCK
LDFLAGS = -lm

TEST_SOURCES = src/test_main.c src/test_model.c src/test_controller.c src/test_input_handler.c src/test_game_state.c src/mock_platform.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_EXEC = run_tests

.PHONY: all clean test

all: $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -f src/*.o $(TEST_EXEC) test_results.txt

valgrind: $(TEST_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_EXEC)