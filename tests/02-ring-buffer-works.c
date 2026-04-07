#include "../src/ring_buffer.h"
#include "common.h"

b8_t should_be_able_to_create_a_ring_buffer(void);
b8_t should_fail_when_pulling_from_an_empty_ring_buffer(void);

int main(void)
{
  if (should_be_able_to_create_a_ring_buffer())
  {
    return 1;
  }

  if (should_fail_when_pulling_from_an_empty_ring_buffer())
  {
    return 1;
  }

  return 0;
}

b8_t should_be_able_to_create_a_ring_buffer(void)
{
  START_CASE;
  ring_buffer_t rb = {0};
  ring_buffer_init(&rb);

  ASSERT_EQ(rb.head, 0);
  ASSERT_EQ(rb.tail, 0);
  ASSERT_EQ((rb.head == rb.tail), 1);

  PASS_CASE;
  return 0;
}

b8_t should_fail_when_pulling_from_an_empty_ring_buffer(void)
{
  START_CASE;
  ring_buffer_t rb = {0};
  uint8_t res = 0;
  error_t err = ring_buffer_pull(&rb, &res);
  ASSERT_NEQ(err, ERR_NO_ERROR);
  PASS_CASE;
  return 0;
}
