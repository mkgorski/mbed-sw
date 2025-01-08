#ifndef _ALLOC2_H_
#define _ALLOC2_H_

#include <stdint.h>
#include "mbed-common.h"
#include "dlist.h"

typedef struct alloc2_pool_t {
  dlist_t free;
  dlist_t taken;
  uint32_t size;
  uint8_t *buffer;
} alloc2_pool_t;

typedef struct alloc2_pool_chunk_t {
  dnode_t node;
  uint32_t size;
  uint32_t _padding;
  uint8_t data[];
} alloc2_pool_chunk_t;
_Static_assert(offsetof(alloc2_pool_chunk_t, data) % 8 == 0, "data field must be 8-byte aligned");

#define DEFINE_ALLOC2_MEMPOOL(name, _size) \
  _Static_assert(((_size) % 8) == 0, "memory pool buffer size must be multiple of 8"); \
  uint8_t _##name##_alloc_buffer[_size] __attribute__ ((aligned(8))); \
  alloc2_pool_t name = { .size = _size,  .buffer = _##name##_alloc_buffer }

void alloc2_init(alloc2_pool_t *pool);
void *alloc2_get(uint32_t size);
void alloc2_free(void *ptr);
int alloc2_gc(void);

/* debug API */
uint32_t alloc2_total_allocs(void);
uint32_t alloc2_total_frees(void);

#endif /* _ALLOC2_H_ */