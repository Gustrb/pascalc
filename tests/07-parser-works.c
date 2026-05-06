// Override PRIVATE before any include that pulls in src/common.h, so
// that the parser's static helpers become visible in this TU.
#define PRIVATE

#include "../src/parser.c"

#include "common.h"
#include "../src/io.h"
#include "../src/lexer.h"

#include <string.h>

memory_mapped_file_t resources_to_cleanup[1024];
size_t resources_to_cleanup_len = 0;

#define PUSH_RESOURCE(f) resources_to_cleanup[resources_to_cleanup_len++] = f;

void cleanup(void)
{
  for (size_t i = 0; i < resources_to_cleanup_len; ++i)
  {
    memory_mapped_file_cleanup(&resources_to_cleanup[i]);
  }
}

typedef struct
{
  memory_mapped_file_t f;
  token_pool_t tpool;
  lexer_t lex;
  node_pool_t npool;
  child_index_pool_t cpool;
  parser_t parser;
} setup_t;

static b8_t setup_load(setup_t *s, const char *path)
{
  memset(s, 0, sizeof(*s));
  if (memory_mapped_file_from_path(&s->f, path) != ERR_NO_ERROR) return 1;
  PUSH_RESOURCE(s->f);
  token_pool_init(&s->tpool);
  lexer_init(&s->lex, &s->tpool, &s->f);
  lexer_lex_file(&s->lex);
  if (node_pool_init(&s->npool) != ERR_NO_ERROR) return 1;
  if (cipool_init(&s->cpool) != ERR_NO_ERROR) return 1;
  if (parser_init(&s->parser, &s->tpool, &s->npool, &s->cpool) != ERR_NO_ERROR) return 1;
  return 0;
}

static void setup_unload(setup_t *s)
{
  node_pool_cleanup(&s->npool);
  cipool_cleanup(&s->cpool);
}

#define ASSERT_SV(sv, lit)                              \
  ASSERT_EQ((sv).len, (uint32_t)strlen(lit))            \
  ASSERT_EQ(memcmp((sv).addr, (lit), (sv).len), 0)

b8_t should_init_parser(void);
b8_t should_parse_identifier(void);
b8_t should_parse_string_literal(void);
b8_t should_parse_call_with_one_arg(void);
b8_t should_parse_empty_block(void);
b8_t should_parse_block_with_one_stmt(void);
b8_t should_parse_hello_world(void);
b8_t should_fail_on_missing_program_keyword(void);
b8_t should_fail_on_missing_period(void);

int main(void)
{
  b8_t (*tests[])(void) = {
      should_init_parser,
      should_parse_identifier,
      should_parse_string_literal,
      should_parse_call_with_one_arg,
      should_parse_empty_block,
      should_parse_block_with_one_stmt,
      should_parse_hello_world,
      should_fail_on_missing_program_keyword,
      should_fail_on_missing_period,
  };

  size_t num_tests = sizeof(tests) / sizeof(tests[0]);

  for (size_t i = 0; i < num_tests; ++i)
  {
    if (tests[i]())
    {
      cleanup();
      return 1;
    }
    cleanup();
    resources_to_cleanup_len = 0;
  }

  return 0;
}

