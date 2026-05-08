#include "parser.h"

#include "common.h"
#include "token.h"
#include "token_pool.h"
#include <stdint.h>

#define NODE_POOL_INITIAL_CAPACITY        2048
#define CHILD_INDEX_POOL_INITIAL_CAPACITY 2048

#define TOKEN_AT(p, idx) (p)->token_pool->tokens[(idx)]
#define NODE_AT(p, idx)  (p)->node_pool->pool[(idx)]

#define GRACEFULLY_HANDLE_IT(err)                                                                  \
  do                                                                                               \
  {                                                                                                \
    if (err != ERR_NO_ERROR)                                                                       \
      return err;                                                                                  \
  } while (0)

#define ERR_WRONG_TOKEN_TYPE    1234
#define ERR_TOO_MANY_STATEMENTS 1235

PRIVATE error_t __parser_next_token(parser_t *parser);
PRIVATE error_t __parser_expect_token_to_be(parser_t *parser, token_type_t t);

PRIVATE error_t __parser_parse_program(parser_t *parser, uint64_t *out);
PRIVATE error_t __parser_parse_identifier(parser_t *parser, uint64_t *output);
PRIVATE error_t __parser_parse_block(parser_t *parser, uint64_t *output);
PRIVATE error_t __parser_parse_statement(parser_t *parser, uint64_t *output);
PRIVATE error_t __parser_parse_expression(parser_t *parser, uint64_t *output);
PRIVATE error_t __parser_parse_call(parser_t *parser, uint64_t *output);
PRIVATE error_t __parser_parse_string_literal(parser_t *parser, uint64_t *output);

PUBLIC error_t parser_init(parser_t *parser, token_pool_t *token_pool, node_pool_t *pool,
                           child_index_pool_t *child_pool)
{
  parser->curr_token_idx = 0;
  parser->peek_token_idx = 0;

  parser->node_pool = pool;
  parser->token_pool = token_pool;
  parser->child_pool = child_pool;

  error_t err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  return ERR_NO_ERROR;
}

PUBLIC error_t parser_parse(parser_t *parser, uint64_t *out)
{
  return __parser_parse_program(parser, out);
}

PRIVATE error_t __parser_parse_program(parser_t *parser, uint64_t *output)
{
  error_t err = __parser_expect_token_to_be(parser, TOKEN_TYPE_PROGRAM);
  GRACEFULLY_HANDLE_IT(err);

  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  err = node_pool_alloc(parser->node_pool, output);
  GRACEFULLY_HANDLE_IT(err);

  uint64_t identifier_idx;
  err = __parser_parse_identifier(parser, &identifier_idx);
  GRACEFULLY_HANDLE_IT(err);

  node_t *node = &NODE_AT(parser, *output);
  node->type = NODE_TYPE_PROGRAM;
  node->left = identifier_idx;

  err = __parser_expect_token_to_be(parser, TOKEN_TYPE_SEMICOLON);
  GRACEFULLY_HANDLE_IT(err);

  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  uint64_t block_idx;
  err = __parser_parse_block(parser, &block_idx);
  GRACEFULLY_HANDLE_IT(err);

  // Recomputing node since the pointer might have been invalidated
  // due to a node_pool reallocation. :P
  node = &NODE_AT(parser, *output);
  node->right = block_idx;

  err = __parser_expect_token_to_be(parser, TOKEN_TYPE_PERIOD);
  GRACEFULLY_HANDLE_IT(err);

  // PERIOD is the last meaningful token; don't advance past it. The token
  // pool's ring buffer has no entries left, so a trailing pull would fail
  // even though parsing succeeded.

  return ERR_NO_ERROR;
}

#define SCRATCH_CAP 256

