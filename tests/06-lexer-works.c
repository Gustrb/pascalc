#include "../src/io.h"
#include "../src/lexer.h"
#include "common.h"

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

#define ASSERT_TOKEN(pool, expected_type, expected_lexeme)                                         \
  {                                                                                                \
    uint8_t idx;                                                                                   \
    ASSERT_EQ(token_pool_pull(&pool, &idx), ERR_NO_ERROR);                                         \
    token_t *t = &pool.tokens[idx];                                                                \
    ASSERT_EQ(t->type, expected_type);                                                             \
    ASSERT_EQ(t->lexeme.len, (uint32_t)strlen(expected_lexeme));                                   \
    if (t->lexeme.len > 0)                                                                         \
    {                                                                                              \
      ASSERT_EQ(memcmp(t->lexeme.addr, expected_lexeme, t->lexeme.len), 0);                        \
    }                                                                                              \
    token_pool_release(&pool, idx);                                                                \
  }

b8_t should_initialize_a_lexer(void);
b8_t should_lex_sample_program(void);
b8_t should_lex_semicolon(void);
b8_t should_lex_period(void);
b8_t should_lex_parens(void);
b8_t should_lex_string_literal(void);
b8_t should_lex_keywords(void);
b8_t should_lex_identifier(void);

int main(void)
{
  b8_t (*tests[])(void) = {
      should_initialize_a_lexer, should_lex_sample_program, should_lex_semicolon,
      should_lex_period,         should_lex_parens,         should_lex_string_literal,
      should_lex_keywords,       should_lex_identifier,
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

b8_t should_initialize_a_lexer(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/hello-world.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  ASSERT_EQ(lexer.pool, &pool);
  ASSERT_EQ(lexer.file, &f);
  ASSERT_NEQ(lexer.read_position, 0);

  PASS_CASE;
  return 0;
}

b8_t should_lex_sample_program(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/hello-world.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_PROGRAM, "program");
  ASSERT_TOKEN(pool, TOKEN_TYPE_IDENTIFIER, "Hello");
  ASSERT_TOKEN(pool, TOKEN_TYPE_SEMICOLON, ";");
  ASSERT_TOKEN(pool, TOKEN_TYPE_BEGIN, "begin");
  ASSERT_TOKEN(pool, TOKEN_TYPE_IDENTIFIER, "writeln");
  ASSERT_TOKEN(pool, TOKEN_TYPE_LPAREN, "(");
  ASSERT_TOKEN(pool, TOKEN_TYPE_STRING_LITERAL, "'Hello, world.'");
  ASSERT_TOKEN(pool, TOKEN_TYPE_RPAREN, ")");
  ASSERT_TOKEN(pool, TOKEN_TYPE_SEMICOLON, ";");
  ASSERT_TOKEN(pool, TOKEN_TYPE_END, "end");
  ASSERT_TOKEN(pool, TOKEN_TYPE_PERIOD, ".");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}

b8_t should_lex_semicolon(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/semicolon.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_SEMICOLON, ";");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}

b8_t should_lex_period(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/period.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_PERIOD, ".");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}

b8_t should_lex_parens(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/parens.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_LPAREN, "(");
  ASSERT_TOKEN(pool, TOKEN_TYPE_RPAREN, ")");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}

b8_t should_lex_string_literal(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/string.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_STRING_LITERAL, "'hello world'");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}

b8_t should_lex_keywords(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/keywords.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_PROGRAM, "program");
  ASSERT_TOKEN(pool, TOKEN_TYPE_BEGIN, "begin");
  ASSERT_TOKEN(pool, TOKEN_TYPE_END, "end");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}

b8_t should_lex_identifier(void)
{
  START_CASE;

  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/programs/identifier.pas");
  PUSH_RESOURCE(f);
  ASSERT_EQ(err, ERR_NO_ERROR);

  token_pool_t pool = {0};
  token_pool_init(&pool);

  lexer_t lexer = {0};
  lexer_init(&lexer, &pool, &f);
  lexer_lex_file(&lexer);

  ASSERT_TOKEN(pool, TOKEN_TYPE_IDENTIFIER, "myVar");
  ASSERT_TOKEN(pool, TOKEN_TYPE_IDENTIFIER, "hello_world");
  ASSERT_TOKEN(pool, TOKEN_TYPE_IDENTIFIER, "test123");
  ASSERT_TOKEN(pool, TOKEN_TYPE_EOF, "");

  PASS_CASE;
  return 0;
}
