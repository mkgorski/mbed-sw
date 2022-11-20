
#ifndef _STRINGSTREAM_H_
#define _STRINGSTREAM_H_

#include <stdint.h>

typedef struct stringstream {
  uint16_t p;
  uint16_t n;
  uint8_t *s;
} stringstream;

void ss_init(stringstream *ss, uint8_t *buf, int len);

int ostring_putc(stringstream *ss, uint8_t c);
int ostring_put16be(stringstream *ss, uint16_t n);
int ostring_put32be(stringstream *ss, uint32_t n);
int ostring_put(stringstream *ss, const uint8_t *s, int n);
int ostring_bytes_left(stringstream *ss);

int istring_load(stringstream *ss, uint8_t *s, int n);
int istring_peek(stringstream *ss, uint8_t **c);
int istring_getc(stringstream *ss, uint8_t *c);
int istring_get16be(stringstream *ss, uint16_t *n);
int istring_get32be(stringstream *ss, uint32_t *n);
int istring_get(stringstream *ss, uint8_t *s, int n);
int istring_rewind(stringstream *ss, int n);
uint8_t *istring_expose(stringstream *ss);
int istring_bytes_left(stringstream *ss);

#endif /* _STRINGSTREAM_H_ */