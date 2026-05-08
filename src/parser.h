#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "token.h"
#include "token_pool.h"

typedef enum
{
  NODE_TYPE_PROGRAM,
  NODE_TYPE_IDENTIFIER,
  NODE_TYPE_BLOCK,

  NODE_TYPE_CALL,
  NODE_TYPE_STRING_LITERAL,
} node_type_t;

typedef struct
{
  node_type_t type;

  union
  {
    string_view_t sv;
  } data;

  // For block nodes, left and right is repurposed to be the interval of statements
  // so when type = NODE_TYPE_BLOCK, all the nodes inside the range [left, right) are statements
  // within the block
  uint64_t left;
  uint64_t right;
} node_t;

typedef struct
{
  node_t *pool;
  uint64_t len;
  uint64_t cap;
} node_pool_t;

PUBLIC error_t node_pool_init(node_pool_t *node_pool);
PUBLIC void node_pool_cleanup(node_pool_t *node_pool);

PUBLIC error_t node_pool_alloc(node_pool_t *pool, uint64_t *out);

// Side array of node indices, used to give nodes with a variable number of
// children (BLOCK, CALL, ...) a contiguous [start, end) range.
// A parent node stores its range in left/right; children are read as
// child_pool->data[i] for i in [left, right).
typedef struct
{
  uint64_t *data;
  uint64_t len;
  uint64_t cap;
} child_index_pool_t;

PUBLIC error_t cipool_init(child_index_pool_t *p);
PUBLIC void cipool_cleanup(child_index_pool_t *p);
PUBLIC error_t cipool_push(child_index_pool_t *p, uint64_t idx);

typedef struct
{
  token_pool_t *token_pool;
  node_pool_t *node_pool;
  child_index_pool_t *child_pool;

  uint8_t curr_token_idx;
  uint8_t peek_token_idx;
} parser_t;

PUBLIC error_t parser_init(parser_t *parser, token_pool_t *token_pool, node_pool_t *node_pool,
                           child_index_pool_t *child_pool);
PUBLIC error_t parser_parse(parser_t *parser, uint64_t *out);

#endif
