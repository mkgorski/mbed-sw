#ifndef _SLTP_TRANSPORT_H_
#define _SLTP_TRANSPORT_H_

#include <stdint.h>

typedef enum {
  TRANSPORT_TX_DONE,
  TRANSPORT_TX_ABORTED,
  TRANSPORT_RX_RDY
} transport_event_type_t;

typedef struct uart_event_tx_t {
	uint8_t *buf;
	uint32_t len;
} uart_event_tx_t;

typedef struct uart_event_rx_t {
	uint8_t *buf;
	uint32_t offset;
	uint32_t len;
} uart_event_rx_t;

typedef struct transport_event_t {
  transport_event_type_t type;
  union transport_event_data {
    uart_event_tx_t tx;
    uart_event_rx_t rx;
  } data;
} transport_event_t;

typedef struct sltp_transport_ctx_t {
  void *userdata;
  int (*write)(uint8_t *buf, uint32_t len);
  void (*event_cb)(transport_event_t *event, void *userdata);
} sltp_transport_ctx_t;

sltp_transport_ctx_t *sltp_transport_get(void);
void transport_deinit(void);

#endif /* _SLTP_TRANSPORT_H_ */