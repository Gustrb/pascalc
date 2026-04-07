#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include "common.h"

typedef struct
{
  uint32_t len;
  const char *addr;
} string_view_t;

#endif
