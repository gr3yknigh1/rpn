#include "token.h"

#include "defs.h"

// const char *token_type_to_str(token_type_t type) {
//     switch (type) {
//         case TOKEN_NONE:
//             return STRINGIFY(TOKEN_NONE);
//             break;
//         case TOKEN_PLUS:
//             return STRINGIFY(TOKEN_PLUS);
//         case TOKEN_MINUS:
//             return STRINGIFY(TOKEN_MINUS);
//         case TOKEN_UMINUS:
//             return STRINGIFY(TOKEN_UMINUS);
//         case TOKEN_MULT:
//             return STRINGIFY(TOKEN_MULT);
//         case TOKEN_DIV:
//             return STRINGIFY(TOKEN_DIV);
//         case TOKEN_LPAREN:
//             return STRINGIFY(TOKEN_LPAREN);
//         case TOKEN_RPAREN:
//             return STRINGIFY(TOKEN_RPAREN);
//         case TOKEN_INT:
//             return STRINGIFY(TOKEN_INT);
//         case TOKEN_FLOAT:
//             return STRINGIFY(TOKEN_FLOAT);
//         case TOKEN_SIN:
//             return STRINGIFY(TOKEN_SIN);
//         case TOKEN_COS:
//             return STRINGIFY(TOKEN_COS);
//         case TOKEN_TAN:
//             return STRINGIFY(TOKEN_TAN);
//         case TOKEN_CTG:
//             return STRINGIFY(TOKEN_CTG);
//         case TOKEN_SQRT:
//             return STRINGIFY(TOKEN_SQRT);
//         case TOKEN_LN:
//             return STRINGIFY(TOKEN_LN);
//         case TOKEN_VAR:
//             return STRINGIFY(TOKEN_VAR);
//         default:
//             return "TOKEN_UNEXPECRED";
//     }
// }

bool token_type_is_op(token_type_t type) {
    return type == TOKEN_MINUS || type == TOKEN_PLUS || type == TOKEN_UMINUS || type == TOKEN_MULT ||
           type == TOKEN_DIV;
}

bool token_type_is_trig_op(token_type_t type) {
    return type == TOKEN_TAN || type == TOKEN_SIN || type == TOKEN_COS || type == TOKEN_CTG ||
           type == TOKEN_LN || type == TOKEN_SQRT;
}

void token_init(token_t *token, token_type_t type, str_t value) {
    *token = (token_t){
        .type = type,
        .value = value,
    };
}

token_t token_copy(const token_t *src) {
    return (token_t){
        .value = str_init(src->value.data),
        .type = src->type,
    };
}

void token_free(token_t *token) {
    str_free(&token->value);
    token->type = TOKEN_NONE;
}

void tokens_free(token_t *tokens, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        token_free(tokens + i);
    }
}