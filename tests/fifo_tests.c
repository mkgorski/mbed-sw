#include <stdio.h>
#include <string.h>
#include "fifo.h"
#include "utest.h"

UTEST(fifo, empty)
{
  DEFINE_FIFO(f, 10);

  ASSERT_NE(fifo_empty(&f), 0);
}

UTEST(fifo, add_single)
{
  DEFINE_FIFO(f, 10);

  fifo_putc(&f, 'c');
  ASSERT_EQ(fifo_data_slots(&f), 1);
  ASSERT_EQ(fifo_free_slots(&f), 8);
  ASSERT_EQ(fifo_empty(&f), 0);
}

UTEST(fifo, add_many)
{
  DEFINE_FIFO(f, 20);

  char *msg = "Test str";
  fifo_put(&f, (uint8_t *)msg, sizeof(msg));
  ASSERT_EQ(fifo_data_slots(&f), 8);
  ASSERT_EQ(fifo_free_slots(&f), 11);
}

UTEST(fifo, get_single)
{
  DEFINE_FIFO(f, 20);
  uint8_t c;

  fifo_putc(&f, 'T');
  fifo_putc(&f, 'e');
  fifo_putc(&f, 's');
  fifo_putc(&f, 't');
  ASSERT_EQ(fifo_data_slots(&f), 4);
  fifo_getc(&f, &c);
  ASSERT_EQ(c, 'T');
  fifo_getc(&f, &c);
  ASSERT_EQ(c, 'e');
  fifo_getc(&f, &c);
  ASSERT_EQ(c, 's');
  fifo_getc(&f, &c);
  ASSERT_EQ(c, 't');
}

UTEST(fifo, get_many)
{
  DEFINE_FIFO(f, 20);
  char *msg = "Test str";
  uint8_t result[20];

  fifo_put(&f, (uint8_t *)msg, sizeof(msg));
  fifo_get(&f, result, 4);
  ASSERT_EQ(memcmp((char *)result, "Test", 4), 0);
  fifo_get(&f, result, 3);
  ASSERT_EQ(memcmp((char *)result, " st", 3), 0);
}

UTEST(fifo, full)
{
  DEFINE_FIFO(f, 20);

  for (int i=0; i<19; i++)
      fifo_putc(&f, i + 'a');

  ASSERT_EQ(19, fifo_data_slots(&f));
  ASSERT_EQ(0, fifo_free_slots(&f));
  ASSERT_EQ(1, fifo_full(&f));
}

UTEST(fifo, full2)
{
  DEFINE_FIFO(f, 20);
  uint8_t c;

  for (int i=0; i<19; i++)
      fifo_putc(&f, i);

  for (int i=0; i<10; i++)
      fifo_getc(&f, &c);

  for (int i=0; i<10; i++)
      fifo_putc(&f, i);

  ASSERT_EQ(19, fifo_data_slots(&f));
  ASSERT_EQ(0, fifo_free_slots(&f));
  ASSERT_EQ(1, fifo_full(&f));
}
