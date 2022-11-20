#include "hw.h"
#include "alloc.h"

static dlist_t pool_list;
static uint32_t total_allocs;
static uint32_t total_frees;

static void alloc_mempool_init(alloc_pool_t *pool);
static void *alloc_mempool_get(alloc_pool_t *pool);
static void alloc_mempool_free(alloc_pool_t *pool, dnode_t *node);

void alloc_init(void)
{
  dlist_init(&pool_list);
}

void alloc_mempool_add(alloc_pool_t *pool)
{
  alloc_mempool_init(pool);
  dlist_append(&pool_list, (dnode_t *)pool);
}

void *alloc_get(uint32_t size)
{
  dnode_t *node;

  DLIST_FOR_EACH(&pool_list, node) {
    alloc_pool_t *pool = (alloc_pool_t *)node;
    if ((pool->chunk_size >= size) &&
        (!dlist_empty(&pool->free))) {
      return alloc_mempool_get(pool);
    }
  }
  return NULL;
}

void alloc_free(void *ptr)
{
  uint8_t *data = (uint8_t *)ptr;
  alloc_pool_chunk_t *chunk = (alloc_pool_chunk_t *)CONTAINER_OF(data, alloc_pool_chunk_t, data);
  alloc_pool_t *pool = chunk->parent;

  alloc_mempool_free(pool, (dnode_t *)chunk);
}

uint32_t alloc_total_allocs(void)
{
  return total_allocs;
}

uint32_t alloc_total_frees(void)
{
  return total_frees;
}

static void alloc_mempool_init(alloc_pool_t *pool)
{
  dlist_init(&pool->free);
  dlist_init(&pool->taken);

  uint8_t *ptr = pool->buffer;
  for (uint32_t i=0; i<pool->chunks; i++) {
    dlist_append(&pool->free, (dnode_t *)ptr);
    ((alloc_pool_chunk_t *)ptr)->parent = pool;
    ptr += pool->chunk_size + sizeof(alloc_pool_chunk_t);
  }
}

static void *alloc_mempool_get(alloc_pool_t *pool)
{
  __disable_irq();
  dnode_t *node = dlist_get_head(&pool->free);
  dlist_append(&pool->taken, node);
  total_allocs++;
  __enable_irq();
  return ((alloc_pool_chunk_t *)node)->data;
}

static void alloc_mempool_free(alloc_pool_t *pool, dnode_t *node)
{
  __disable_irq();
  dlist_remove(&pool->taken, node);
  dlist_append(&pool->free, node);
  total_frees++;
  __enable_irq(); 
}