#include <string.h>
#include "sltp.h"
#include "logging.h"

#define CHAR_END 0xc0
#define CHAR_ESC 0xdb
#define CHAR_ESC_END 0xdc
#define CHAR_ESC_ESC 0xdd

static void packetizer_event_cb(transport_event_t *event, void *userdata)
{
  sltp_packetizer_t *self = (sltp_packetizer_t *)userdata;

  switch(event->type) {
    case TRANSPORT_TX_ABORTED:
      log("Transport TX ERROR\n");
      // fall through
    case TRANSPORT_TX_DONE:
      ss_init(&self->output, self->obuf, sizeof(self->obuf));
      self->tx_ready = true;
      break;
    case TRANSPORT_RX_RDY:
      packetizer_feed(self, event->data.rx.buf + event->data.rx.offset, event->data.rx.len);
      break;
    default:
      break;    
  }
}

void packetizer_init(sltp_packetizer_t *self, sltp_transport_ctx_t *transport)
{
  memset(self, 0, sizeof(sltp_packetizer_t));
  self->escaped = false;
  self->tx_ready = true;
  ss_init(&self->output, self->obuf, sizeof(self->obuf));
  ostring_putc(&self->output, CHAR_END); // separate garbage prior init
  self->transport = transport;
  self->transport->userdata = self;
  self->transport->event_cb = packetizer_event_cb;
}

static int __max(int a, int b)
{
  return (a > b) ? a : b;
}

bool packetizer_write(sltp_packetizer_t *self, uint8_t *buf, uint32_t len)
{
  if (!self->tx_ready) return false;

  // hack, assume min(20, 1/8th buffer) characters are enough for special chars
  int required_space = (int)(len + __max(20, len>>3));
  if (required_space > PACKETIZER_BUF_SIZE) {
    log("fatal: packetizer buffer too small!\n");
    return false;
  }

  if (ostring_bytes_left(&self->output) < required_space) return false;

  for (uint32_t i=0; i<len; i++) {
    switch(buf[i]) {
      case CHAR_END:
        ostring_putc(&self->output, CHAR_ESC);
        ostring_putc(&self->output, CHAR_ESC_END);
        break;
      case CHAR_ESC:
        ostring_putc(&self->output, CHAR_ESC);
        ostring_putc(&self->output, CHAR_ESC_ESC);
        break;
      default:
        ostring_putc(&self->output, buf[i]);
    }
  }

  int ok = ostring_putc(&self->output, CHAR_END);
  if (!ok) {
    log("fatal: packetizer packet not complete!\n");
    return false;
  }

  return true;
}

void packetizer_poll_out(sltp_packetizer_t *self)
{
  if (!self->tx_ready) return;
  if (self->output.p == 0) return;

  int ret = self->transport->write(self->output.s, self->output.p);
  if (ret < 0) {
    log("uart tx failed. err=%d\n", ret);
    self->tx_ready = true;
  }
  else if (ret == self->output.p) {
    // sent immediately
    ss_init(&self->output, self->obuf, sizeof(self->obuf));
    self->tx_ready = true;
  }
  else {
    self->tx_ready = false;
  }
}

void packetizer_feed(sltp_packetizer_t *self, uint8_t *buf, uint32_t len)
{
  uint8_t *dest = &self->ibuf[self->iptr];

  for (uint32_t i=0; i<len; i++) {
    if (self->escaped) {
      if (*buf == CHAR_ESC_ESC) *dest++ = CHAR_ESC;
      if (*buf == CHAR_ESC_END) *dest++ = CHAR_END;
      self->escaped = false;
    }
    else if (*buf == CHAR_ESC) {
      self->escaped = true;
    }
    else if (*buf == CHAR_END) {
      sltp_proto_t *proto = CONTAINER_OF(self, sltp_proto_t, packetizer);
      proto_feed(proto, self->ibuf, dest - self->ibuf);
      dest = self->ibuf;
    }
    else {
      *dest++ = *buf;
    }
    buf++;
  }
  self->iptr = dest - self->ibuf;
}
