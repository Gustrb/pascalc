#include "ring_buffer.h"
#include <stdatomic.h>

#define ERR_BUFFER_FULL 1

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
    return ERR_BUFFER_FULL;
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

const char *ring_buffer_error(error_t error)
{
  UNUSED(error);
  return "";
}
