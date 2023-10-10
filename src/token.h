#ifndef TOKEN_H
#define TOKEN_H

#include "str.h"

typedef enum token_type {
    TOKEN_NONE,    // Invalid token
    TOKEN_PLUS,    // '+'
    TOKEN_MINUS,   // '-'
    TOKEN_UMINUS,  // '-' (unary minus)
    TOKEN_MULT,    // '*'
    TOKEN_DIV,     // '/'
    TOKEN_LPAREN,  // '('
    TOKEN_RPAREN,  // ')'
    TOKEN_INT,     //  1 2 3     Integer
    TOKEN_FLOAT,   //  1.2 -3.02 Real number
    TOKEN_SIN,     // sin(x)
    TOKEN_COS,     // cos(x)
    TOKEN_TAN,     // tan(x)
    TOKEN_CTG,     // ctg(x)
    TOKEN_SQRT,    // sqrt(x)
    TOKEN_LN,      // ln(x)
    TOKEN_VAR,     // 'x'
} token_type_t;

typedef struct token {
    token_type_t type;
    str_t value;
} token_t;

// const char *token_type_to_str(token_type_t type);
bool token_type_is_op(token_type_t type);
bool token_type_is_trig_op(token_type_t type);

void token_init(token_t *token, token_type_t type, str_t value);
token_t token_copy(const token_t *src);
void token_free(token_t *token);
void tokens_free(token_t *tokens, size_t size);

#endif  // TOKEN_H
