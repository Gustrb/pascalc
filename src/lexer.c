#include "lexer.h"
#include "common.h"
#include "token.h"

#include <string.h>

PRIVATE void lexer_read_char(lexer_t *lexer);
PRIVATE void lexer_skip_whitespace(lexer_t *lexer);
PRIVATE void lexer_read_identifier(lexer_t *lexer, token_t *tok);
PRIVATE void lexer_read_string(lexer_t *lexer, token_t *tok);
PRIVATE token_type_t lexer_lookup_keyword(const char *addr, uint32_t len);
PRIVATE b8_t is_letter(char ch);

PUBLIC void lexer_init(lexer_t *lexer, token_pool_t *pool, memory_mapped_file_t *mmpf)
{
  lexer->file = mmpf;
  lexer->pool = pool;
  lexer->position = 0;
  lexer->ch = 0;
  lexer->read_position = 0;

  lexer_read_char(lexer);
}

PUBLIC void lexer_lex_file(lexer_t *lexer)
{
  while (lexer->ch != 0)
  {
    lexer_skip_whitespace(lexer);
    if (lexer->ch == 0)
    {
      break;
    }

    uint8_t index;
    token_pool_alloc_blocking(lexer->pool, &index);

    token_t *tok = &lexer->pool->tokens[index];
    tok->lexeme.addr = &lexer->file->addr[lexer->position];
    tok->lexeme.len = 1;

    switch (lexer->ch)
    {
    case ';':
      tok->type = TOKEN_TYPE_SEMICOLON;
      lexer_read_char(lexer);
      break;
    case ',':
      tok->type = TOKEN_TYPE_COMMA;
      lexer_read_char(lexer);
      break;
    case '.':
      tok->type = TOKEN_TYPE_PERIOD;
      lexer_read_char(lexer);
      break;
    case '(':
      tok->type = TOKEN_TYPE_LPAREN;
      lexer_read_char(lexer);
      break;
    case ')':
      tok->type = TOKEN_TYPE_RPAREN;
      lexer_read_char(lexer);
      break;
    case '\'':
      lexer_read_string(lexer, tok);
      break;
    default:
      if (is_letter(lexer->ch))
      {
        lexer_read_identifier(lexer, tok);
      }
      else
      {
        lexer_read_char(lexer);
      }
      break;
    }

    token_pool_push(lexer->pool, index);
  }

  uint8_t index;
  token_pool_alloc_blocking(lexer->pool, &index);

  lexer->pool->tokens[index].type = TOKEN_TYPE_EOF;
  lexer->pool->tokens[index].lexeme = (string_view_t){.len = 0, .addr = NULL};
  token_pool_push(lexer->pool, index);
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

PRIVATE void lexer_skip_whitespace(lexer_t *lexer)
{
  while (lexer->ch == ' ' || lexer->ch == '\t' || lexer->ch == '\n' || lexer->ch == '\r')
  {
    lexer_read_char(lexer);
  }
}

PRIVATE b8_t is_letter(char ch)
{
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

PRIVATE void lexer_read_identifier(lexer_t *lexer, token_t *tok)
{
  uint32_t start = lexer->position;

  while (is_letter(lexer->ch) || (lexer->ch >= '0' && lexer->ch <= '9'))
  {
    lexer_read_char(lexer);
  }

  tok->lexeme.addr = &lexer->file->addr[start];
  tok->lexeme.len = lexer->position - start;
  tok->type = lexer_lookup_keyword(tok->lexeme.addr, tok->lexeme.len);
}

PRIVATE void lexer_read_string(lexer_t *lexer, token_t *tok)
{
  uint32_t start = lexer->position;

  lexer_read_char(lexer);
  while (lexer->ch != '\'' && lexer->ch != 0)
  {
    lexer_read_char(lexer);
  }
  lexer_read_char(lexer);

  tok->type = TOKEN_TYPE_STRING_LITERAL;
  tok->lexeme.addr = &lexer->file->addr[start];
  tok->lexeme.len = lexer->position - start;
}

PRIVATE token_type_t lexer_lookup_keyword(const char *addr, uint32_t len)
{
  if (len == 7 && memcmp(addr, "program", 7) == 0)
    return TOKEN_TYPE_PROGRAM;
  if (len == 5 && memcmp(addr, "begin", 5) == 0)
    return TOKEN_TYPE_BEGIN;
  if (len == 3 && memcmp(addr, "end", 3) == 0)
    return TOKEN_TYPE_END;
  return TOKEN_TYPE_IDENTIFIER;
}
