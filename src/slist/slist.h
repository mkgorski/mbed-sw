#ifndef _SLIST_H_
#define _SLIST_H_

#include <stdint.h>
#include "mbed-common.h"

typedef struct node_t {
  struct node_t *next;
} node_t;

typedef struct slist_t {
  node_t *head;
  node_t *tail;
  uint32_t size;
} slist_t;

#define SLIST_FOR_EACH(sl, sn) for (sn = (sl)->head; sn; sn = sn->next)

void slist_init(slist_t *list);
int slist_empty(slist_t *list);
uint32_t slist_size(slist_t *list);
void slist_prepend(slist_t *list, node_t *node);
void slist_append(slist_t *list, node_t *node);
node_t *slist_peek_head(slist_t *list);
node_t *slist_peek_tail(slist_t *list);
node_t *slist_get(slist_t *list);
void slist_remove(slist_t *list, node_t *prev, node_t *node);
int slist_find_and_remove(slist_t *list, node_t *node);

#endif /* _SLIST_H_ */