PRIVATE error_t __parser_parse_block(parser_t *parser, uint64_t *output)
{
  error_t err = __parser_expect_token_to_be(parser, TOKEN_TYPE_BEGIN);
  GRACEFULLY_HANDLE_IT(err);

  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  // Note: needed since a statement can alloc more stuff within
  // If we see scratch_cap be hit we can just bump the value
  uint64_t scratch[SCRATCH_CAP] = {0};
  uint64_t n = 0;
  while (TOKEN_AT(parser, parser->curr_token_idx).type != TOKEN_TYPE_END)
  {
    if (n >= SCRATCH_CAP)
    {
      return ERR_TOO_MANY_STATEMENTS;
    }
    uint64_t stmt_idx;
    err = __parser_parse_statement(parser, &stmt_idx);
    GRACEFULLY_HANDLE_IT(err);
    scratch[n++] = stmt_idx;

    if (TOKEN_AT(parser, parser->curr_token_idx).type == TOKEN_TYPE_SEMICOLON)
    {
      err = __parser_next_token(parser);
      GRACEFULLY_HANDLE_IT(err);
    }
  }

  err = __parser_expect_token_to_be(parser, TOKEN_TYPE_END);
  GRACEFULLY_HANDLE_IT(err);

  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  uint64_t start = parser->child_pool->len;
  for (uint64_t i = 0; i < n; ++i)
  {
    err = cipool_push(parser->child_pool, scratch[i]);
    GRACEFULLY_HANDLE_IT(err);
  }

  err = node_pool_alloc(parser->node_pool, output);
  GRACEFULLY_HANDLE_IT(err);

  node_t *block = &NODE_AT(parser, *output);
  block->type = NODE_TYPE_BLOCK;
  block->left = start;
  block->right = start + n;

  return ERR_NO_ERROR;
}

PRIVATE error_t __parser_parse_statement(parser_t *parser, uint64_t *output)
{
  token_type_t curr = TOKEN_AT(parser, parser->curr_token_idx).type;
  token_type_t peek = TOKEN_AT(parser, parser->peek_token_idx).type;

  if (curr == TOKEN_TYPE_IDENTIFIER)
  {
    if (peek == TOKEN_TYPE_LPAREN)
      return __parser_parse_call(parser, output);
    else
      return ERR_WRONG_TOKEN_TYPE;
  }

  return ERR_NO_ERROR;
}

PRIVATE error_t __parser_parse_call(parser_t *parser, uint64_t *output)
{
  error_t err = __parser_expect_token_to_be(parser, TOKEN_TYPE_IDENTIFIER);
  GRACEFULLY_HANDLE_IT(err);

  string_view_t name = TOKEN_AT(parser, parser->curr_token_idx).lexeme;
  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  err = __parser_expect_token_to_be(parser, TOKEN_TYPE_LPAREN);
  GRACEFULLY_HANDLE_IT(err);
  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  uint64_t scratch[SCRATCH_CAP] = {0};
  uint64_t n = 0;
  while (TOKEN_AT(parser, parser->curr_token_idx).type != TOKEN_TYPE_RPAREN)
  {
    if (n >= SCRATCH_CAP)
    {
      return ERR_TOO_MANY_STATEMENTS;
    }

    uint64_t expr_idx;
    ;
    err = __parser_parse_expression(parser, &expr_idx);
    GRACEFULLY_HANDLE_IT(err);
    scratch[n++] = expr_idx;

    if (TOKEN_AT(parser, parser->curr_token_idx).type == TOKEN_TYPE_COMMA)
    {
      err = __parser_next_token(parser);
      GRACEFULLY_HANDLE_IT(err);
    }
  }

  err = __parser_expect_token_to_be(parser, TOKEN_TYPE_RPAREN);
  GRACEFULLY_HANDLE_IT(err);
  err = __parser_next_token(parser);
  GRACEFULLY_HANDLE_IT(err);

  uint64_t start = parser->child_pool->len;
  for (uint64_t i = 0; i < n; ++i)
  {
    err = cipool_push(parser->child_pool, scratch[i]);
    GRACEFULLY_HANDLE_IT(err);
  }

  err = node_pool_alloc(parser->node_pool, output);
  GRACEFULLY_HANDLE_IT(err);

  node_t *node = &NODE_AT(parser, *output);

  node->type = NODE_TYPE_CALL;
  node->data.sv = name;
  node->left = start;
  node->right = start + n;

  return ERR_NO_ERROR;
}

PRIVATE error_t __parser_parse_expression(parser_t *parser, uint64_t *output)
{
  if (TOKEN_AT(parser, parser->curr_token_idx).type == TOKEN_TYPE_STRING_LITERAL)
  {
    return __parser_parse_string_literal(parser, output);
  }
  else
  {
    return ERR_WRONG_TOKEN_TYPE;
  }
}

