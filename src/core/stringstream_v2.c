#include <string.h>
#include "stringstream_v2.h"

void ss_init(stringstream *ss, uint8_t *buf, int len)
{
  ss->r = 0;
  ss->w = 0;
  ss->n = len;
  ss->s = buf;
}

int ss_putc(stringstream *ss, uint8_t c)
{
  if (ss->n - ss->w > 0) {
    ss->s[ss->w++] = c;
    return 1;
  }
  return 0;
}

int ss_put16be(stringstream *ss, uint16_t n)
{
  if (ss->n - ss->w > 1) {
    ss->s[ss->w++] = n >> 8;
    ss->s[ss->w++] = n;
    return 2;
  }
  return 0;
}

int ss_put32be(stringstream *ss, uint32_t n)
{
  if (ss->n - ss->w > 3) {
    ss->s[ss->w++] = n >> 24;
    ss->s[ss->w++] = n >> 16;
    ss->s[ss->w++] = n >> 8;
    ss->s[ss->w++] = n;
    return 4;
  }
  return 0;
}

int ss_put(stringstream *ss, const uint8_t *s, int n)
{
  if (ss->n - ss->w >= n) {
    memcpy(ss->s + ss->w, s, n);
    ss->w += n;
    return n;
  }
  return 0;
}

int ss_capacity_left(stringstream *ss)
{
  return ss->n - ss->w;
}

int ss_bytes_left(stringstream *ss)
{
  return ss->w - ss->r;
}

int ss_getc(stringstream *ss, uint8_t *c)
{
  if (ss->w - ss->r > 0) {
    *c = ss->s[ss->r++];
    return 1;
  }
  return 0;
}

int ss_get16be(stringstream *ss, uint16_t *n)
{
  if (ss->w - ss->r > 1) {
    *n = (ss->s[ss->r] << 8) + (ss->s[ss->r + 1]);
    ss->r += 2;
    return 2;
  }
  return 0;
}

int ss_get32be(stringstream *ss, uint32_t *n)
{
  if (ss->w - ss->r > 4) {
    *n = (ss->s[ss->r] << 24) + (ss->s[ss->r + 1] << 16) + (ss->s[ss->r + 2] << 8) + (ss->s[ss->r + 3]);
    ss->r += 4;
    return 4;
  }
  return 0;
}

int ss_get(stringstream *ss, uint8_t *s, int n)
{
  if (ss->w - ss->r >= n) {
    memcpy(s, ss->s + ss->r, n);
    ss->r += n;
    return n;
  }
  return 0;
}

uint8_t *ss_raw_buf(stringstream *ss)
{
  return ss->s;
}

int ss_rewind(stringstream *ss, int n)
{
  if (n > 0 && ss->r + n <= ss->w) { // rewind forward
    ss->r += n;
    return n;
  }
  if (n < 0 && ss->r + n >= 0) { // rewind backward
    ss->r += n;
    return n;
  }
  return 0;
}
