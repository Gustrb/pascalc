#include "io.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERR_FAILED_TO_OPEN 1
#define ERR_FAILED_TO_STAT 2
#define ERR_FAILED_TO_MMAP 3

PUBLIC error_t memory_mapped_file_from_path(memory_mapped_file_t *mmf, const char *path)
{
  assert(mmf != NULL);
  assert(path != NULL);

  int32_t fd = open(path, O_RDONLY);
  if (fd < 0)
  {
    return ERR_FAILED_TO_OPEN;
  }

  struct stat sb;
  if (fstat(fd, &sb) < 0)
  {
    return ERR_FAILED_TO_STAT;
  }

  const char *map = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED)
  {
    return ERR_FAILED_TO_MMAP;
  }

  mmf->addr = map;
  mmf->fd = fd;
  mmf->size = sb.st_size;

  return 0;
}

PUBLIC void memory_mapped_file_cleanup(memory_mapped_file_t *mmf)
{
  // Little hack to not have to downcast const
  union
  {
    const char *a;
    void *b;
  } data = {.a = mmf->addr};

  close(mmf->fd);
  munmap(data.b, mmf->size);

  mmf->fd = 0;
  mmf->addr = NULL;
  mmf->size = 0;
}

PUBLIC const char *memory_mapped_error(error_t error)
{
  UNUSED(error);
  return "";
}
