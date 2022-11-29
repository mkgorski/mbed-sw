#ifndef _PACKETIZER_H_
#define _PACKETIZER_H_

#include "stringstream.h"

#define PACKETIZER_BUF_SIZE 512

typedef struct sltp_packetizer_t {
  uint8_t ibuf[PACKETIZER_BUF_SIZE];
  uint32_t iptr;
  uint8_t obuf[PACKETIZER_BUF_SIZE];
  sltp_transport_ctx_t *transport;
  stringstream output;
  bool escaped;
  bool tx_ready;
} sltp_packetizer_t;

void packetizer_init(sltp_packetizer_t *self, sltp_transport_ctx_t *transport);
bool packetizer_write(sltp_packetizer_t *self, uint8_t *buf, uint32_t len);
void packetizer_poll_out(sltp_packetizer_t *self);
void packetizer_feed(sltp_packetizer_t *self, uint8_t *buf, uint32_t len);

#endif /* _PACKETIZER_H_ */