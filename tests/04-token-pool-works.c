#include "../src/token_pool.h"
#include "common.h"

b8_t should_init_pool_with_free_slots(void);
b8_t should_alloc_and_release(void);
b8_t should_push_and_pull_through_ready(void);
b8_t should_fail_pull_on_empty_ready(void);
b8_t should_fail_alloc_when_all_slots_used(void);

int main(void)
{
  if (should_init_pool_with_free_slots())
    return 1;
  if (should_alloc_and_release())
    return 1;
  if (should_push_and_pull_through_ready())
    return 1;
  if (should_fail_pull_on_empty_ready())
    return 1;
  if (should_fail_alloc_when_all_slots_used())
    return 1;

  return 0;
}

b8_t should_init_pool_with_free_slots(void)
{
  START_CASE;
  token_pool_t pool = {0};
  token_pool_init(&pool);

  uint8_t index = 0;
  error_t err = token_pool_alloc(&pool, &index);
  ASSERT_EQ(err, ERR_NO_ERROR);

  PASS_CASE;
  return 0;
}

b8_t should_alloc_and_release(void)
{
  START_CASE;
  token_pool_t pool = {0};
  token_pool_init(&pool);

  // Drain all free slots
  uint8_t idx = 0;
  for (int i = 0; i < RING_BUFFER_LEN - 1; i++)
  {
    error_t err = token_pool_alloc(&pool, &idx);
    ASSERT_EQ(err, ERR_NO_ERROR);
  }

  // Free list is empty now, release one slot
  error_t err = token_pool_release(&pool, 42);
  ASSERT_EQ(err, ERR_NO_ERROR);

  // Next alloc should return the released slot
  uint8_t idx2 = 0;
  err = token_pool_alloc(&pool, &idx2);
  ASSERT_EQ(err, ERR_NO_ERROR);
  ASSERT_EQ(idx2, 42);

  PASS_CASE;
  return 0;
}

b8_t should_push_and_pull_through_ready(void)
{
  START_CASE;
  token_pool_t pool = {0};
  token_pool_init(&pool);

  uint8_t idx = 0;
  error_t err = token_pool_alloc(&pool, &idx);
  ASSERT_EQ(err, ERR_NO_ERROR);

  pool.tokens[idx].type = TOKEN_TYPE_BEGIN;

  err = token_pool_push(&pool, idx);
  ASSERT_EQ(err, ERR_NO_ERROR);

  uint8_t pulled = 0;
  err = token_pool_pull(&pool, &pulled);
  ASSERT_EQ(err, ERR_NO_ERROR);
  ASSERT_EQ(idx, pulled);
  ASSERT_EQ(pool.tokens[pulled].type, TOKEN_TYPE_BEGIN);

  PASS_CASE;
  return 0;
}

b8_t should_fail_pull_on_empty_ready(void)
{
  START_CASE;
  token_pool_t pool = {0};
  token_pool_init(&pool);

  uint8_t idx = 0;
  error_t err = token_pool_pull(&pool, &idx);
  ASSERT_NEQ(err, ERR_NO_ERROR);

  PASS_CASE;
  return 0;
}

b8_t should_fail_alloc_when_all_slots_used(void)
{
  START_CASE;
  token_pool_t pool = {0};
  token_pool_init(&pool);

  uint8_t idx = 0;
  for (int i = 0; i < RING_BUFFER_LEN - 1; i++)
  {
    error_t err = token_pool_alloc(&pool, &idx);
    ASSERT_EQ(err, ERR_NO_ERROR);
  }

  error_t err = token_pool_alloc(&pool, &idx);
  ASSERT_NEQ(err, ERR_NO_ERROR);

  PASS_CASE;
  return 0;
}
