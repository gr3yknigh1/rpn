#ifndef LEXER_H
#define LEXER_H

#include "bool.h"
#include "darray.h"
#include "defs.h"
#include "token.h"

typedef enum lexer_status {
    LEXER_OK,
    LEXER_UNEXP_TOKEN,
    LEXER_SYNTAX_ERR,
    LEXER_INVALID_NAME,
} lexer_status_t;

bool char_is_digit(char c);
bool char_is_alpha(char c);

token_type_t lexer_get_name_token_type(const char *name);

lexer_status_t lexer_parse(darray_t *tokens, const char *str);
lexer_status_t lexer_parse_number(token_t *token, const char **str);
lexer_status_t lexer_parse_name(token_t *token, const char **str);

lexer_status_t lexer_check_op_syntax(const token_t *tokens, size_t tokens_count);
lexer_status_t lexer_check_parens(const token_t *tokens, size_t tokens_count);
lexer_status_t lexer_next_token(darray_t *tokens, token_t *token, const char **str, size_t index);

#endif  // LEXER_H