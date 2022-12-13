#include <string.h>
#include "hw.h"
#include "sltp.h"
#include "alloc.h"
#include "logging.h"


/*
 * Private functions prototypes
 */
static uint8_t proto_checksum(sltp_proto_t *self, sltp_packet_t *pkt);
static sltp_packet_t *proto_find_packet(slist_t *list, uint8_t cnt);
static sltp_packet_t *proto_find_and_remove_packet(slist_t *list, uint8_t cnt);
static sltp_packet_t *proto_raw_send(sltp_proto_t *self, sltp_packet_type type, uint8_t *buf, uint32_t len);
static sltp_packet_t *proto_send_keepalive(sltp_proto_t *self);
static sltp_packet_t *proto_send_retry(sltp_proto_t *self, uint8_t cnt);
static sltp_packet_t *proto_send_reset(sltp_proto_t *self);
static bool proto_peek_waiting_list(sltp_proto_t *self);
static void proto_reset(sltp_proto_t *self);
static void proto_process(sltp_proto_t *self, sltp_packet_t *pkt);
static bool proto_process_data(sltp_proto_t *self, sltp_packet_t *pkt);
static void proto_poll_in(sltp_proto_t *self);
static void proto_poll_out(sltp_proto_t *self);
static void proto_publish(sltp_proto_t *self, sltp_packet_t *pkt);
// static void proto_prof(sltp_proto_t *self);

/*
 * Public API
 */
void proto_init(sltp_proto_t *self, sltp_transport_ctx_t *transport)
{
  memset(self, 0, sizeof(sltp_proto_t));
  slist_init(&self->input_list);
  slist_init(&self->output_list);
  slist_init(&self->sent_list);
  slist_init(&self->waiting_list);
  self->init = true;
  timer_start(&self->timeout, 500);
  timer_start(&self->keepalive_period, 20);
  packetizer_init(&self->packetizer, transport);

  proto_send_reset(self);
}

void proto_poll(sltp_proto_t *self)
{
  if (timer_expired(&self->timeout)) {
    timer_restart(&self->timeout);
    proto_reset(self);
  }

  if (timer_expired(&self->keepalive_period)) {
    timer_start(&self->keepalive_period, 200);
    proto_send_keepalive(self);
  }

  proto_poll_out(self);
  packetizer_poll_out(&self->packetizer);
  proto_poll_in(self);
}

void proto_feed(sltp_proto_t *self, uint8_t *buf, uint32_t len)
{
  sltp_packet_t *pkt = (sltp_packet_t *)alloc_get(sizeof(sltp_packet_t) + len);
  if (!pkt) {
    log("Out of memory!\n");
    return;
  }

  memcpy(&pkt->frame, buf, len);
  pkt->len = len;

  slist_append(&self->input_list, (node_t *)pkt);
}

sltp_packet_t *proto_send(sltp_proto_t *self, uint8_t *buf, uint32_t len)
{
  return proto_raw_send(self, SLTP_DATA, buf, len);
}

/*
 * Private functions
 */
static uint8_t proto_checksum(sltp_proto_t *self, sltp_packet_t *pkt)
{
  (void)self;

  uint8_t *buf = (uint8_t *)&pkt->frame;
  uint8_t chksum = *buf++;
  for (uint32_t i=1; i<pkt->len; i++) {
    chksum ^= *buf++;
  }
  return chksum;
}

static sltp_packet_t *proto_find_packet(slist_t *list, uint8_t cnt)
{
  node_t *node;
  SLIST_FOR_EACH(list, node) {
    sltp_packet_t *rpkt = (sltp_packet_t *)node;
    if (rpkt->frame.cnt == cnt) {
      return rpkt;
    }
  }
  return NULL;
}

static sltp_packet_t *proto_find_and_remove_packet(slist_t *list, uint8_t cnt)
{
  node_t *node;
  node_t *prev_node = NULL;

  SLIST_FOR_EACH(list, node) {
    sltp_packet_t *rpkt = (sltp_packet_t *)node;
    if (rpkt->frame.cnt == cnt) {
      slist_remove(list, prev_node, node);
      return rpkt;
    }
    prev_node = node;
  }
  return NULL;
}

