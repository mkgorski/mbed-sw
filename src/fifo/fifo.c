#include "fifo.h"

int fifo_data_slots (fifo *f)
{
  int l = f->w - f->r;
  if (l < 0) l += f->len;
  return l;
}

int fifo_free_slots (fifo *f)
{
  int l = f->r - f->w;
  if (l <= 0) l += f->len;
  return l - 1;
}

int fifo_full (fifo *f)
{
  int l = f->r - f->w;
  if(l == 1) return 1;
  if(f->len + l == 1) return 1; // r == start && w == end
  return 0;
}

int fifo_empty (fifo *f)
{
  return f->r == f->w;
}

int fifo_putc (fifo *f, elem_t c)
{
  if (fifo_full (f)) return 0;
  elem_t *x = f->w;
  *x++ = c;
  if (x == f->end) x -= f->len;
  f->w = x;
  return 1;
}

int fifo_put (fifo *f, elem_t *s, int n)
{
  if (fifo_free_slots (f) < n) return 0;
  int clen = f->end - f->w;
  if (n <= clen) {
    elem_t *x = f->w;
    int i = n;
    while (i--) *x++ = *s++;
    if (x == f->end) x -= f->len;
    f->w = x;
  } else {
    fifo_put (f, s, clen);
    fifo_put (f, s + clen, n - clen);
  }
  return n;
}

int fifo_getc (fifo *f, elem_t *c)
{
  if (fifo_empty (f)) return 0;
  elem_t *x = f->r;
  *c = *x++;
  if (x == f->end) x -= f->len;
  f->r = x;
  return 1;
}

int fifo_peekc (fifo *f, elem_t *c)
{
  if (fifo_empty (f)) return 0;
  *c = *f->r;
  return 1;
}

int fifo_get (fifo *f, elem_t *s, int n)
{
  int a = fifo_data_slots (f);
  if (a > n) a = n;
  int clen = f->end - f->r;
  if (a <= clen) {
    elem_t *x = f->r;
    int i = a;
    while (i--) *s++ = *x++;
    if (x == f->end) x -= f->len;
    f->r = x;
  } else {
    fifo_get (f, s, clen);
    fifo_get (f, s + clen, a - clen);
  }
  return a;
}

int fifo_drop (fifo *f, int n)
{
  int a = fifo_data_slots (f);
  if (n > a) n = a;
  elem_t *x = f->r;
  x += n;
  if (x > f->end) x = x - f->len;
  return n;
}
