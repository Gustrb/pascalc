#include "common.h"

#include "./io.h"

int main(int32_t argc, const char **argv)
{
  if (argc <= 1)
  {
    // TODO: Proper errors
    return 1;
  }

  const char *file_name = argv[1];
  memory_mapped_file_t f = {0};

  error_t err = ERR_NO_ERROR;
  if ((err = memory_mapped_file_from_path(&f, file_name)) != ERR_NO_ERROR)
  {
    return err;
  }

  memory_mapped_file_cleanup(&f);

  return 0;
}