PRIVATE error_t __parser_parse_string_literal(parser_t *parser, uint64_t *output)
{
  error_t err = __parser_expect_token_to_be(parser, TOKEN_TYPE_STRING_LITERAL);
  GRACEFULLY_HANDLE_IT(err);

  err = node_pool_alloc(parser->node_pool, output);
  GRACEFULLY_HANDLE_IT(err);

  node_t *node = &NODE_AT(parser, *output);
  token_t *tok = &TOKEN_AT(parser, parser->curr_token_idx);

  node->type = NODE_TYPE_STRING_LITERAL;
  node->data.sv = tok->lexeme;

  return __parser_next_token(parser);
}

PRIVATE error_t __parser_parse_identifier(parser_t *parser, uint64_t *output)
{
  error_t err = __parser_expect_token_to_be(parser, TOKEN_TYPE_IDENTIFIER);
  GRACEFULLY_HANDLE_IT(err);

  err = node_pool_alloc(parser->node_pool, output);
  GRACEFULLY_HANDLE_IT(err);

  node_t *node = &NODE_AT(parser, *output);
  token_t *tok = &TOKEN_AT(parser, parser->curr_token_idx);

  node->type = NODE_TYPE_IDENTIFIER;
  node->data.sv = tok->lexeme;

  return __parser_next_token(parser);
}

PUBLIC error_t node_pool_init(node_pool_t *node_pool)
{
  node_pool->pool = malloc(sizeof(node_t) * NODE_POOL_INITIAL_CAPACITY);
  if (node_pool->pool == NULL)
  {
    return ERR_OUT_OF_MEMORY;
  }

  node_pool->len = 0;
  node_pool->cap = NODE_POOL_INITIAL_CAPACITY;
  return ERR_NO_ERROR;
}

PUBLIC error_t node_pool_alloc(node_pool_t *pool, uint64_t *out)
{
  if (pool->len >= pool->cap)
  {
    pool->cap *= 2;
    void *temp = realloc(pool->pool, pool->cap * sizeof(node_t));
    if (temp == NULL)
    {
      return ERR_OUT_OF_MEMORY;
    }

    pool->pool = temp;
  }

  *out = pool->len;
  pool->len++;

  return ERR_NO_ERROR;
}

PUBLIC void node_pool_cleanup(node_pool_t *node_pool)
{
  free(node_pool->pool);
  node_pool->cap = 0;
  node_pool->len = 0;
}

PUBLIC error_t cipool_init(child_index_pool_t *p)
{
  p->data = malloc(sizeof(uint64_t) * CHILD_INDEX_POOL_INITIAL_CAPACITY);
  if (p->data == NULL)
  {
    return ERR_OUT_OF_MEMORY;
  }

  p->len = 0;
  p->cap = CHILD_INDEX_POOL_INITIAL_CAPACITY;
  return ERR_NO_ERROR;
}

PUBLIC error_t cipool_push(child_index_pool_t *p, uint64_t idx)
{
  if (p->len >= p->cap)
  {
    p->cap *= 2;
    void *temp = realloc(p->data, p->cap * sizeof(uint64_t));
    if (temp == NULL)
    {
      return ERR_OUT_OF_MEMORY;
    }

    p->data = temp;
  }

  p->data[p->len++] = idx;
  return ERR_NO_ERROR;
}

PUBLIC void cipool_cleanup(child_index_pool_t *p)
{
  free(p->data);
  p->cap = 0;
  p->len = 0;
}

PRIVATE error_t __parser_next_token(parser_t *parser)
{
  token_pool_release(parser->token_pool, parser->curr_token_idx);
  parser->curr_token_idx = parser->peek_token_idx;
  return token_pool_pull(parser->token_pool, &parser->peek_token_idx);
}

PRIVATE error_t __parser_expect_token_to_be(parser_t *parser, token_type_t t)
{
  token_t *tok = &TOKEN_AT(parser, parser->curr_token_idx);
  if (tok->type != t)
  {
    return ERR_WRONG_TOKEN_TYPE;
  }

  return ERR_NO_ERROR;
}
