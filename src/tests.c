#include <string.h>

#include "tests.h"
#include "memory.h"
#include "darray.h"
#include "str.h"
#include "log.h"
#include "lexer.h"
#include "rpn.h"
#include "rpn.h"
#include "token.h"
#include "defs.h"

TEST_PACK(GRAPH);

TEST(MEMORY_COPY) {
    size_t size = 10;
    char buf[10] = {0};
    char src[] = "123456789";

    memory_copy(buf, src, size);

    char *buf_it = buf, *src_it = src;

    while (size > 0) {
        CC_ASSERT(*buf_it == *src_it);
        ++buf_it;
        ++src_it;
        --size;
    }
}

TEST(MEMORY_COPY_ONE) {
    size_t size = 1;
    char buf[1] = {0};
    char src[] = "";

    memory_copy(buf, src, size);

    char *buf_it = buf, *src_it = src;

    while (size > 0) {
        CC_ASSERT(*buf_it == *src_it);
        ++buf_it;
        ++src_it;
        --size;
    }
}

TEST(DARRAY_INIT_AND_FREE) {
    darray_t arr = {0};
    darray_init(&arr, 10, sizeof(int));

    CC_ASSERT(arr.item_capacity == 10);
    CC_ASSERT(arr.item_size == sizeof(int));
    CC_ASSERT(arr.item_count == 0);
    CC_ASSERT(arr.items != NULL);

    darray_free(&arr);

    CC_ASSERT(arr.item_size == sizeof(int));
    CC_ASSERT(arr.item_capacity == 0);
    CC_ASSERT(arr.item_count == 0);
    CC_ASSERT(arr.items == NULL);
}

TEST(DARRAY_PUSH_AND_POP) {
    darray_t arr = {0};
    darray_init(&arr, 10, sizeof(int));

    int x = 10;
    darray_push(&arr, &x);

    CC_ASSERT(arr.item_count == 1);

    int out_x = -1;
    darray_pop(&arr, &out_x);

    CC_ASSERT(x == out_x);
    CC_ASSERT(arr.item_count == 0);

    darray_free(&arr);
}

TEST(DARRAY_PEEK) {
    darray_t arr = {0};
    darray_init(&arr, 10, sizeof(int));

    int x = 10;
    darray_push(&arr, &x);

    CC_ASSERT(arr.item_count == 1);

    const int * out_x = NULL;
    darray_peek(&arr, (void const ** const)&out_x);

    CC_ASSERT(x == *out_x);
    CC_ASSERT(arr.item_count == 1);

    darray_free(&arr);
}

TEST(STR_INIT_AND_FREE) {
    const char *s0 = "hello world";
    size_t some_string_size = strlen(s0) + 1;

    str_t str_0 = str_init(s0);
    //    ^^^^^ - must eq whole string
    CC_ASSERT(strcmp(s0, str_0.data) == 0);
    CC_ASSERT(str_0.size == some_string_size);

    str_t str_1 = str_init_0(s0, 5);
    //    ^^^^^ - must eq 'hello'
    CC_ASSERT(strcmp("hello", str_1.data) == 0);
    CC_ASSERT(str_1.size == 6);  // Length + 1 (for '\0' char)

    str_free(&str_0);
    str_free(&str_1);
}

TEST(STR_COMP) {
    const char *s0 = "hello world";
    const char *s1 = "hello mir";

    str_t str_0 = str_init(s0);
    str_t str_1 = str_init_0(s0, 5); // "hello"
    str_t str_2 = str_init(s1);

    CC_ASSERT(!str_is_equal(str_0, str_1));
    CC_ASSERT(str_is_equal(str_0, str_0));
    CC_ASSERT(!str_is_equal(str_0, str_2));

    str_free(&str_0);
    str_free(&str_1);
    str_free(&str_2);
}

TEST(STR_COMP_EMPTY) {
    const char *s0 = "";
    str_t str_0 = str_init(s0);
    str_t str_1 = str_init(s0);

    CC_ASSERT(str_is_equal(str_0, str_1));

    str_free(&str_0);
    str_free(&str_1);
}

