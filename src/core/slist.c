#include "slist.h"

void slist_init(slist_t *list)
{
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

int slist_empty(slist_t *list)
{
  return list->head == NULL ? 1 : 0;
}

uint32_t slist_size(slist_t *list)
{
  return list->size;
}

void slist_prepend(slist_t *list, node_t *node)
{
  node->next = list->head;
  list->head = node;
  if (!list->tail) list->tail = node;
  list->size++;
}

void slist_append(slist_t *list, node_t *node)
{
  if (list->tail) {
    list->tail->next = node;
    list->tail = node;
  }
  else {
    list->head = list->tail = node;
  }
  node->next = NULL;
  list->size++;
}

node_t *slist_peek_head(slist_t *list)
{
  return list->head;
}

node_t *slist_peek_tail(slist_t *list)
{
  return list->tail;
}

node_t *slist_get(slist_t *list)
{
  node_t *head = list->head;
  if (head) {
    list->head = head->next;
    list->size--;
    if (!list->head) list->tail = NULL;
  }
  return head;
}

void slist_remove(slist_t *list, node_t *prev, node_t *node)
{
  if (!prev) { // node == head
    list->head = node->next;
    if (!list->head) list->tail = NULL;
  }
  else if (!node->next) { // node == tail
    list->tail = prev;
    prev->next = NULL;
  }
  else {
    prev->next = node->next;
  }
  list->size--;
}

int slist_find_and_remove(slist_t *list, node_t *node)
{
  node_t *current = list->head;
  node_t *last = NULL;

  while(current) {
    if (current == node) {
      slist_remove(list, last, current);
      return 1;
    }
    last = current;
    current = current->next;
  }
  return 0;
}