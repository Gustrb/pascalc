#include "../src/ring_buffer.h"
#include "common.h"
#include <pthread.h>

#define NUM_ITEMS 200

static ring_buffer_t rb;

static void *producer(void *arg)
{
  UNUSED(arg);
  for (uint8_t i = 1; i <= NUM_ITEMS; i++)
  {
    while (ring_buffer_put(&rb, i) != ERR_NO_ERROR)
    {
      // Buffer full, spin until consumer drains a slot
    }
  }
  return NULL;
}

static void *consumer(void *arg)
{
  int32_t *sum = (int32_t *)arg;
  *sum = 0;
  for (int count = 0; count < NUM_ITEMS; count++)
  {
    uint8_t v = 0;
    while (ring_buffer_pull(&rb, &v) != ERR_NO_ERROR)
    {
      // Buffer empty, spin until producer adds an item
    }
    *sum += v;
  }
  return NULL;
}

b8_t should_produce_and_consume_concurrently(void);

int main(void)
{
  if (should_produce_and_consume_concurrently())
  {
    return 1;
  }

  return 0;
}

b8_t should_produce_and_consume_concurrently(void)
{
  START_CASE;

  ring_buffer_init(&rb);

  int32_t consumed_sum = 0;
  pthread_t prod, cons;

  pthread_create(&prod, NULL, producer, NULL);
  pthread_create(&cons, NULL, consumer, &consumed_sum);

  pthread_join(prod, NULL);
  pthread_join(cons, NULL);

  // sum of 1..NUM_ITEMS = NUM_ITEMS * (NUM_ITEMS + 1) / 2
  int32_t expected = NUM_ITEMS * (NUM_ITEMS + 1) / 2;
  ASSERT_EQ(consumed_sum, expected);

  PASS_CASE;
  return 0;
}
