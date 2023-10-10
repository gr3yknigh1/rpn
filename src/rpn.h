#ifndef RPN_H
#define RPN_H

#include <stdio.h>
#include <stdlib.h>

#include "str.h"
#include "token.h"

typedef struct rpn_stack {
    token_t *tokens;
    size_t size;
    long top;
} rpn_stack_t;

#define RPN_FLOAT_MAX_STR_LEN 1079

#define RPN_TOK_OP(TOK_A, TOK_B, OP, TOK_RES_PTR)                           \
    do {                                                                    \
        double __A_VAL = atof((TOK_A).value.data);                          \
        double __B_VAL = atof((TOK_B).value.data);                          \
        token_free(&(TOK_A));                                               \
        token_free(&(TOK_B));                                               \
        double __RES = __A_VAL OP __B_VAL;                                  \
        char *__RES_BUF = malloc(sizeof(char) * RPN_FLOAT_MAX_STR_LEN + 1); \
        snprintf(__RES_BUF, RPN_FLOAT_MAX_STR_LEN + 1, "%lf", __RES);       \
        token_init((TOK_RES_PTR), TOKEN_FLOAT, str_init(__RES_BUF));        \
        free(__RES_BUF);                                                    \
    } while (0)

#define RPN_TOK_UOP(TOK, OP, TOK_RES_PTR)                                   \
    do {                                                                    \
        double __VAL = atof((TOK).value.data);                              \
        token_free(&(TOK));                                                 \
        double __RES = OP(__VAL);                                           \
        char *__RES_BUF = malloc(sizeof(char) * RPN_FLOAT_MAX_STR_LEN + 1); \
        snprintf(__RES_BUF, RPN_FLOAT_MAX_STR_LEN + 1, "%lf", __RES);       \
        token_init((TOK_RES_PTR), TOKEN_FLOAT, str_init(__RES_BUF));        \
        free(__RES_BUF);                                                    \
    } while (0)

void rpn_stack_init(rpn_stack_t *stack, size_t size);
void rpn_stack_push(rpn_stack_t *stack, token_t token);
token_t rpn_stack_pop(rpn_stack_t *stack);
token_t *infix_to_rpn(const token_t *infix_tokens, size_t infix_length, size_t *rpn_length);

double evaluate(const token_t *rpn_tokens, size_t rpn_length, double x);

#endif  // RPN_H