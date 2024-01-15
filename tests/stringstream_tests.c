#include <stdio.h>
#include <string.h>
#include "stringstream.h"
#include "utest.h"

UTEST(stringstream, empty)
{
  stringstream ss;
  uint8_t buf[16];

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(istring_bytes_left(&ss), 0);
  ASSERT_EQ(ostring_bytes_left(&ss), (int)sizeof(buf));
}

UTEST(stringstream, iload_ok)
{
  stringstream ss;
  uint8_t buf[16];
  char *test_data = "Testdata";
  int len = 8;

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(istring_load(&ss, (uint8_t *)test_data, len), len);
  ASSERT_EQ(istring_bytes_left(&ss), len);
}


UTEST(stringstream, iload_ok2)
{
  stringstream ss;
  uint8_t buf[16];
  char *test_data = "TestdataTestdata";
  int len = 16;

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(istring_load(&ss, (uint8_t *)test_data, len), len);
  ASSERT_EQ(istring_bytes_left(&ss), len);
}

UTEST(stringstream, iload_ovf)
{
  stringstream ss;
  uint8_t buf[8];
  char *test_data = "TestdataTestdata";
  int len = 16;

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(istring_load(&ss, (uint8_t *)test_data, len), 0);
  ASSERT_EQ(istring_bytes_left(&ss), 0);
}
