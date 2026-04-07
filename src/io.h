#ifndef IO_H
#define IO_H

#include "common.h"

typedef int32_t file_descriptor_t;

typedef struct
{
  file_descriptor_t fd;
  // Does it make sense to support files larger than 32bits?
  uint32_t size;
  const char *addr;
} memory_mapped_file_t;

PUBLIC error_t memory_mapped_file_from_path(memory_mapped_file_t *mmf, const char *path);
PUBLIC void memory_mapped_file_cleanup(memory_mapped_file_t *mmf);
PUBLIC const char *memory_mapped_error(error_t error);

#endif
