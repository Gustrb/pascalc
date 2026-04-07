#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "io.h"
#include "token.h"
#include "token_pool.h"
#include "io.h"

typedef struct
{
  memory_mapped_file_t *file;
  token_pool_t *pool;

  uint32_t position;
  uint32_t read_position;
  char ch;
} lexer_t;

PUBLIC void lexer_init(lexer_t *lexer, token_pool_t *pool, memory_mapped_file_t *mmpf);
PUBLIC void lexer_lex_file(lexer_t *lexer);

#endif