TEST(LEXER_PARSE) {
    const char *expr = "234.23 - 32";
    darray_t tokens = {0};
    darray_init(&tokens, 10, sizeof(token_t));

    lexer_status_t lex_ret = lexer_parse(&tokens, expr);

    const token_type_t expected_token_types[] = {
        TOKEN_FLOAT, 
        TOKEN_MINUS, 
        TOKEN_INT,
    };

    str_t expected_token_values_data[] = { 
        str_init("234.23"), 
        str_init("-"), 
        str_init("32"),
    };

    CC_ASSERT(tokens.item_count == 3);

    if (lex_ret == LEXER_OK) {
        for (size_t i = 0; i < tokens.item_count; ++i) {
            const token_t tok = ((token_t *)tokens.items)[i];
            // LOG_DEBUG("TOK: [%s] [%s]", token_type_to_str(tok.type), tok.value.data);
            CC_ASSERT(expected_token_types[i] == tok.type);
            CC_ASSERT(str_is_equal(tok.value, expected_token_values_data[i]));
        }
    } 

    CC_ASSERT(lex_ret == LEXER_OK);

    for (size_t i = 0; i < tokens.item_count; ++i) {
        token_t token = ((token_t *)tokens.items)[i];
        token_free(&token);
    }

    for (size_t i = 0; i < 3; ++i) {
        str_free(expected_token_values_data + i);
    }

    darray_free(&tokens);
}

TEST(LEXER_NEXT_TOKEN) {
    const char *expr_str = "234.23 - 32";

    darray_t tokens = {0};
    darray_init(&tokens, 10, sizeof(token_t));

    char *expr_buf = malloc(strlen(expr_str) + 1);
    str_trim_char(expr_buf, expr_str, ' ');

    const char *expr_it = expr_buf;
    size_t tok_idx = 0;

    const token_type_t expected_token_types[] = {
        TOKEN_FLOAT, 
        TOKEN_MINUS, 
        TOKEN_INT,
    };

    str_t expected_token_values_data[] = { 
        str_init("234.23"), 
        str_init("-"), 
        str_init("32"),
    };

    while (*expr_it != '\0') {
        token_t tok = {0};
        lexer_status_t ret = lexer_next_token(&tokens, &tok, &expr_it, tok_idx);

        // LOG_DEBUG(">>> <Token [%lu] value=[%s] type=%s>", tok_idx, tok.value.data, token_type_to_str(tok.type));

        CC_ASSERT(ret == LEXER_OK);
        CC_ASSERT(tok.type == expected_token_types[tok_idx]);
        CC_ASSERT(str_is_equal(tok.value, expected_token_values_data[tok_idx]));

        darray_push(&tokens, &tok);
        ++tok_idx;
    }

    for (size_t i = 0; i < tokens.item_count; ++i) {
        token_free((token_t *)tokens.items + i);
    }
    darray_free(&tokens);

    for (size_t i = 0; i < 3; ++i) {
        str_free(expected_token_values_data + i);
    }

    free(expr_buf);
}

TEST(STR_SET) {
    str_t str = str_init("Hello world");
    const char *expected_str = "Goodbye world!";

    str_t other = str_init(expected_str);
    str_set(&str, other);
    CC_ASSERT(str_is_equal(str, other));

    str_set_0(&str, expected_str);
    CC_ASSERT(str_is_equal_0(str, expected_str));

    str_free(&str);
    str_free(&other);
}

TEST(RPN_CONVERSION0) {
    token_t infix_expression[] = {
        {TOKEN_VAR,  str_init("x")},
        {TOKEN_MULT, str_init("*")},
        {TOKEN_VAR,  str_init("x")}
    };
    size_t infix_length = sizeof(infix_expression) / sizeof(token_t);

    size_t rpn_length = 0;
    token_t *rpn_expression;
    rpn_expression = infix_to_rpn(infix_expression, infix_length, &rpn_length);

    token_t expected_rpn_expression[] = {
        {TOKEN_VAR,  str_init("x")},
        {TOKEN_VAR,  str_init("x")},
        {TOKEN_MULT, str_init("*")}
    };

    for (size_t i = 0; i < rpn_length; i++) {
        LOG_DEBUG("%s ", rpn_expression[i].value.data);
        CC_ASSERT(rpn_expression[i].type == expected_rpn_expression[i].type);
        CC_ASSERT(str_is_equal(rpn_expression[i].value, expected_rpn_expression[i].value));
    }

    free(rpn_expression);
    tokens_free(infix_expression, infix_length);
    tokens_free(expected_rpn_expression, rpn_length);
}

