#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "alloc2.h"
#include "utest.h"

DEFINE_ALLOC2_MEMPOOL(pool1k, 1024 + 24);
DEFINE_ALLOC2_MEMPOOL(pool4k, 4096 + 24);

/* helpers */
#define CHUNK_OVERHEAD offsetof(alloc2_pool_chunk_t, data)

struct allocs_t {
  uint32_t size;
  void *ptr;
};

static inline uint32_t random_alloc_size(uint32_t min, uint32_t max)
{
  uint32_t range = max - min;
  uint32_t size = (rand() % range) + min;
  return size & 0xFFFFFFFC; // align to 4 bytes
}

static int check_overlap(struct allocs_t *tbl, int len)
{
  for (int i=0; i<len; i++) {
    uint8_t *lhs_start = (uint8_t *)tbl[i].ptr - CHUNK_OVERHEAD;
    uint8_t *lhs_end = (uint8_t *)tbl[i].ptr + tbl[i].size;

    for (int j=1; j<len; j++) {
      if (i == j)
        continue;

      uint8_t *rhs_start = (uint8_t *)tbl[j].ptr - CHUNK_OVERHEAD;
      uint8_t *rhs_end = (uint8_t *)tbl[j].ptr + tbl[j].size;

      if (lhs_start < rhs_end && lhs_end > rhs_start)
        return 1;
    }
  }

  return 0;
}

// /* debug */
// static void print_chunk(alloc2_pool_chunk_t *chk, char *msg)
// {
//   printf("%s base=%p data=%p(+%d) size=%-4d end=%p\n",
//       msg,
//       chk,
//       chk->data,
//       (int)((uint8_t *)chk->data - (uint8_t *)chk),
//       chk->size,
//       chk->data + chk->size);
// }

// static void print_pool(alloc2_pool_t *pool, char *msg)
// {
//   dnode_t *node;

//   printf("pool dump %s\nFREE:\n", msg);
//   DLIST_FOR_EACH(&pool->free, node) {
//     print_chunk((alloc2_pool_chunk_t *)node, "");
//   }

//   printf("TAKEN:\n");
//   DLIST_FOR_EACH(&pool->taken, node) {
//     print_chunk((alloc2_pool_chunk_t *)node, "");
//   }

//   printf("\n");
// }

UTEST(alloc2, init)
{
  alloc2_init(&pool1k);

  ASSERT_EQ(alloc2_total_allocs(), (uint32_t)0);
  ASSERT_EQ(alloc2_total_frees(), (uint32_t)0);
}

UTEST(alloc2, one_small)
{
  alloc2_init(&pool1k);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);

  void *p0 = alloc2_get(20);
  ASSERT_NE(p0, NULL);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 1);

  alloc2_free(p0);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 0);
}

UTEST(alloc2, one_large)
{
  alloc2_init(&pool1k);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);

  void *p0 = alloc2_get(512);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 1);

  alloc2_free(p0);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 0);

  ASSERT_EQ(alloc2_total_allocs(), (uint32_t)1);
  ASSERT_EQ(alloc2_total_frees(), (uint32_t)1);
}

UTEST(alloc2, too_large)
{
  alloc2_init(&pool1k);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);

  void *p0 = alloc2_get(1030);
  ASSERT_EQ(p0, NULL);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 0);
  ASSERT_EQ(alloc2_total_allocs(), (uint32_t)0);
  ASSERT_EQ(alloc2_total_frees(), (uint32_t)0);
}

UTEST(alloc2, min_allocation)
{
  alloc2_init(&pool1k);

  uint8_t *p0 = (uint8_t *)alloc2_get(20);
  uint8_t *p1 = (uint8_t *)alloc2_get(20);
  // min allocated space is 32 bytes
  ASSERT_EQ(p0 - p1, (unsigned)(32 + CHUNK_OVERHEAD));
}