b8_t should_init_parser(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/hello-world.pas"), 0);

  ASSERT_EQ(s.parser.token_pool, &s.tpool);
  ASSERT_EQ(s.parser.node_pool, &s.npool);
  ASSERT_EQ(s.parser.child_pool, &s.cpool);
  ASSERT_EQ(s.tpool.tokens[s.parser.curr_token_idx].type, TOKEN_TYPE_PROGRAM);

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_parse_identifier(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/single-identifier.pas"), 0);

  uint64_t idx;
  ASSERT_EQ(__parser_parse_identifier(&s.parser, &idx), ERR_NO_ERROR);

  node_t *n = &s.npool.pool[idx];
  ASSERT_EQ(n->type, NODE_TYPE_IDENTIFIER);
  ASSERT_SV(n->data.sv, "foo");

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_parse_string_literal(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/single-string.pas"), 0);

  uint64_t idx;
  ASSERT_EQ(__parser_parse_string_literal(&s.parser, &idx), ERR_NO_ERROR);

  node_t *n = &s.npool.pool[idx];
  ASSERT_EQ(n->type, NODE_TYPE_STRING_LITERAL);
  // The lexer keeps the surrounding quotes in the lexeme.
  ASSERT_SV(n->data.sv, "'hello world'");

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_parse_call_with_one_arg(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/call.pas"), 0);

  uint64_t idx;
  ASSERT_EQ(__parser_parse_call(&s.parser, &idx), ERR_NO_ERROR);

  node_t *call = &s.npool.pool[idx];
  ASSERT_EQ(call->type, NODE_TYPE_CALL);
  ASSERT_SV(call->data.sv, "writeln");
  ASSERT_EQ(call->right - call->left, 1u);

  uint64_t arg_idx = s.cpool.data[call->left];
  node_t *arg = &s.npool.pool[arg_idx];
  ASSERT_EQ(arg->type, NODE_TYPE_STRING_LITERAL);
  ASSERT_SV(arg->data.sv, "'hi'");

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_parse_empty_block(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/empty-block.pas"), 0);

  uint64_t idx;
  ASSERT_EQ(__parser_parse_block(&s.parser, &idx), ERR_NO_ERROR);

  node_t *block = &s.npool.pool[idx];
  ASSERT_EQ(block->type, NODE_TYPE_BLOCK);
  ASSERT_EQ(block->left, block->right);

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_parse_block_with_one_stmt(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/block-with-stmt.pas"), 0);

  uint64_t idx;
  ASSERT_EQ(__parser_parse_block(&s.parser, &idx), ERR_NO_ERROR);

  node_t *block = &s.npool.pool[idx];
  ASSERT_EQ(block->type, NODE_TYPE_BLOCK);
  ASSERT_EQ(block->right - block->left, 1u);

  uint64_t stmt_idx = s.cpool.data[block->left];
  node_t *stmt = &s.npool.pool[stmt_idx];
  ASSERT_EQ(stmt->type, NODE_TYPE_CALL);
  ASSERT_SV(stmt->data.sv, "writeln");

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_parse_hello_world(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/hello-world.pas"), 0);

  uint64_t root;
  ASSERT_EQ(parser_parse(&s.parser, &root), ERR_NO_ERROR);

  node_t *prog = &s.npool.pool[root];
  ASSERT_EQ(prog->type, NODE_TYPE_PROGRAM);

  node_t *id = &s.npool.pool[prog->left];
  ASSERT_EQ(id->type, NODE_TYPE_IDENTIFIER);
  ASSERT_SV(id->data.sv, "Hello");

  node_t *block = &s.npool.pool[prog->right];
  ASSERT_EQ(block->type, NODE_TYPE_BLOCK);
  ASSERT_EQ(block->right - block->left, 1u);

  uint64_t stmt_idx = s.cpool.data[block->left];
  node_t *call = &s.npool.pool[stmt_idx];
  ASSERT_EQ(call->type, NODE_TYPE_CALL);
  ASSERT_SV(call->data.sv, "writeln");
  ASSERT_EQ(call->right - call->left, 1u);

  uint64_t arg_idx = s.cpool.data[call->left];
  node_t *arg = &s.npool.pool[arg_idx];
  ASSERT_EQ(arg->type, NODE_TYPE_STRING_LITERAL);
  ASSERT_SV(arg->data.sv, "'Hello, world.'");

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_fail_on_missing_program_keyword(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/bad-no-program-keyword.pas"), 0);

  uint64_t root;
  ASSERT_NEQ(parser_parse(&s.parser, &root), ERR_NO_ERROR);

  setup_unload(&s);
  PASS_CASE;
  return 0;
}

b8_t should_fail_on_missing_period(void)
{
  START_CASE;

  setup_t s;
  ASSERT_EQ(setup_load(&s, "./tests/fixtures/programs/bad-no-period.pas"), 0);

  uint64_t root;
  ASSERT_NEQ(parser_parse(&s.parser, &root), ERR_NO_ERROR);

  setup_unload(&s);
  PASS_CASE;
  return 0;
}
