#include "rpn.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

int get_precedence(token_type_t type) {
    int ret = 0;
    switch (type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_UMINUS:
            ret = 2;
            break;

        case TOKEN_MULT:
        case TOKEN_DIV:
            ret = 3;
            break;

        case TOKEN_SIN:
        case TOKEN_COS:
        case TOKEN_TAN:
        case TOKEN_CTG:
        case TOKEN_SQRT:
        case TOKEN_LN:
            ret = 4;
            break;

        case TOKEN_LPAREN:
        case TOKEN_RPAREN:
            ret = 1;
            break;

        case TOKEN_INT:
        case TOKEN_FLOAT:
        case TOKEN_VAR:
            ret = 5;
            break;

        default:
            ret = 0;
            break;
    }
    return ret;
}

/*
 * allocate memory for the stack's tokens
 * store the specified size of the stack, indicating the max it can hold
 * initialize the stack's top to -1 to indicate it's empty initially
 */
void rpn_stack_init(rpn_stack_t *stack, size_t size) {
    stack->tokens = (token_t *)malloc(size * sizeof(token_t));
    stack->size = size;
    stack->top = -1;
}

/*
 * adds a new el (token) onto the stack while ensuring that the stack doesn't overflow
 */
void rpn_stack_push(rpn_stack_t *stack, token_t token) {
    if (stack->top < (long)stack->size - 1) {
        stack->tokens[++stack->top] = token;
    }
}

/*
 * pops and returns the topmost element from the stack if the stack is not empty, ensuring that it
 * If the stack is empty, it returns a special token indicating that the stack is empty.
 */
token_t rpn_stack_pop(rpn_stack_t *stack) {
    if (stack->top >= 0) {
        return stack->tokens[stack->top--];
    }
    return (token_t){.type = TOKEN_NONE, .value = str_make_empty()};
}

/* from infix notation to reverse polish notation */
token_t *infix_to_rpn(const token_t *infix_tokens, size_t infix_length, size_t *rpn_length) {
    rpn_stack_t operator_stack;
    rpn_stack_init(&operator_stack, infix_length);
    token_t *rpn_tokens = (token_t *)malloc(infix_length * sizeof(token_t));
    size_t rpn_index = 0;

    for (size_t i = 0; i < infix_length; i++) {
        token_t current_token = infix_tokens[i];

        if (current_token.type == TOKEN_INT || current_token.type == TOKEN_FLOAT ||
            current_token.type == TOKEN_VAR) {
            rpn_tokens[rpn_index++] = current_token;
        }

        else if (current_token.type >= TOKEN_SIN && current_token.type <= TOKEN_SQRT) {
            while (operator_stack.top >= 0 &&
                   get_precedence(operator_stack.tokens[operator_stack.top].type) >=
                       get_precedence(current_token.type)) {
                rpn_tokens[rpn_index++] = rpn_stack_pop(&operator_stack);
            }
            rpn_stack_push(&operator_stack, current_token);
        }

        else if (current_token.type == TOKEN_LPAREN) {
            rpn_stack_push(&operator_stack, current_token);
        }

        else if (current_token.type == TOKEN_RPAREN) {
            while (operator_stack.top >= 0 &&
                   operator_stack.tokens[operator_stack.top].type != TOKEN_LPAREN) {
                rpn_tokens[rpn_index++] = rpn_stack_pop(&operator_stack);
            }

            if (operator_stack.top >= 0 && operator_stack.tokens[operator_stack.top].type == TOKEN_LPAREN) {
                rpn_stack_pop(&operator_stack);
            }
        }

        else {
            while (operator_stack.top >= 0 &&
                   get_precedence(operator_stack.tokens[operator_stack.top].type) >=
                       get_precedence(current_token.type)) {
                rpn_tokens[rpn_index++] = rpn_stack_pop(&operator_stack);
            }
            rpn_stack_push(&operator_stack, current_token);
        }
    }

    while (operator_stack.top >= 0) {
        rpn_tokens[rpn_index++] = rpn_stack_pop(&operator_stack);
    }

    *rpn_length = rpn_index;
    free(operator_stack.tokens);
    return rpn_tokens;
}

// NOTE(wittenbb): INT CONVERTED TO FLOATS --- HAAAACK!!!!

void perform_operation(token_t token, rpn_stack_t *stack) {
    // TODO: THIS IS HAAACK
    token_t tok = {0}, tok_0 = {0}, tok_1 = {0}, res = {0};

    switch (token.type) {
        case TOKEN_PLUS:
            tok_0 = rpn_stack_pop(stack);
            tok_1 = rpn_stack_pop(stack);
            RPN_TOK_OP(tok_0, tok_1, +, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_MINUS:
            tok_0 = rpn_stack_pop(stack);
            tok_1 = rpn_stack_pop(stack);
            RPN_TOK_OP(tok_0, tok_1, -, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_MULT:
            tok_0 = rpn_stack_pop(stack);
            tok_1 = rpn_stack_pop(stack);
            RPN_TOK_OP(tok_0, tok_1, *, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_DIV:
            // ERROR
            // TODO(wittenbb): DIVISION ERROR
            tok_0 = rpn_stack_pop(stack);
            tok_1 = rpn_stack_pop(stack);
            RPN_TOK_OP(tok_0, tok_1, /, &res);
            rpn_stack_push(stack, res);
            // if (num2 != 0)
            //     rpn_stack_push(stack, num1 / num2);
            // else
            //     rpn_stack_push(stack, 1000);  // Handle division by zero
            break;
        case TOKEN_UMINUS:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, -, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_SIN:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, sin, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_COS:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, cos, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_TAN:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, tan, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_CTG:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, 1 / tan, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_SQRT:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, sqrt, &res);
            rpn_stack_push(stack, res);
            break;
        case TOKEN_LN:
            tok = rpn_stack_pop(stack);
            RPN_TOK_UOP(tok, log, &res);
            rpn_stack_push(stack, res);
            break;
        // TODO: CHECK
        case TOKEN_INT:
        case TOKEN_FLOAT:
            rpn_stack_push(stack, token);
            break;
        case TOKEN_VAR:
            // rpn_stack_push(stack, 5.0);
            break;
        default:
            break;
    }
}

double evaluate(const token_t *rpn_tokens, size_t rpn_length, double x) {
    rpn_stack_t stack = {0};
    rpn_stack_init(&stack, rpn_length);

    for (size_t i = 0; i < rpn_length; i++) {
        token_t token = token_copy(rpn_tokens + i);

        // FIXME: HAAAACK
        if (token.type == TOKEN_LPAREN || token.type == TOKEN_RPAREN) {
            continue;
        }

        if (token.type == TOKEN_VAR) {
            char *conv_buffer = malloc(RPN_FLOAT_MAX_STR_LEN * sizeof(char) + 1);
            snprintf(conv_buffer, RPN_FLOAT_MAX_STR_LEN, "%lf", x);

            str_set_0(&token.value, conv_buffer);
            token.type = TOKEN_FLOAT;
            rpn_stack_push(&stack, token);
            free(conv_buffer);
        } else if (token.type == TOKEN_INT || token.type == TOKEN_FLOAT) {
            rpn_stack_push(&stack, token);
        } else {
            perform_operation(token, &stack);
        }
    }

    token_t result = rpn_stack_pop(&stack);
    double result_val = atof(result.value.data);
    return result_val;
}
