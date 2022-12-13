#ifndef _PROTO_H_
#define _PROTO_H_

#include <stdint.h>
#include "slist.h"
#include "timer.h"
#include "transport.h"
#include "packetizer.h"

struct sltp_t;

typedef enum {
  SLTP_DATA = 1,
  SLTP_KEEPALIVE,
  SLTP_RETRY,
  SLTP_RESET
} sltp_packet_type;

typedef struct __attribute__((packed,aligned(4))) sltp_frame_t {
  uint8_t type;
  uint8_t cnt;
  uint8_t reserved1;
  uint8_t reserved2;
  uint8_t payload[];
} sltp_frame_t;

typedef struct __attribute__((packed,aligned(4))) sltp_packet_t {
  node_t node;
  uint32_t len;
  sltp_frame_t frame;
} sltp_packet_t;

typedef struct sltp_proto_t {
  struct sltp_t *parent;
  slist_t input_list;
  slist_t output_list;
  slist_t sent_list;
  slist_t waiting_list;
  int32_t sent_list_size;
  timer timeout;
  timer keepalive_period;
  uint8_t in_cnt;
  uint8_t out_cnt;
  bool init;
  bool connected;
  sltp_packetizer_t packetizer;
} sltp_proto_t;

void proto_init(sltp_proto_t *self, sltp_transport_ctx_t *transport);
void proto_poll(sltp_proto_t *self);
void proto_feed(sltp_proto_t *self, uint8_t *buf, uint32_t len);
sltp_packet_t *proto_send(sltp_proto_t *self, uint8_t *buf, uint32_t len);


#endif /* _PROTO_H_ */