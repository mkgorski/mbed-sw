
#include <stdint.h>
#include <string.h>
#include "logging.h"
#include "alloc.h"
#include "sltp.h"

int sltp_init(sltp_t *sltp, sltp_transport_ctx_t *transport)
{
  sltp->transport = transport;

  sltp->proto = (sltp_proto_t *)alloc_get(sizeof(sltp_proto_t));
  if (!sltp->proto) {
    log("Failed to allocate space for sltp!\n");
    return -1;
  }

  proto_init(sltp->proto, transport);
  sltp->proto->parent = sltp;
  return 0;
}

void sltp_poll(sltp_t *sltp)
{
  proto_poll(sltp->proto);
}

int sltp_send(sltp_t *sltp, uint8_t *buf, uint32_t len)
{
  proto_send(sltp->proto, buf, len);
  return (int)len;
}

void sltp_set_data_callback(sltp_t *sltp, sltp_data_cb user_cb, void *userdata)
{
  sltp->user_cb = user_cb;
  sltp->userdata = userdata;
}
