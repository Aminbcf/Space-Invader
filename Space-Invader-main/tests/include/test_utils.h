#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Test assertion macros
#define TEST_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #expr); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(a, b) TEST_ASSERT((a) == (b))
#define TEST_ASSERT_NE(a, b) TEST_ASSERT((a) != (b))
#define TEST_ASSERT_LT(a, b) TEST_ASSERT((a) < (b))
#define TEST_ASSERT_LE(a, b) TEST_ASSERT((a) <= (b))
#define TEST_ASSERT_GT(a, b) TEST_ASSERT((a) > (b))
#define TEST_ASSERT_GE(a, b) TEST_ASSERT((a) >= (b))
#define TEST_ASSERT_STR_EQ(a, b) TEST_ASSERT(strcmp((a), (b)) == 0)
#define TEST_ASSERT_STR_NE(a, b) TEST_ASSERT(strcmp((a), (b)) != 0)

// Test function type
typedef bool (*test_func_t)(void);

// Test suite structure
typedef struct {
    const char* name;
    test_func_t function;
} test_case_t;

// Test runner
int run_test_suite(const char* suite_name, test_case_t* tests, int num_tests);

// Helper functions
void create_test_file(const char* filename, const void* data, size_t size);
void remove_test_file(const char* filename);

// Test fixtures
extern const char* TEST_HIGHSCORE_FILENAME;

#endif // TEST_UTILS_H