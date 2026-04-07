#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../src/io.h"
#include "common.h"

b8_t should_fail_if_file_does_not_exist(void);
b8_t should_map_the_file_correctly(void);
b8_t should_unmap_the_file_correctly(void);

memory_mapped_file_t resources_to_cleanup[1024];
size_t resources_to_cleanup_len = 0;

#define PUSH_RESOURCE(f) resources_to_cleanup[resources_to_cleanup_len++] = f;

void cleanup(void)
{
  for (size_t i = 0; i < resources_to_cleanup_len; ++i)
  {
    memory_mapped_file_cleanup(&resources_to_cleanup[i]);
  }
}

int main(void)
{
  if (should_fail_if_file_does_not_exist())
  {
    cleanup();
    return 1;
  }

  if (should_map_the_file_correctly())
  {
    cleanup();
    return 1;
  }

  if (should_unmap_the_file_correctly())
  {
    cleanup();
    return 1;
  }
  cleanup();

  return 0;
}

b8_t should_fail_if_file_does_not_exist(void)
{
  START_CASE;
  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "a/path/that/does/not/exist");
  PUSH_RESOURCE(f);
  ASSERT_NEQ(err, ERR_NO_ERROR);
  PASS_CASE;
  return 0;
}

b8_t should_map_the_file_correctly(void)
{
  START_CASE;
  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/cume.md");
  ASSERT_EQ(err, ERR_NO_ERROR);
  PUSH_RESOURCE(f);
  ASSERT_EQ(f.size, 84);

  FILE *file = fopen("./tests/fixtures/cume.md", "r");
  ASSERT_NEQ(file, NULL);

  char buff[256];
  size_t read_bytes = fread(buff, sizeof(char), f.size, file);
  fclose(file);
  ASSERT_EQ(read_bytes, f.size);

  for (uint32_t i = 0; i < f.size; ++i)
  {
    ASSERT_EQ(buff[i], f.addr[i]);
  }

  PASS_CASE;
  return 0;
}

b8_t should_unmap_the_file_correctly(void)
{
  START_CASE;
  memory_mapped_file_t f = {0};
  error_t err = memory_mapped_file_from_path(&f, "./tests/fixtures/cume.md");
  ASSERT_EQ(err, ERR_NO_ERROR);

  file_descriptor_t fd = f.fd;
  const char *addr = f.addr;

  memory_mapped_file_cleanup(&f);

  // Check fd is closed via /proc/self/fd/<fd>
  char fd_path[64];
  snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);
  ASSERT_NEQ(access(fd_path, F_OK), 0);

  // Check memory is unmapped via /proc/self/maps
  FILE *maps = fopen("/proc/self/maps", "r");
  ASSERT_NEQ(maps, NULL);
  char line[512];
  char addr_str[32];
  snprintf(addr_str, sizeof(addr_str), "%lx", (unsigned long)addr);
  while (fgets(line, sizeof(line), maps))
  {
    ASSERT_EQ((strstr(line, addr_str) == line), 0);
  }
  fclose(maps);

  // Check struct fields are zeroed
  ASSERT_EQ(f.fd, 0);
  ASSERT_EQ(f.addr, NULL);
  ASSERT_EQ(f.size, 0);

  PASS_CASE;
  return 0;
}
