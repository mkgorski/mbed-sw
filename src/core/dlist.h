#ifndef _DLIST_H_
#define _DLIST_H_

#include <stdint.h>
#include "mbed-common.h"

typedef struct dnode_t {
  struct dnode_t *next;
  struct dnode_t *prev;
} dnode_t;

typedef struct dlist_t {
  dnode_t *head;
  dnode_t *tail;
  uint32_t size;
} dlist_t;

#define DLIST_FOR_EACH(dl, dn) for (dn = (dl)->head; dn; dn = dn->next)

void dlist_init(dlist_t *list);
int dlist_empty(dlist_t *list);
uint32_t dlist_size(dlist_t *list);
void dlist_prepend(dlist_t *list, dnode_t *node);
void dlist_append(dlist_t *list, dnode_t *node);
dnode_t *dlist_peek_head(dlist_t *list);
dnode_t *dlist_peek_tail(dlist_t *list);
dnode_t *dlist_get_head(dlist_t *list);
dnode_t *dlist_get_tail(dlist_t *list);
void dlist_remove(dlist_t *list, dnode_t *node);
int dlist_find_and_remove(dlist_t *list, dnode_t *node);
int dlist_insert_before(dlist_t *list, dnode_t *what, dnode_t *where);

#endif /* _DLIST_H_ */