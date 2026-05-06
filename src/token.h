#ifndef TOKEN_H
#define TOKEN_H

#include "common.h"
#include "string_view.h"

typedef enum
{
  TOKEN_TYPE_PROGRAM,
  TOKEN_TYPE_BEGIN,
  TOKEN_TYPE_END,

  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_STRING_LITERAL,

  TOKEN_TYPE_SEMICOLON,
  TOKEN_TYPE_PERIOD,
  TOKEN_TYPE_LPAREN,
  TOKEN_TYPE_RPAREN,
  TOKEN_TYPE_COMMA,

  TOKEN_TYPE_EOF,
} token_type_t;

typedef struct
{
  token_type_t type;
  string_view_t lexeme;

  // TODO: add row column
} token_t;

#endif
