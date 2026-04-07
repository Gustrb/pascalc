#include "lexer.h"
#include "common.h"

PRIVATE void lexer_read_char(lexer_t *lexer);

PUBLIC void lexer_init(lexer_t *lexer, token_pool_t *pool, memory_mapped_file_t *mmpf)
{
  lexer->file = mmpf;
  lexer->pool = pool;
  lexer->position = 0;
  lexer->ch = 0;
  lexer->read_position = 0;

  lexer_read_char(lexer);
}

PRIVATE void lexer_read_char(lexer_t *lexer)
{
  if (lexer->read_position >= lexer->file->size)
  {
    lexer->ch = 0;
  }
  else
  {
    lexer->ch = lexer->file->addr[lexer->read_position];
  }

  lexer->position = lexer->read_position++;    
}
