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

b8_t should_initialize_a_lexer(void);

int main(void)
{
  if (should_initialize_a_lexer())
  {
    cleanup();
    return 1;
  }

  cleanup();

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
