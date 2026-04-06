#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef uint8_t b8_t;

#define UNUSED(x) (void)(x);

#define MAX2(a, b) (a) < (b) ? (b) : (a)
#define MIN2(a, b) (a) < (b) ? (a) : (b)

#define PUBLIC
#define PRIVATE static

#endif
