#include "token_pool.h"

PUBLIC void token_pool_init(token_pool_t *pool)
{
  assert(pool != NULL);

  ring_buffer_init(&pool->ready);
  ring_buffer_init(&pool->free_list);

  for (uint8_t i = 0; i < RING_BUFFER_LEN - 1; i++)
  {
    ring_buffer_put(&pool->free_list, i);
  }
}

PUBLIC error_t token_pool_alloc(token_pool_t *pool, uint8_t *index)
{
  return ring_buffer_pull(&pool->free_list, index);
}

PUBLIC error_t token_pool_push(token_pool_t *pool, uint8_t index)
{
  return ring_buffer_put(&pool->ready, index);
}

PUBLIC error_t token_pool_pull(token_pool_t *pool, uint8_t *index)
{
  return ring_buffer_pull(&pool->ready, index);
}

PUBLIC error_t token_pool_release(token_pool_t *pool, uint8_t index)
{
  return ring_buffer_put(&pool->free_list, index);
}
