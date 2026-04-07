#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdatomic.h>
#include "common.h"

typedef struct
{
  uint8_t buffer[256];

  _Atomic uint8_t head;
  _Atomic uint8_t tail;
} ring_buffer_t;

void ring_buffer_init(ring_buffer_t *);
error_t ring_buffer_pull(ring_buffer_t *, uint8_t *v);
const char *ring_buffer_error(error_t error);

#endif