UTEST(alloc2, max_out)
{
  void *ptrs[6];
  int idx = 0;

  alloc2_init(&pool1k);

  // there's 20 bytes of overhead per chunk
  ptrs[idx] = alloc2_get(128); ASSERT_NE(ptrs[idx++], NULL);
  ptrs[idx] = alloc2_get( 64); ASSERT_NE(ptrs[idx++], NULL);
  ptrs[idx] = alloc2_get(512); ASSERT_NE(ptrs[idx++], NULL);
  ptrs[idx] = alloc2_get( 32); ASSERT_NE(ptrs[idx++], NULL);
  ptrs[idx] = alloc2_get(128); ASSERT_NE(ptrs[idx++], NULL);
  ptrs[idx] = alloc2_get( 32); ASSERT_NE(ptrs[idx++], NULL);

  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 0);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 6);

  void *pnull = alloc2_get(32);
  ASSERT_EQ(pnull, NULL);

  alloc2_free(ptrs[3]);
  alloc2_free(ptrs[0]);
  alloc2_free(ptrs[1]);
  alloc2_free(ptrs[5]);
  alloc2_free(ptrs[4]);
  alloc2_free(ptrs[2]);

  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 0);
}

UTEST(alloc2, free_in_the_middle)
{
  alloc2_init(&pool1k);

  void *p0 = alloc2_get(128);
  void *p1 = alloc2_get(128);
  void *p2 = alloc2_get(32);
  void *p3 = alloc2_get(128);
  void *p4 = alloc2_get(128);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 5);

  alloc2_free(p0);
  alloc2_free(p3);
  alloc2_free(p4);
  alloc2_free(p1);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 2);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 1);

  alloc2_free(p2);
}

UTEST(alloc2, alloc_get_the_same_chunk)
{
  /*
   * This one is tricky because we need to make sure that there are 2 cunks in free list
   * and that the first one will be too small.
   */
  alloc2_init(&pool1k);

  void *p0 = alloc2_get(256);
  void *p1 = alloc2_get(256);
  void *p3 = alloc2_get(256);
  void *p4 = alloc2_get(64);
  alloc2_free(p1);

  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 2);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 3);

  void *p5 = alloc2_get(256);
  ASSERT_EQ(p1, p5);
  ASSERT_EQ(dlist_size(&pool1k.free), (uint32_t) 1);
  ASSERT_EQ(dlist_size(&pool1k.taken), (uint32_t) 4);

  alloc2_free(p5);
  alloc2_free(p0);
  alloc2_free(p3);
  alloc2_free(p4);
}

UTEST(alloc2, long_run_no_oom)
{
  srand(time(NULL));

  alloc2_init(&pool4k);

  #define MAX_PTRS 8
  struct allocs_t tbl[MAX_PTRS];

  for (int i=0; i<100000; i++) {
    for (int j=0; j<MAX_PTRS; j++) {
      uint32_t size = random_alloc_size(32, 480);
      tbl[j].size = size;
      tbl[j].ptr = alloc2_get(size);
      // if (!tbl[j].ptr) {
      //   printf("OOM at %d\n", i);
      //   print_pool(&pool4k, "pool4k");
      // }
      ASSERT_NE(tbl[j].ptr, NULL);
    }

    ASSERT_EQ(check_overlap(tbl, MAX_PTRS), 0);

    for (int j=0; j<MAX_PTRS; j++) {
      tbl[j].size = 0;
      alloc2_free(tbl[j].ptr);
    }

    ASSERT_LT(dlist_size(&pool1k.free), (uint32_t) 2);
  }
}

UTEST(alloc2, alloc_aligned_to_8)
{
  alloc2_init(&pool1k);

  #define NO_ALLOCS 10
  int sizes[NO_ALLOCS] = { 20, 1, 7, 67, 211, 128, 32, 16, 16, 29 };
  void *ptrs[NO_ALLOCS];

  for(int i = 0; i < NO_ALLOCS; i++) {
    void *p = alloc2_get(sizes[i]);
    int aligned = (unsigned long)p % 8;
    ASSERT_EQ(aligned, 0);
    ptrs[i] = p;
  }

  for(int i = 0; i < NO_ALLOCS; i++) {
    alloc2_free(ptrs[i]);
  }
}