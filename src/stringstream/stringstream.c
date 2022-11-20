#include <string.h>
#include "stringstream.h"

void ss_init(stringstream *ss, uint8_t *buf, int len)
{
  ss->p = 0;
  ss->n = len;
  ss->s = buf;
}

int ostring_putc(stringstream *ss, uint8_t c)
{
  if (!ss->n) return 0;
  ss->s[ss->p++] = c;
  ss->n--;
  return 1;
}

int ostring_put16be(stringstream *ss, uint16_t n)
{
  if (ss->n < 2) return 0;
  ss->s[ss->p++] = n >> 8;
  ss->s[ss->p++] = n;
  ss->n -= 2;
  return 2;
}

int ostring_put32be(stringstream *ss, uint32_t n)
{
  if (ss->n < 4) return 0;
  ss->s[ss->p++] = n >> 24;
  ss->s[ss->p++] = n >> 16;
  ss->s[ss->p++] = n >> 8;
  ss->s[ss->p++] = n;
  ss->n -= 4;
  return 4;
}

int ostring_put(stringstream *ss, const uint8_t *s, int n)
{
  if (n > ss->n) return 0;
  memcpy(ss->s + ss->p, s, n);
  ss->p += n; ss->n -= n;
  return n;
}

int ostring_bytes_left(stringstream *ss)
{
  return ss->n;
}

int istring_peek(stringstream *ss, uint8_t **c)
{
  *c = ss->s + ss->n;
  return ss->p;
}

int istring_load(stringstream *ss, uint8_t *s, int n)
{
  if (ss->p) {
    ss->n += ss->p; ss->p = 0;
  }
  if (n > ss->n) return 0;
  memcpy(ss->s + ss->n - n, s, n);
  ss->p = n;
  ss->n -= n;
  return n;
}

int istring_getc(stringstream *ss, uint8_t *c)
{
  if (!ss->p) return 0;
  ss->p--;
  *c = ss->s[ss->n++];
  return 1;
}

int istring_get16be(stringstream *ss, uint16_t *n)
{
  if (ss->p < 2) return 0;
  ss->p -= 2;
  *n = (ss->s[ss->n] << 8) + (ss->s[ss->n + 1]);
  ss->n += 2;
  return 2;
}

int istring_get32be(stringstream *ss, uint32_t *n)
{
  if (ss->p < 4) return 0;
  ss->p -= 4;
  *n = (ss->s[ss->n] << 24) + (ss->s[ss->n + 1] << 16) + (ss->s[ss->n + 2] << 8) + (ss->s[ss->n + 3]);
  ss->n += 4;
  return 4;
}

int istring_get(stringstream *ss, uint8_t *s, int n)
{
  if (n > ss->p) return 0;
  memcpy(s, ss->s + ss->n, n);
  ss->p -= n; ss->n += n;
  return n;
}

int istring_rewind(stringstream *ss, int n)
{
  if (n > ss->n) return 0;
  ss->p += n; ss->n -= n;
  return n;
}

uint8_t *istring_expose(stringstream *ss)
{
  return ss->s + ss->n;
}

int istring_bytes_left(stringstream *ss)
{
  return ss->p;
}