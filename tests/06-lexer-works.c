#include "../src/io.h"
#include "../src/lexer.h"
#include "common.h"

b8_t should_initialize_a_lexer(void);

int main(void)
{
  if (should_initialize_a_lexer())
  {
    return 1;
  }

  return 0;
}

b8_t should_initialize_a_lexer(void)
{
  START_CASE;
  PASS_CASE;
  return 0;
}
