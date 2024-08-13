#include "hw.h"
#include "alloc2.h"

#define ALIGN8(x) (((x) + 0x07) & ~0x07)
#define MIN_ALLOC_SIZE 32
#define CHUNK_OVERHEAD ALIGN8(offsetof(alloc2_pool_chunk_t, data))

static alloc2_pool_t *_pool;
static uint32_t total_allocs;
static uint32_t total_frees;

static int _alloc_gc(alloc2_pool_chunk_t *start);

static inline alloc2_pool_chunk_t *extract_chunk(alloc2_pool_chunk_t *chk, uint32_t size)
{
  alloc2_pool_chunk_t *result = (alloc2_pool_chunk_t *)(chk->data + chk->size - size);
  chk->size -= size;
  result->size = size - CHUNK_OVERHEAD;
  return result;
}

void alloc2_init(alloc2_pool_t *pool)
{
  _pool = pool;
  dlist_init(&_pool->free);
  dlist_init(&_pool->taken);

  alloc2_pool_chunk_t *chk = (alloc2_pool_chunk_t *)_pool->buffer;
  chk->size = _pool->size - CHUNK_OVERHEAD;
  dlist_append(&_pool->free, (dnode_t *)chk);

  total_allocs = 0;
  total_frees = 0;
}

void *alloc2_get(uint32_t size)
{
  dnode_t *node;

  size = ALIGN8(size);
  if (size < MIN_ALLOC_SIZE)
    size = MIN_ALLOC_SIZE;
  size += CHUNK_OVERHEAD;

  DLIST_FOR_EACH(&_pool->free, node) {
    alloc2_pool_chunk_t *chk = (alloc2_pool_chunk_t *)node;
    if (chk->size + CHUNK_OVERHEAD >= size) {
      __disable_irq();
      if ((int32_t)(chk->size - size) < MIN_ALLOC_SIZE) {
        dlist_remove(&_pool->free, node);
      }
      else {
        chk = extract_chunk(chk, size);
      }
      dlist_append(&_pool->taken, (dnode_t *)chk);
      total_allocs++;
      __enable_irq();
      return chk->data;
    }
  }

  return NULL;
}

void alloc2_free(void *ptr)
{
  dnode_t *node;
  uint8_t *data = (uint8_t *)ptr;
  alloc2_pool_chunk_t *freed = (alloc2_pool_chunk_t *)CONTAINER_OF(data, alloc2_pool_chunk_t, data);

  __disable_irq();
  dlist_remove(&_pool->taken, &freed->node);

  DLIST_FOR_EACH(&_pool->free, node) {
    alloc2_pool_chunk_t *chk = (alloc2_pool_chunk_t *)node;
    if (freed < chk) {
      dlist_insert_before(&_pool->free, &freed->node, &chk->node);
      goto _finish_free;
    }
  }

  dlist_append(&_pool->free, &freed->node);

_finish_free:
  total_frees++;
  _alloc_gc(freed);
  __enable_irq(); 
}

uint32_t alloc2_total_allocs(void)
{
  return total_allocs;
}

uint32_t alloc2_total_frees(void)
{
  return total_frees;
}

static int _alloc_gc(alloc2_pool_chunk_t *start)
{
  dnode_t *node = start->node.prev ? start->node.prev : &start->node;
  int i = 2;

  if (!node) return 0;

  while (i-- && node->next) {
    alloc2_pool_chunk_t *chk = (alloc2_pool_chunk_t *)node;

    if (chk->data + chk->size == (uint8_t *)chk->node.next) {
      chk->size += ((alloc2_pool_chunk_t *)chk->node.next)->size + CHUNK_OVERHEAD;
      dlist_remove(&_pool->free, chk->node.next);
    }
    else {
      node = node->next;
    }
  }
  return 0;
}
