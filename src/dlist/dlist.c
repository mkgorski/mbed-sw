#include "dlist.h"

void dlist_init(dlist_t *list)
{
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

int dlist_empty(dlist_t *list)
{
  return list->head == NULL ? 1 : 0;
}

uint32_t dlist_size(dlist_t *list)
{
  return list->size;
}

void dlist_prepend(dlist_t *list, dnode_t *node)
{
  node->next = list->head;
  node->prev = NULL;
  if (list->head) list->head->prev = node;

  list->head = node;
  if (!list->tail) list->tail = node;
  list->size++;
}

void dlist_append(dlist_t *list, dnode_t *node)
{
  node->next = NULL;
  node->prev = list->tail;
  if (list->tail) {
    list->tail->next = node;
    list->tail = node;
  }
  else {
    list->head = list->tail = node;
  }
  list->size++;
}

dnode_t *dlist_peek_head(dlist_t *list)
{
  return list->head;
}

dnode_t *dlist_peek_tail(dlist_t *list)
{
  return list->tail;
}

dnode_t *dlist_get_head(dlist_t *list)
{
  dnode_t *head = list->head;
  if (head) {
    list->head = head->next;
    if (list->head) list->head->prev = NULL;
    list->size--;
    if (!list->head) list->tail = NULL;
  }
  return head;
}

dnode_t *dlist_get_tail(dlist_t *list)
{
  dnode_t *tail = list->tail;
  if (tail) {
    list->tail = tail->prev;
    if (list->tail) list->tail->next = NULL;
    list->size--;
    if (!list->tail) list->head = NULL;
  }
  return tail;
}

void dlist_remove(dlist_t *list, dnode_t *node)
{
  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;

  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;

  list->size--;
}

int dlist_find_and_remove(dlist_t *list, dnode_t *node)
{
  dnode_t *current = list->head;

  while(current) {
    if (current == node) {
      dlist_remove(list, current);
      return 1;
    }
    current = current->next;
  }
  return 0;
}