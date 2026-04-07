#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef uint8_t b8_t;
typedef int32_t error_t;

#define UNUSED(x) (void)(x);

#define MAX2(a, b) (a) < (b) ? (b) : (a)
#define MIN2(a, b) (a) < (b) ? (a) : (b)

#define ABS(a) (a) < 0 ? -1 * (a) : (a)

#define ERR_NO_ERROR 0

#define PUBLIC
#define PRIVATE static

#endif
