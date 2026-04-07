#include "ring_buffer.h"
#include "common.h"
#include <stdatomic.h>

#define ERR_BUFFER_FULL 1
#define ERR_BUFFER_EMPTY 2

void ring_buffer_init(ring_buffer_t *rb)
{
  assert(rb != NULL);

  atomic_store(&rb->head, 0);
  atomic_store(&rb->tail, 0);
}

error_t ring_buffer_pull(ring_buffer_t *rb, uint8_t *out)
{
  uint8_t tail = atomic_load(&rb->tail);
  if (tail == atomic_load(&rb->head))
  {
    return ERR_BUFFER_EMPTY;
  }

  uint8_t item = rb->buffer[tail];

  // Note: Yes, this overloads, yes, it is intentional
  // since the buffer is 256 elements long, after 255 (last index)
  // it will wrap-around to 0, which is the correct behavior
  // Cool trick that can be used if n is a power of 2
  atomic_store(&rb->tail, tail + 1);

  *out = item;

  return 0;
}

error_t ring_buffer_put(ring_buffer_t *rb, uint8_t val)
{
  uint8_t head = atomic_load(&rb->head);
  uint8_t next_head = head+1;

  if (next_head == atomic_load(&rb->tail))
  {
    return ERR_BUFFER_FULL;
  }

  rb->buffer[head] = val;
  atomic_store(&rb->head, next_head);

  return ERR_NO_ERROR;
}

const char *ring_buffer_error(error_t error)
{
  UNUSED(error);
  return "";
}
