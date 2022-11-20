#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <stdint.h>
#include "mbed-common.h"
#include "dlist.h"

typedef struct alloc_pool_t {
  dnode_t node;
  uint32_t chunk_size;
  uint32_t chunks;
  dlist_t free;
  dlist_t taken;
  uint8_t *buffer;
} alloc_pool_t;

typedef struct alloc_pool_chunk_t {
  dnode_t node;
  alloc_pool_t *parent;
  uint8_t data[];
} alloc_pool_chunk_t;

#define DEFINE_ALLOC_MEMPOOL(name, _chunks, _chunk_size) \
  uint8_t _##name##_alloc_buffer[_chunks * (_chunk_size + sizeof(alloc_pool_chunk_t))] __attribute__ ((aligned(4))); \
  alloc_pool_t name = { .chunk_size = _chunk_size, .chunks = _chunks, .buffer = _##name##_alloc_buffer }

void alloc_init(void);
void alloc_mempool_add(alloc_pool_t *pool);
void *alloc_get(uint32_t size);
void alloc_free(void *ptr);

/* debug API */
uint32_t alloc_total_allocs(void);
uint32_t alloc_total_frees(void);

#endif /* _ALLOC_H_ */