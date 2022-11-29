#ifndef _SLTP_H_
#define _SLTP_H_

#include "transport.h"
#include "proto.h"

typedef enum {
  SLTP_EVENT_CONNECTED,
  SLTP_EVENT_DISCONNECTED,
  SLTP_EVENT_DATA
} sltp_event_type_t;

typedef void (*sltp_data_cb)(sltp_event_type_t event_type, uint8_t *buf, uint32_t len, void *userdata);

typedef struct sltp_t {
  sltp_data_cb user_cb;
  sltp_transport_ctx_t *transport;
  sltp_proto_t *proto;
  void *userdata;
} sltp_t;

int sltp_init(sltp_t *sltp, sltp_transport_ctx_t *transport);
void sltp_poll(sltp_t *sltp);
int sltp_send(sltp_t *sltp, uint8_t *buf, uint32_t len);
void sltp_set_data_callback(sltp_t *sltp, sltp_data_cb user_cb, void *userdata);

#endif /* _SLTP_H_ */