TEST(RPN_CONVERSION1) {
    token_t infix_expression[] = {
        {TOKEN_VAR,  str_init("x")}
    };
    size_t infix_length = sizeof(infix_expression) / sizeof(token_t);

    size_t rpn_length = 0;
    token_t *rpn_expression = infix_to_rpn(infix_expression, infix_length, &rpn_length);

    token_t expected_rpn_expression[] = {
        {TOKEN_VAR,  str_init("x")},
    };

    for (size_t i = 0; i < rpn_length; i++) {
        LOG_DEBUG("%s ", rpn_expression[i].value.data);
        CC_ASSERT(rpn_expression[i].type == expected_rpn_expression[i].type);
        CC_ASSERT(str_is_equal(rpn_expression[i].value, expected_rpn_expression[i].value));
    }

    free(rpn_expression);
    tokens_free(infix_expression, infix_length);
    tokens_free(expected_rpn_expression, rpn_length);
}

TEST(RPN_CONVERSION2) {
    token_t infix_expression[] = {
        {TOKEN_SIN,    str_init("sin")},
        {TOKEN_LPAREN, str_init("(")},
        {TOKEN_VAR,    str_init("x")},
        {TOKEN_RPAREN, str_init(")")},
        {TOKEN_PLUS,   str_init("+")},
        {TOKEN_COS,    str_init("cos")},
        {TOKEN_LPAREN, str_init("(")},
        {TOKEN_VAR,    str_init("y")},
        {TOKEN_RPAREN, str_init(")")},
    };
    size_t infix_length = sizeof(infix_expression) / sizeof(token_t);

    size_t rpn_length = 0;
    token_t *rpn_expression;
    rpn_expression = infix_to_rpn(infix_expression, infix_length, &rpn_length);

    token_t expected_rpn_expression[] = {
        {TOKEN_VAR, str_init("x")},
        {TOKEN_SIN, str_init("sin")},
        {TOKEN_VAR, str_init("y")},
        {TOKEN_COS, str_init("cos")},
        {TOKEN_PLUS, str_init("+")},
    };

    for (size_t i = 0; i < rpn_length; i++) {
        LOG_DEBUG("%s ", rpn_expression[i].value.data);
        CC_ASSERT(rpn_expression[i].type == expected_rpn_expression[i].type);
        CC_ASSERT(str_is_equal(rpn_expression[i].value, expected_rpn_expression[i].value));
    }

    free(rpn_expression);
    tokens_free(infix_expression, infix_length);
    tokens_free(expected_rpn_expression, rpn_length);
}

TEST(RPN_CONVERSION3) {
    token_t infix_expression[] = {
        {TOKEN_TAN,    str_init("tan")},
        {TOKEN_LPAREN, str_init("(")},
        {TOKEN_VAR,    str_init("x")},
        {TOKEN_RPAREN, str_init(")")},
        {TOKEN_PLUS,   str_init("+")},
        {TOKEN_SQRT,   str_init("sqrt")},
        {TOKEN_LPAREN, str_init("(")},
        {TOKEN_VAR,    str_init("y")},
        {TOKEN_RPAREN, str_init(")")},
    };
    size_t infix_length = STATIC_SIZE(infix_expression);

    size_t rpn_length = 0;
    token_t *rpn_expression = infix_to_rpn(infix_expression, infix_length, &rpn_length);

    token_t expected_rpn_expression[] = {
        {TOKEN_VAR,  str_init("x")},
        {TOKEN_TAN,  str_init("tan")},
        {TOKEN_VAR,  str_init("y")},
        {TOKEN_SQRT, str_init("sqrt")},
        {TOKEN_PLUS, str_init("+")},
    };

    for (size_t i = 0; i < rpn_length; i++) {
        LOG_DEBUG("%s ", rpn_expression[i].value.data);
        CC_ASSERT(rpn_expression[i].type == expected_rpn_expression[i].type);
        CC_ASSERT(str_is_equal(rpn_expression[i].value, expected_rpn_expression[i].value));
    }

    free(rpn_expression);
    tokens_free(infix_expression, infix_length);
    tokens_free(expected_rpn_expression, rpn_length);
}

