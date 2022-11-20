#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "alloc.h"
#include "utest.h"

DEFINE_ALLOC_MEMPOOL(pool32, 4, 32);
DEFINE_ALLOC_MEMPOOL(pool128, 2, 128);

/* helpers */
void *_get_raw_buffer_slot(alloc_pool_t *pool, uint32_t slot)
{
  alloc_pool_chunk_t *chunk = (alloc_pool_chunk_t *)&pool->buffer[slot * (pool->chunk_size + sizeof(alloc_pool_chunk_t))];
  return chunk->data;
}

int _is_within_pool(alloc_pool_t *pool, void *p)
{
  uint8_t *ptr = (uint8_t *)p;
  return (ptr >= pool->buffer && ptr < pool->buffer + pool->chunks * (pool->chunk_size + sizeof(alloc_pool_chunk_t)));
}

static inline void *random_alloc(alloc_pool_t **target_pool)
{
  uint32_t size = rand() % 2 ? 20 : 80;
  alloc_pool_t *pool = size < 32 ? &pool32 : &pool128;
  *target_pool = pool;
  return alloc_get(size);
}

/* debug */
void print_pool(alloc_pool_t *pool, char *title)
{
  printf("\n%s\n", title);
  printf("Pool base %p\n", pool);
  printf("  chunks=%d\n", pool->chunks);
  printf("  chunk_size=%d\n", pool->chunk_size);
  printf("  buffer=0x%p\n", pool->buffer);
  printf("  free head=%p\n", dlist_peek_head(&pool->free));
  printf("  free tail=%p\n", dlist_peek_tail(&pool->free));
  printf("  free size=%d\n", dlist_size(&pool->free));
  printf("  taken head=%p\n", dlist_peek_head(&pool->taken));
  printf("  taken tail=%p\n", dlist_peek_tail(&pool->taken));
  printf("  taken size=%d\n", dlist_size(&pool->taken));
}

void print_chunk(void *ptr, char *title)
{
  uint8_t *data = (uint8_t *)ptr;
  alloc_pool_chunk_t *chunk = (alloc_pool_chunk_t *)CONTAINER_OF(data, alloc_pool_chunk_t, data);
  printf("\n%s\n", title);
  printf("data base %p\n", data);
  printf("  chunk base %p\n", chunk);
  printf("  parent %p\n", chunk->parent);
}

UTEST(alloc, init)
{
  alloc_init();
  alloc_mempool_add(&pool32);
  alloc_mempool_add(&pool128);

  ASSERT_EQ(alloc_total_allocs(), (uint32_t)0);
  ASSERT_EQ(alloc_total_frees(), (uint32_t)0);
}

UTEST(alloc, one_small)
{
  alloc_init();
  alloc_mempool_add(&pool32);
  alloc_mempool_add(&pool128);

  void *p0 = alloc_get(20);
  ASSERT_EQ(p0, _get_raw_buffer_slot(&pool32, 0));
  ASSERT_EQ(dlist_size(&pool32.free), (uint32_t) 3);
  ASSERT_EQ(dlist_size(&pool32.taken), (uint32_t) 1);
  ASSERT_EQ(alloc_total_allocs(), (uint32_t)1);
  ASSERT_EQ(alloc_total_frees(), (uint32_t)0);

  alloc_free(p0);
  ASSERT_EQ(dlist_size(&pool32.free), (uint32_t) 4);
  ASSERT_EQ(dlist_size(&pool32.taken), (uint32_t) 0);
  ASSERT_EQ(alloc_total_allocs(), (uint32_t)1);
  ASSERT_EQ(alloc_total_frees(), (uint32_t)1);
}

