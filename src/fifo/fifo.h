#ifndef _FIFO_H_
#define _FIFO_H_

#include <stdint.h>
#include "mbed-common.h"

#define elem_t uint8_t

typedef struct fifo {
  elem_t *w, *r;
  elem_t *const end;
  const unsigned len;
} fifo;

#define INIT_FIFO(bufname) \
  { .w = bufname, .r = bufname, .end = bufname + sizeof(bufname) / sizeof(elem_t), .len = sizeof(bufname) / sizeof(elem_t) }
#define DEFINE_FIFO(name, size) \
  elem_t _##name##_buffer[size]; \
  fifo name = INIT_FIFO(_##name##_buffer)

int fifo_data_slots (fifo *f);
int fifo_free_slots (fifo *f);
int fifo_full (fifo *f);
int fifo_empty (fifo *f);
int fifo_putc (fifo *f, elem_t c);
int fifo_put (fifo *f, elem_t *s, int n);
int fifo_getc (fifo *f, elem_t *c);
int fifo_peekc (fifo *f, elem_t *c);
int fifo_get (fifo *f, elem_t *s, int n);
int fifo_drop (fifo *f, int n);

#endif /* _FIFO_H */
