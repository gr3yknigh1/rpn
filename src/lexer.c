#include "lexer.h"

#include <string.h>

#include "log.h"
bool char_is_digit(char c) { return c >= '0' && c <= '9'; }
bool char_is_alpha(char c) { return c >= 'A' && c <= 'z'; }

token_type_t lexer_get_name_token_type(const char *name) {
    token_type_t ret = TOKEN_NONE;

    if (strcmp(name, "sin") == 0) {
        ret = TOKEN_SIN;
    } else if (strcmp(name, "cos") == 0) {
        ret = TOKEN_COS;
    } else if (strcmp(name, "tan") == 0) {
        ret = TOKEN_TAN;
    } else if (strcmp(name, "ctg") == 0) {
        ret = TOKEN_CTG;
    } else if (strcmp(name, "sqrt") == 0) {
        ret = TOKEN_SQRT;
    } else if (strcmp(name, "ln") == 0) {
        ret = TOKEN_LN;
    } else if (strcmp(name, "x") == 0) {
        ret = TOKEN_VAR;
    }

    return ret;
}

lexer_status_t lexer_parse_name(token_t *token, const char **name_begin) {
    lexer_status_t ret = LEXER_OK;

    const char *it = *name_begin;

    while (char_is_alpha(*it)) {
        ++it;
    }

    const char *name_end = it - 1;
    size_t name_len = (size_t)name_end - (size_t)*name_begin + 1;
    str_t name_str = str_init_1(*name_begin, name_len);

    token_type_t token_type = lexer_get_name_token_type(name_str.data);

    if (token_type == TOKEN_NONE) {
        ret = LEXER_INVALID_NAME;
    } else {
        token_init(token, token_type, str_init_1(*name_begin, name_len));
        *name_begin = name_end;
    }

    return ret;
}

lexer_status_t lexer_next_token(darray_t *tokens, token_t *token, const char **str, size_t index) {
    lexer_status_t ret = LEXER_OK;

    char c = **str;

    token_t *tokens_items = (token_t *)tokens->items;

    bool is_uminus = c == '-' && (index == 0 || (tokens_items[tokens->item_count - 1].type == TOKEN_LPAREN));

    if (char_is_digit(c) || c == '.') {
        ret = lexer_parse_number(token, str);
    } else if (char_is_alpha(c)) {
        bool is_valid_name = index == 0 || tokens_items[tokens->item_count - 1].type == TOKEN_LPAREN ||
                             token_type_is_op(tokens_items[tokens->item_count - 1].type);

        if (is_valid_name) {
            ret = lexer_parse_name(token, str);
        } else {
            ret = LEXER_SYNTAX_ERR;
        }

    } else if (c == '+') {
        token_init(token, TOKEN_PLUS, str_init("+"));
    } else if (is_uminus) {
        token_init(token, TOKEN_UMINUS, str_init("-"));
    } else if (c == '-') {
        token_init(token, TOKEN_MINUS, str_init("-"));
    } else if (c == '*') {
        token_init(token, TOKEN_MULT, str_init("*"));
    } else if (c == '/') {
        token_init(token, TOKEN_DIV, str_init("/"));
    } else if (c == '(') {
        token_init(token, TOKEN_LPAREN, str_init("("));
    } else if (c == ')') {
        token_init(token, TOKEN_RPAREN, str_init(")"));
    } else {
        // LOG_ERROR("Unexpected token! [%c]", **str);
        ret = LEXER_UNEXP_TOKEN;
    }

    if (ret == LEXER_OK) {
        *str = *str + 1;
    }

    return ret;
}

lexer_status_t lexer_parse(darray_t *tokens, const char *expr) {
    lexer_status_t ret = LEXER_OK;

    char *expr_buf = malloc(strlen(expr) + 1);
    str_trim_char(expr_buf, expr, ' ');
    // LOG_DEBUG("TRIMMED EXPR: %s", expr_buf);

    const char *expr_it = expr_buf;
    size_t token_idx = 0;

    while (*expr_it != '\0') {
        token_t token = {0};
        ret = lexer_next_token(tokens, &token, &expr_it, token_idx);

        if (ret != LEXER_OK) {
            break;
        }

        if (token.type != TOKEN_NONE) {
            darray_push(tokens, &token);
        }
        ++token_idx;
    }

    if (ret == LEXER_OK) {
        ret = lexer_check_parens(tokens->items, tokens->item_count);
    }

    if (ret == LEXER_OK) {
        ret = lexer_check_op_syntax(tokens->items, tokens->item_count);
    }

    free(expr_buf);

    return ret;
}

lexer_status_t lexer_check_op_syntax(const token_t *tokens, size_t tokens_count) {
    lexer_status_t ret = LEXER_OK;

    if (tokens_count != 1) {
        const token_t *prev_tok = tokens;
        for (size_t i = 1; i < tokens_count; ++i) {
            const token_t *tok = tokens + i;

            bool is_invalid_op = (token_type_is_op(prev_tok->type) && token_type_is_op(tok->type)) ||
                                 (token_type_is_trig_op(prev_tok->type) && token_type_is_trig_op(tok->type));

            if (is_invalid_op) {
                // LOG_DEBUG("curr=<Token type=[%s] value=[%s]>", token_type_to_str(tok->type),
                // tok->value.data); LOG_DEBUG("prev=<Token type=[%s] value=[%s]>",
                // token_type_to_str(prev_tok->type),
                //           prev_tok->value.data);
                ret = LEXER_SYNTAX_ERR;
                break;
            }

            prev_tok++;
        }
    }

    return ret;
}

lexer_status_t lexer_check_parens(const token_t *tokens, size_t tokens_count) {
    lexer_status_t ret = LEXER_OK;

    long opened_parens_count = 0;

    for (size_t i = 0; i < tokens_count; ++i) {
        if (tokens[i].type == TOKEN_LPAREN) {
            opened_parens_count++;
        } else if (tokens[i].type == TOKEN_RPAREN) {
            if (opened_parens_count - 1 < 0 || tokens[i - 1].type == TOKEN_LPAREN) {
                ret = LEXER_SYNTAX_ERR;
                break;
            }
            --opened_parens_count;
        }
    }

    if (opened_parens_count != 0) {
        ret = LEXER_SYNTAX_ERR;
    }

    return ret;
}

lexer_status_t lexer_parse_number(token_t *token, const char **str) {
    lexer_status_t ret = LEXER_OK;

    const char *num_begin = *str;
    const char *it = num_begin;
    char c = *it;

    bool has_dot = false;

    while (char_is_digit(c) || c == '.') {
        if (c == '.' && has_dot) {
            LOG_ERROR("Double dot in number: [%s]", num_begin);
            ret = LEXER_UNEXP_TOKEN;
            break;
        }

        if (c == '.') {
            has_dot = true;
        }

        ++it;
        c = *it;
    }

    if (ret == LEXER_OK) {
        const char *num_end = it - 1;
        size_t num_len = (size_t)num_end - (size_t)num_begin + 1;
        token_init(token, has_dot ? TOKEN_FLOAT : TOKEN_INT, str_init_1(num_begin, num_len));
        *str = num_end;
    }

    return ret;
}