UTEST(alloc, one_large)
{
  alloc_init();
  alloc_mempool_add(&pool32);
  alloc_mempool_add(&pool128);

  void *p0 = alloc_get(36);
  ASSERT_EQ(p0, _get_raw_buffer_slot(&pool128, 0));
  ASSERT_EQ(dlist_size(&pool128.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool128.taken), (uint32_t) 1);
  ASSERT_EQ(alloc_total_allocs(), (uint32_t)2);
  ASSERT_EQ(alloc_total_frees(), (uint32_t)1);

  alloc_free(p0);
  ASSERT_EQ(dlist_size(&pool128.free), (uint32_t) 2);
  ASSERT_EQ(dlist_size(&pool128.taken), (uint32_t) 0);
  ASSERT_EQ(alloc_total_allocs(), (uint32_t)2);
  ASSERT_EQ(alloc_total_frees(), (uint32_t)2);
}

UTEST(alloc, too_large)
{
  alloc_init();
  alloc_mempool_add(&pool32);
  alloc_mempool_add(&pool128);

  void *p0 = alloc_get(200);
  ASSERT_EQ(p0, (void *)0);
  ASSERT_EQ(alloc_total_allocs(), (uint32_t)2);
  ASSERT_EQ(alloc_total_frees(), (uint32_t)2);
}

UTEST(alloc, max_out)
{
  void *ptrs[6];
  int idx = 0;

  alloc_init();
  alloc_mempool_add(&pool32);
  alloc_mempool_add(&pool128);

  ptrs[idx] = alloc_get(20); ASSERT_NE(ptrs[idx++], (void *)0);
  ptrs[idx] = alloc_get(20); ASSERT_NE(ptrs[idx++], (void *)0);
  ptrs[idx] = alloc_get(60); ASSERT_NE(ptrs[idx++], (void *)0);
  ptrs[idx] = alloc_get(20); ASSERT_NE(ptrs[idx++], (void *)0);
  ptrs[idx] = alloc_get(60); ASSERT_NE(ptrs[idx++], (void *)0);
  ptrs[idx] = alloc_get(20); ASSERT_NE(ptrs[idx++], (void *)0);

  ASSERT_EQ(dlist_size(&pool32.free), (uint32_t) 0);
  ASSERT_EQ(dlist_size(&pool32.taken), (uint32_t) 4);
  ASSERT_EQ(dlist_size(&pool128.free), (uint32_t) 0);
  ASSERT_EQ(dlist_size(&pool128.taken), (uint32_t) 2);

  void *pnull = alloc_get(32);
  ASSERT_EQ(pnull, (void *)0);
  
  pnull = alloc_get(80);
  ASSERT_EQ(pnull, (void *)0);

  alloc_free(ptrs[3]);
  alloc_free(ptrs[0]);
  alloc_free(ptrs[1]);
  alloc_free(ptrs[5]);
  alloc_free(ptrs[4]);
  alloc_free(ptrs[2]);

  ASSERT_EQ(dlist_size(&pool32.free), (uint32_t) 4);
  ASSERT_EQ(dlist_size(&pool32.taken), (uint32_t) 0);
  ASSERT_EQ(dlist_size(&pool128.free), (uint32_t) 2);
  ASSERT_EQ(dlist_size(&pool128.taken), (uint32_t) 0);  
}

UTEST(alloc, long_run)
{
  srand(time(NULL));

  alloc_init();
  alloc_mempool_add(&pool32);
  alloc_mempool_add(&pool128);

  alloc_pool_t *pool;
  void *p0;
  void *p1;
  void *p2;
  
  for (int i=0; i<1000; i++) {
    p0 = random_alloc(&pool); if (p0) ASSERT_TRUE(_is_within_pool(pool, p0));
    p1 = random_alloc(&pool); if (p1) ASSERT_TRUE(_is_within_pool(pool, p1));
    p2 = random_alloc(&pool); if (p2) ASSERT_TRUE(_is_within_pool(pool, p2));

    if (p1) alloc_free(p1);
    if (p0) alloc_free(p0);
    if (p2) alloc_free(p2);
  }
}