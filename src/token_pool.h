#ifndef TOKEN_POOL_H
#define TOKEN_POOL_H

#include "common.h"
#include "token.h"
#include "ring_buffer.h"

#define TOKEN_POOL_SIZE 256

typedef struct
{
  token_t tokens[TOKEN_POOL_SIZE];

  ring_buffer_t ready;
  ring_buffer_t free_list;
} token_pool_t;

void token_pool_init(token_pool_t *pool);

error_t token_pool_alloc(token_pool_t *pool, uint8_t *index);
error_t token_pool_push(token_pool_t *pool, uint8_t index);
error_t token_pool_pull(token_pool_t *pool, uint8_t *index);
error_t token_pool_release(token_pool_t *pool, uint8_t index);

#endif