static sltp_packet_t *proto_raw_send(sltp_proto_t *self, sltp_packet_type type, uint8_t *buf, uint32_t len)
{
  sltp_packet_t *pkt = (sltp_packet_t *)alloc_get(sizeof(sltp_packet_t) + len + 2);
  if (!pkt) {
    log("Out of memory!\n");
    return NULL;
  }

  pkt->frame.type = type;
  pkt->frame.cnt = self->out_cnt;
  pkt->frame.reserved1 = 0;
  pkt->frame.reserved2 = 0;
  if (len > 0)
    memcpy(&pkt->frame.payload[0], buf, len);
  pkt->len = sizeof(sltp_frame_t) + len;
  pkt->frame.payload[len] = proto_checksum(self, pkt);
  pkt->len++;

  if (pkt->frame.type == SLTP_DATA) {
    self->out_cnt++;
    slist_append(&self->output_list, (node_t *)pkt);
  }
  else {
    slist_prepend(&self->output_list, (node_t *)pkt);
  }

  return pkt;
}

static sltp_packet_t *proto_send_keepalive(sltp_proto_t *self)
{
  return proto_raw_send(self, SLTP_KEEPALIVE, NULL, 0);
}

static sltp_packet_t *proto_send_retry(sltp_proto_t *self, uint8_t cnt)
{
  return proto_raw_send(self, SLTP_RETRY, &cnt, 1);
}

static sltp_packet_t *proto_send_reset(sltp_proto_t *self)
{
  return proto_raw_send(self, SLTP_RESET, NULL, 0);
}

static bool proto_peek_waiting_list(sltp_proto_t *self)
{
  sltp_packet_t *pkt = proto_find_packet(&self->waiting_list, self->in_cnt + 1);
  if (pkt) {
    self->in_cnt = pkt->frame.cnt;
    slist_find_and_remove(&self->waiting_list, (node_t *)pkt);
    proto_publish(self, pkt);
    alloc_free(pkt);
    return true;
  }
  return false;
}

static void proto_reset(sltp_proto_t *self)
{
  node_t *node;

  if (self->connected) {
    self->connected = false;

    if (self->parent->user_cb)
      self->parent->user_cb(SLTP_EVENT_DISCONNECTED, NULL, 0, self->parent->userdata);
  }

  self->in_cnt = 0;
  self->out_cnt = 1;
  self->sent_list_size = 0;
  SLIST_FOR_EACH(&self->input_list, node) { alloc_free(node); }
  SLIST_FOR_EACH(&self->output_list, node) { alloc_free(node); }
  SLIST_FOR_EACH(&self->sent_list, node) { alloc_free(node); }
  SLIST_FOR_EACH(&self->waiting_list, node) { alloc_free(node); }
  slist_init(&self->input_list);
  slist_init(&self->output_list);
  slist_init(&self->sent_list);
  slist_init(&self->waiting_list);
  self->init = true;
}

static bool proto_process_data(sltp_proto_t *self, sltp_packet_t *pkt)
{

  if (self->init) {
    self->in_cnt = pkt->frame.cnt - 1;
    self->init = false;
  }

  uint8_t gap = pkt->frame.cnt - self->in_cnt;
  // log("packet validated, type=%d, cnt=%d, gap=%d\n", pkt->frame.type, pkt->frame.cnt, gap);
  if (gap == 1) {
    self->in_cnt = pkt->frame.cnt;
    proto_publish(self, pkt);
    if (!slist_empty(&self->waiting_list)) {
      while (proto_peek_waiting_list(self));
    }
  }
  else if (gap > 1) {
    if (gap > 240) {
      // packet from the past, ignore it
    }
    else if (gap > 5) {
      log("sltp frame gap=%d. Packets lost\n", gap);
      proto_reset(self);
    }
    else {
      for (int i=1; i<gap; i++) {
        if (!proto_find_packet(&self->waiting_list, self->in_cnt + i)) {
          proto_send_retry(self, self->in_cnt + i);
        }
      }
      slist_append(&self->waiting_list, (node_t *)pkt);
      return false;
    }
  }
  else {
    // gap = 0, ignore
  }
  return true;
}

