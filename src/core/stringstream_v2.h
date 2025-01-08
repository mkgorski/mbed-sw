
#ifndef _STRINGSTREAM_V2_H_
#define _STRINGSTREAM_V2_H_

#include <stdint.h>

typedef struct stringstream {
  int16_t r;
  int16_t w;
  int16_t n;
  uint8_t *s;
} stringstream;

void ss_init(stringstream *ss, uint8_t *buf, int len);

int ss_putc(stringstream *ss, uint8_t c);
int ss_put16be(stringstream *ss, uint16_t n);
int ss_put32be(stringstream *ss, uint32_t n);
int ss_put(stringstream *ss, const uint8_t *s, int n);
int ss_capacity_left(stringstream *ss);
int ss_bytes_left(stringstream *ss);
int ss_getc(stringstream *ss, uint8_t *c);
int ss_get16be(stringstream *ss, uint16_t *n);
int ss_get32be(stringstream *ss, uint32_t *n);
int ss_get(stringstream *ss, uint8_t *s, int n);
uint8_t *ss_raw_buf(stringstream *ss);
int ss_rewind(stringstream *ss, int n);


#endif /* _STRINGSTREAM_V2_H_ */