TEST(RPN_CONVERSION4) {

    // - 12 + 2 * (-2) - (1) / 2 - sin(cos(tan(-10)))
    token_t infix_expression[] = {
    {TOKEN_UMINUS,  str_init("-")},
    {TOKEN_INT,    str_init("12")},
    {TOKEN_PLUS,   str_init("+")},
    {TOKEN_INT,    str_init("2")},
    {TOKEN_MULT,   str_init("*")},
    {TOKEN_LPAREN, str_init("(")},
    {TOKEN_UMINUS,  str_init("-")},
    {TOKEN_INT,    str_init("2")},
    {TOKEN_RPAREN, str_init(")")},
    {TOKEN_MINUS,  str_init("-")},
    {TOKEN_LPAREN, str_init("(")},
    {TOKEN_INT,    str_init("1")},
    {TOKEN_RPAREN, str_init(")")},
    {TOKEN_DIV,    str_init("/")},
    {TOKEN_INT,    str_init("2")},
    {TOKEN_MINUS,  str_init("-")},
    {TOKEN_SIN,    str_init("sin")},
    {TOKEN_LPAREN, str_init("(")},
    {TOKEN_COS,    str_init("cos")},
    {TOKEN_LPAREN, str_init("(")},
    {TOKEN_TAN,    str_init("tan")},
    {TOKEN_LPAREN, str_init("(")},
    {TOKEN_UMINUS,  str_init("-")},
    {TOKEN_INT,    str_init("10")},
    {TOKEN_RPAREN, str_init(")")},
    {TOKEN_RPAREN, str_init(")")},
    {TOKEN_RPAREN, str_init(")")},
};

    size_t infix_length = STATIC_SIZE(infix_expression);

    size_t rpn_length = 0;
    token_t *rpn_expression = infix_to_rpn(infix_expression, infix_length, &rpn_length);

    // 12 - 2 2 - * + 1 - 2 / 10 - tan cos sin -
    token_t expected_rpn_expression[] = {
        {TOKEN_INT,    str_init("12")},
        {TOKEN_MINUS,  str_init("-")},
        {TOKEN_INT,    str_init("2")},
        {TOKEN_INT,    str_init("2")},
        {TOKEN_MINUS,  str_init("-")},
        {TOKEN_MULT,   str_init("*")},
        {TOKEN_PLUS,   str_init("+")},
        {TOKEN_INT,    str_init("1")},
        {TOKEN_MINUS,  str_init("-")},
        {TOKEN_INT,    str_init("2")},
        {TOKEN_DIV,    str_init("/")},
        {TOKEN_INT,    str_init("10")},
        {TOKEN_MINUS,  str_init("-")},
        {TOKEN_TAN,    str_init("tan")},
        {TOKEN_COS,    str_init("cos")},
        {TOKEN_SIN,    str_init("sin")},
        {TOKEN_MINUS,  str_init("-")},
    };


    for (size_t i = 0; i < rpn_length; i++) {
        LOG_DEBUG("%s ", rpn_expression[i].value.data);
        CC_ASSERT(rpn_expression[i].type == expected_rpn_expression[i].type);
        CC_ASSERT(str_is_equal(rpn_expression[i].value, expected_rpn_expression[i].value));
    }

    free(rpn_expression);
    tokens_free(infix_expression, infix_length);
    tokens_free(expected_rpn_expression, rpn_length);
}