#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#define ASSERT_EQ(a, b) \
  if (a != b)           \
  {                     \
    fail(__func__, "assertion failed", __LINE__); \
    return 1;                           \
  }


#define ASSERT_NEQ(a, b) \
  if (a == b)           \
  {                     \
    fail(__func__, "assertion failed", __LINE__); \
    return 1;                           \
  }

#define START_CASE \
  fprintf(stdout, "[TEST]: Running (%s)\n", __func__);
#define PASS_CASE \
  fprintf(stdout, "[TEST]: Pass (%s)\n", __func__);


#define fail(casename, message, line) fprintf(stderr, "[FAIL]: %s, %s. Line: %d, file: %s\n", casename, message, line, __FILE__);

#endif
