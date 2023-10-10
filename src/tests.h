#ifndef TESTS_H
#define TESTS_H

/*
 * This is my mini unittesting framework
 * @author wittenbb (aka gr3yknigh1)
 * @url <https://github.com/gr3yknigh1/cboxes/blob/master/tests/ccheck.h>
 * */

#include <stdio.h>
#include <stdlib.h>

typedef void (*cc_proc_t)(void);

typedef enum {
    CC_NONE,
    CC_ERROR,
    CC_PASS,
} cc_test_case_status_t;

typedef struct {
    const char *expr;
    const char *file_path;
    unsigned long line_number;
} cc_assert_expr_info_t;

typedef struct {
    cc_proc_t init;
    cc_proc_t fini;

    const char *_name;
    cc_proc_t _run;
    cc_test_case_status_t _status;
    cc_assert_expr_info_t _last_expr_info;
} cc_test_case_t;

typedef struct {
    cc_proc_t init;
    cc_proc_t fini;

    const char *_name;
    cc_test_case_t *_cur_case;
} cc_test_suite_t;

static cc_test_suite_t CC_TEST_SUITE;

#define CC_SUITE CC_TEST_SUITE
#define CC_CUR_CASE CC_TEST_SUITE._cur_case
#define CC_CUR_EXPR_INFO CC_TEST_SUITE._cur_case->_last_expr_info
#define CC_CUR_STATUS CC_TEST_SUITE._cur_case->_status

#define CC_CALL_PROC_IF_NOT_NULL(PROC) \
    if (PROC != NULL) {                \
        PROC();                        \
    }

#define CC_TEST_CASE_MAX_COUNT 128
static cc_test_case_t *CC_TEST_CASES[CC_TEST_CASE_MAX_COUNT];
static unsigned long CC_TEST_CASE_COUNT = 0;

#define CC_PRE_MAIN_PROC(NAME)                    \
    void NAME(void) __attribute__((constructor)); \
    void NAME(void)

#define CC_TEST_RUN_NAME(__TEST_NAME) _##__TEST_NAME##_run
#define CC_TEST_STRUCT_NAME(__TEST_NAME) _##__TEST_NAME

#define CC_TEST_STRUCT(__TEST_NAME) static cc_test_case_t CC_TEST_STRUCT_NAME(__TEST_NAME) =
#define CC_TEST_STRUCT_INIT(__TEST_NAME)                                       \
    CC_PRE_MAIN_PROC(__TEST_NAME) {                                            \
        CC_TEST_CASES[CC_TEST_CASE_COUNT] = &CC_TEST_STRUCT_NAME(__TEST_NAME); \
        CC_TEST_CASE_COUNT++;                                                  \
    }

#define TEST(TEST_NAME)                      \
    void CC_TEST_RUN_NAME(TEST_NAME)(void);  \
    CC_TEST_STRUCT(TEST_NAME){               \
        .init = NULL,                        \
        .fini = NULL,                        \
        ._name = #TEST_NAME,                 \
        ._run = CC_TEST_RUN_NAME(TEST_NAME), \
        ._status = CC_NONE,                  \
    };                                       \
    CC_TEST_STRUCT_INIT(TEST_NAME)           \
    void CC_TEST_RUN_NAME(TEST_NAME)(void)

#define TEST_PACK(PACK_NAME)                                                                             \
    int main(void) {                                                                                     \
        int return_code = EXIT_SUCCESS;                                                                  \
        for (unsigned long i = 0; i < CC_TEST_CASE_COUNT; ++i) {                                         \
            CC_CUR_CASE = CC_TEST_CASES[i];                                                              \
            CC_CALL_PROC_IF_NOT_NULL(CC_CUR_CASE->init);                                                 \
            CC_CUR_CASE->_run();                                                                         \
            if (CC_CUR_CASE->_status == CC_ERROR) {                                                      \
                printf("[%s::%s]: FAILED: '%s' in %s:%lu\n", #PACK_NAME, CC_CUR_CASE->_name,             \
                       CC_CUR_EXPR_INFO.expr, CC_CUR_EXPR_INFO.file_path, CC_CUR_EXPR_INFO.line_number); \
                return_code = EXIT_FAILURE;                                                              \
            }                                                                                            \
            if (CC_CUR_STATUS == CC_PASS) {                                                              \
                printf("[%s::%s]: PASS\n", #PACK_NAME, CC_CUR_CASE->_name);                              \
            }                                                                                            \
            if (CC_CUR_STATUS == CC_NONE) {                                                              \
                printf("[%s::%s]: NONE\n", #PACK_NAME, CC_CUR_CASE->_name);                              \
            }                                                                                            \
            CC_CALL_PROC_IF_NOT_NULL(CC_CUR_CASE->fini);                                                 \
        }                                                                                                \
        return return_code;                                                                              \
    }                                                                                                    \
    static cc_test_suite_t CC_SUITE = {._name = #PACK_NAME}

#define CC_SET_EXPR_INFO(__OUT_PTR, __EXPR)                              \
    *(__OUT_PTR) = (cc_assert_expr_info_t) {                             \
        .expr = #__EXPR, .file_path = __FILE__, .line_number = __LINE__, \
    }

#define CC_VALIDATE_EXPR(__OUT_STATUS, __EXPR) \
    if ((__EXPR) == 0) {                       \
        *(__OUT_STATUS) = CC_ERROR;            \
    } else {                                   \
        *(__OUT_STATUS) = CC_PASS;             \
    }

#define CC_ASSERT(__EXPR)                            \
    do {                                             \
        CC_SET_EXPR_INFO(&CC_CUR_EXPR_INFO, __EXPR); \
        CC_VALIDATE_EXPR(&CC_CUR_STATUS, __EXPR);    \
    } while (0)

#endif  // TESTS_H