static void proto_process(sltp_proto_t *self, sltp_packet_t *pkt)
{
  if (pkt->len < 5) {
    log("sltp packet too short\n");
    goto _process_finish;
  }

  if (proto_checksum(self, pkt) != 0) {
    log("sltp invalid checksum\n");
    goto _process_finish;
  }

  switch (pkt->frame.type) {
    case SLTP_DATA: {
      bool processed = proto_process_data(self, pkt);
      if (!processed) pkt = NULL; // prevent free
      timer_restart(&self->timeout);
      break;
    }
    case SLTP_KEEPALIVE:
      if (!self->connected) {
        self->connected = true;
        if (self->parent->user_cb)
          self->parent->user_cb(SLTP_EVENT_CONNECTED, NULL, 0, self->parent->userdata);
      }
      timer_restart(&self->timeout);
      break;
    case SLTP_RETRY: {
      sltp_packet_t *spkt = proto_find_and_remove_packet(&self->sent_list, pkt->frame.payload[0]);
      if (spkt) {
        self->sent_list_size--;
        slist_prepend(&self->output_list, (node_t *)spkt);
      }
      break;
    }
    case SLTP_RESET: {
      proto_reset(self);
      break;
    }
    default:
      log("sltp unsupported packet type=%d\n", pkt->frame.type);
      break;
  }

_process_finish:
  if (pkt)
    alloc_free(pkt);
}

static void proto_poll_in(sltp_proto_t *self)
{
  if (!slist_empty(&self->input_list)) {
    __disable_irq();
    sltp_packet_t *pkt = (sltp_packet_t *)slist_get(&self->input_list);
    __enable_irq();

    if (!pkt) return;
    proto_process(self, pkt);
  }
}

static void proto_poll_out(sltp_proto_t *self)
{
  sltp_packet_t *pkt;
  while ((pkt = (sltp_packet_t *)slist_peek_head(&self->output_list))) {
    if (packetizer_write(&self->packetizer, (uint8_t *)&pkt->frame, pkt->len)) {
      slist_remove(&self->output_list, NULL, (node_t *)pkt);
      if (pkt->frame.type == SLTP_DATA) {
        slist_append(&self->sent_list, (node_t *)pkt);
        if (++self->sent_list_size > 10) {
          pkt = (sltp_packet_t *)slist_get(&self->sent_list);
          if (pkt) {
            self->sent_list_size--;
            alloc_free(pkt);
          }
        }
      }
      else {
        alloc_free(pkt);
      }
    }
    else {
      // packetizer is not able to send data
      break;
    }
  }
}

static void proto_publish(sltp_proto_t *self, sltp_packet_t *pkt)
{
  sltp_t *sltp = self->parent;
  if (sltp->user_cb) {
    sltp->user_cb(SLTP_EVENT_DATA, &pkt->frame.payload[0], pkt->len - sizeof(sltp_frame_t) - 1, sltp->userdata);
  }
}

/* degug */
// static void proto_prof(sltp_proto_t *self)
// {
//   node_t *node;
//   int icnt = 0;
//   int ocnt = 0;
//   int scnt = 0;
//   int wcnt = 0;
//   SLIST_FOR_EACH(&self->input_list, node) { icnt++; }
//   SLIST_FOR_EACH(&self->output_list, node) { ocnt++; }
//   SLIST_FOR_EACH(&self->sent_list, node) { scnt++; }
//   SLIST_FOR_EACH(&self->waiting_list, node) { wcnt++; }
//   log("sltp ilen=%d olen=%d slen=%d wlen=%d\n", icnt, ocnt, scnt, wcnt);
// }
