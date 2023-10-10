/*
 *  === Polish Notation ===
 *
 *  [Team 70]
 *  @authors:
 *    - lambertm (Leader)
 *    - atruskat
 *    - wittenbb
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "bool.h"
#include "darray.h"
#include "defs.h"
#include "lexer.h"
#include "log.h"
#include "rpn.h"
#include "token.h"

#define WIDTH 80
#define HEIGHT 25

void render_graph(token_t *tokens, size_t token_count);
double mapd(double x, double in_min, double in_max, double out_min, double out_max);

int read_input(darray_t *buf);

int main(void) {
    int ret = EXIT_SUCCESS;

    darray_t expr_buf = {0};
    darray_init(&expr_buf, 10, sizeof(char));

    if (read_input(&expr_buf) != EXIT_SUCCESS) {
        ret = EXIT_FAILURE;
    } else {
        const str_t expr = str_init_1(expr_buf.items, expr_buf.item_count);  // "sqrt(-(1/(-x*x)))";

        darray_t infix_tokens = {0};
        darray_init(&infix_tokens, 10, sizeof(token_t));

        lexer_status_t lex_ret = lexer_parse(&infix_tokens, expr.data);

        // token_t *infix_tokens_data = infix_tokens.items;

        if (lex_ret != LEXER_OK) {
            // LOG_ERROR("Lexer error: %u", lex_ret);
            ret = EXIT_FAILURE;
        } else {
            // for (size_t i = 0; i < infix_tokens.item_count; ++i) {
            //     const char *type = token_type_to_str(infix_tokens_data[i].type);
            //     const char *data = infix_tokens_data[i].value.data;
            //     LOG_DEBUG("<Token type=%s value=[%s]>", type, data);
            // }

            size_t rpn_length = 0;
            token_t *rpn_tokens = infix_to_rpn(infix_tokens.items, infix_tokens.item_count, &rpn_length);

            // printf("Infix Expression: ");
            // for (size_t i = 0; i < infix_tokens.item_count; i++) {
            //     printf("%s ", ((token_t *)infix_tokens.items)[i].value.data);
            // }
            // printf("\n");

            // printf("RPN Expression: ");
            // for (size_t i = 0; i < rpn_length; i++) {
            //     printf("%s ", rpn_tokens[i].value.data);
            // }
            // printf("\n");

            render_graph(rpn_tokens, rpn_length);
            // double y = evaluate(rpn_tokens, rpn_length, 1);
            // LOG_DEBUG("Y=%lf", y);

            free(rpn_tokens);
        }

        darray_free(&infix_tokens);

        for (size_t i = 0; i < infix_tokens.item_count; ++i) {
            token_t token = ((token_t *)infix_tokens.items)[i];
            token_free(&token);
        }
    }

    if (ret != EXIT_SUCCESS) {
        printf("n/a");
    }

    darray_free(&expr_buf);

    return ret;
}

double mapd(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void render_graph(token_t *tokens, size_t tokens_count) {
    double x_min = 0;
    double x_max = (double)4 * M_PI;
    double y_min = -1;
    double y_max = 1;

    for (double y = 0; y < HEIGHT; y++) {
        for (double x = 0; x < WIDTH; x++) {
            double x_out = mapd(x, 0, WIDTH, x_min, x_max);
            double y_in = evaluate(tokens, tokens_count, x_out);
            double y_out = mapd(y_in, y_min, y_max, 0, HEIGHT);

            // LOG_DEBUG("f(%.2lf) = %.2lf (from y_in=%.2lf)", x_out, y_out, y_in);
            if (y == round(y_out)) {
                printf("*");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

int read_input(darray_t *buf) {
    int ret = EXIT_SUCCESS;

    char c = (char)getchar();

    while (c != '\n') {
        darray_push(buf, &c);
        c = (char)getchar();
    }

    return ret;
}