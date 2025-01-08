#include <stdio.h>
#include <string.h>
#include "stringstream_v2.h"
#include "utest.h"

UTEST(stringstream, empty)
{
  stringstream ss;
  uint8_t buf[16];

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(ss_bytes_left(&ss), 0);
  ASSERT_EQ(ss_capacity_left(&ss), (int)sizeof(buf));
}

UTEST(stringstream, write_read)
{
  stringstream ss;
  uint8_t buf[16];

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(ss_putc(&ss, 0x01), 1);
  ASSERT_EQ(ss_put16be(&ss, 0xabcd), 2);
  ASSERT_EQ(ss_bytes_left(&ss), 3);
  ASSERT_EQ(ss_capacity_left(&ss), 13);

  // fill the buffer
  ASSERT_EQ(ss_put32be(&ss, 0x12345678), 4);
  ASSERT_EQ(ss_putc(&ss, 0x05), 1);
  ASSERT_EQ(ss_put(&ss, (uint8_t *)"Testdata", 8), 8);
  ASSERT_EQ(ss_bytes_left(&ss), 16);
  ASSERT_EQ(ss_capacity_left(&ss), 0);

  // read back
  uint8_t c;
  ASSERT_EQ(ss_getc(&ss, &c), 1);
  ASSERT_EQ(c, 0x01);

  uint16_t n;
  ASSERT_EQ(ss_get16be(&ss, &n), 2);
  ASSERT_EQ(n, 0xabcd);

  uint32_t m;
  ASSERT_EQ(ss_get32be(&ss, &m), 4);
  ASSERT_EQ(m, (uint32_t)0x12345678);

  ASSERT_EQ(ss_getc(&ss, &c), 1);
  ASSERT_EQ(c, 0x05);

  uint8_t s[8];
  ASSERT_EQ(ss_get(&ss, s, 8), 8);
  ASSERT_EQ(memcmp(s, "Testdata", 8), 0);

  ASSERT_EQ(ss_bytes_left(&ss), 0);
  ASSERT_EQ(ss_capacity_left(&ss), 0);
}


UTEST(stringstream, read_write_overflow)
{
  stringstream ss;
  uint8_t buf[16];
  char *test_data = "TestdataTestdata";
  int len = 16;

  ss_init(&ss, buf, sizeof(buf));

  ASSERT_EQ(ss_put(&ss, (uint8_t *)test_data, len), 16);
  ASSERT_EQ(ss_bytes_left(&ss), 16);
  ASSERT_EQ(ss_capacity_left(&ss), 0);

  // put overflow
  ASSERT_EQ(ss_putc(&ss, 0x01), 0);
  ASSERT_EQ(ss_put16be(&ss, 0xabcd), 0);
  ASSERT_EQ(ss_put32be(&ss, 0x12345678), 0);
  ASSERT_EQ(ss_put(&ss, (uint8_t *)"Testdata", 8), 0);
  ASSERT_EQ(ss_bytes_left(&ss), 16);
  ASSERT_EQ(ss_capacity_left(&ss), 0);

  // read back
  uint8_t null[16];
  ASSERT_EQ(ss_get(&ss, null, 16), 16);

  // read overflow
  uint8_t c;
  ASSERT_EQ(ss_getc(&ss, &c), 0);
  uint16_t n;
  ASSERT_EQ(ss_get16be(&ss, &n), 0);
  uint32_t m;
  ASSERT_EQ(ss_get32be(&ss, &m), 0);
  ASSERT_EQ(ss_get(&ss, null, 8), 0);
}

UTEST(stringstream, rewind)
{
  stringstream ss;
  uint8_t buf[8];
  char *test_data = "Testdata";
  int len = 8;

  ss_init(&ss, buf, sizeof(buf));
  ASSERT_EQ(ss_put(&ss, (uint8_t *)test_data, len), 8);

  // rewind forward
  ASSERT_EQ(ss_rewind(&ss, 4), 4);
  ASSERT_EQ(ss_bytes_left(&ss), 4);
  ASSERT_EQ(ss_capacity_left(&ss), 0);
  uint8_t s[4];
  ASSERT_EQ(ss_get(&ss, s, 4), 4);
  ASSERT_EQ(memcmp(s, "data", 4), 0);

  // rewind backward
  ASSERT_EQ(ss_rewind(&ss, -6), -6);
  ASSERT_EQ(ss_bytes_left(&ss), 6);
  ASSERT_EQ(ss_get(&ss, s, 4), 4);
  ASSERT_EQ(memcmp(s, "stda", 4), 0);
}