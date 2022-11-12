#include <stdio.h>
#include <string.h>
#include "slist.h"
#include "utest.h"
#include <stdio.h>

// Uncomment for debug purposes
// static void list_print(slist_t *list) {
//   printf("head: %p\n", list->head);
//   printf("tail: %p\n", list->tail);
//   printf("size: %d\n", list->size);

//   node_t *node = list->head;
//   while(node) {
//     printf("  node: %p, next: %p\n", node, node->next);
//     node = node->next;
//   }
// }


UTEST(slist, init)
{
  slist_t list;

  slist_init(&list);

  ASSERT_EQ(slist_size(&list), (uint32_t)0);
  ASSERT_EQ(list.head, NULL);
  ASSERT_EQ(list.tail, NULL);
  ASSERT_EQ(slist_empty(&list), 1);
}

UTEST(slist, prepend)
{
  slist_t list;
  node_t node1;
  node_t node2;

  slist_init(&list);

  slist_prepend(&list, &node1);
  ASSERT_EQ(slist_size(&list), (uint32_t)1);
  ASSERT_EQ(list.head, &node1);
  ASSERT_EQ(list.tail, &node1);

  slist_prepend(&list, &node2);
  ASSERT_EQ(list.size, (uint32_t)2);
  ASSERT_EQ(list.head, &node2);
  ASSERT_EQ(list.tail, &node1);
}

UTEST(slist, append)
{
  slist_t list;
  node_t node1;
  node_t node2;

  slist_init(&list);

  slist_append(&list, &node1);
  ASSERT_EQ(slist_size(&list), (uint32_t)1);
  ASSERT_EQ(list.head, &node1);
  ASSERT_EQ(list.tail, &node1);

  slist_append(&list, &node2);
  ASSERT_EQ(slist_size(&list), (uint32_t)2);
  ASSERT_EQ(list.head, &node1);
  ASSERT_EQ(list.tail, &node2);
}

UTEST(slist, peek)
{
  slist_t list;
  node_t node1;
  node_t node2;

  slist_init(&list);
  ASSERT_EQ(slist_peek_head(&list), NULL);
  ASSERT_EQ(slist_peek_tail(&list), NULL);

  slist_append(&list, &node1);
  ASSERT_EQ(slist_peek_head(&list), &node1);
  ASSERT_EQ(slist_peek_tail(&list), &node1);

  slist_append(&list, &node2);
  ASSERT_EQ(slist_peek_head(&list), &node1);
  ASSERT_EQ(slist_peek_tail(&list), &node2);
}

UTEST(slist, get)
{
  slist_t list;
  node_t node1;
  node_t node2;
  node_t node3;

  slist_init(&list);
  ASSERT_EQ(slist_get(&list), NULL);

  slist_append(&list, &node1);
  slist_append(&list, &node2);
  slist_append(&list, &node3);
  ASSERT_EQ(slist_size(&list), (uint32_t)3);

  ASSERT_EQ(slist_get(&list), &node1);
  ASSERT_EQ(slist_size(&list), (uint32_t)2);
  ASSERT_EQ(slist_peek_head(&list), &node2);
  ASSERT_EQ(slist_peek_tail(&list), &node3);
  ASSERT_EQ(slist_get(&list), &node2);
  ASSERT_EQ(slist_size(&list), (uint32_t)1);
  ASSERT_EQ(slist_peek_head(&list), &node3);
  ASSERT_EQ(slist_peek_tail(&list), &node3);
  ASSERT_EQ(slist_get(&list), &node3);
  ASSERT_EQ(slist_size(&list), (uint32_t)0);
  ASSERT_EQ(slist_get(&list), NULL);

  slist_init(&list);
  slist_append(&list, &node1);
  ASSERT_EQ(slist_get(&list), &node1);
  ASSERT_EQ(slist_size(&list), (uint32_t)0);
  ASSERT_EQ(slist_get(&list), NULL);
  ASSERT_EQ(slist_peek_head(&list), NULL);
  ASSERT_EQ(slist_peek_tail(&list), NULL);
}

UTEST(slist, remove)
{
  slist_t list;
  node_t node1;
  node_t node2;
  node_t node3;
  node_t node4;

  slist_init(&list);
  slist_append(&list, &node1);
  slist_append(&list, &node2);
  slist_append(&list, &node3);
  slist_append(&list, &node4);

  slist_remove(&list, &node1, &node2);
  ASSERT_EQ(slist_size(&list), (uint32_t)3);
  ASSERT_EQ(slist_peek_head(&list), &node1);
  ASSERT_EQ(slist_peek_tail(&list), &node4);

  slist_remove(&list, NULL, &node1);
  ASSERT_EQ(slist_size(&list), (uint32_t)2);
  ASSERT_EQ(slist_peek_head(&list), &node3);
  ASSERT_EQ(slist_peek_tail(&list), &node4);

  slist_remove(&list, &node3, &node4);
  ASSERT_EQ(slist_size(&list), (uint32_t)1);
  ASSERT_EQ(slist_peek_head(&list), &node3);
  ASSERT_EQ(slist_peek_tail(&list), &node3);

  slist_remove(&list, NULL, &node3);
  ASSERT_EQ(slist_size(&list), (uint32_t)0);
  ASSERT_EQ(slist_peek_head(&list), NULL);
  ASSERT_EQ(slist_peek_tail(&list), NULL);
}

UTEST(slist, find_and_remove)
{
  slist_t list;
  node_t node1;
  node_t node2;
  node_t node3;
  node_t node4;

  slist_init(&list);
  slist_append(&list, &node1);
  slist_append(&list, &node2);
  slist_append(&list, &node3);
  slist_append(&list, &node4);

  ASSERT_EQ(slist_find_and_remove(&list, &node2), 1);
  ASSERT_EQ(slist_size(&list), (uint32_t)3);
  ASSERT_EQ(slist_peek_head(&list), &node1);
  ASSERT_EQ(slist_peek_tail(&list), &node4);

  ASSERT_EQ(slist_find_and_remove(&list, &node2), 0);
  ASSERT_EQ(slist_size(&list), (uint32_t)3);
  ASSERT_EQ(slist_peek_head(&list), &node1);
  ASSERT_EQ(slist_peek_tail(&list), &node4);

  ASSERT_EQ(slist_find_and_remove(&list, &node4), 1);
  ASSERT_EQ(slist_size(&list), (uint32_t)2);
  ASSERT_EQ(slist_peek_head(&list), &node1);
  ASSERT_EQ(slist_peek_tail(&list), &node3);

  ASSERT_EQ(slist_find_and_remove(&list, &node1), 1);
  ASSERT_EQ(slist_size(&list), (uint32_t)1);
  ASSERT_EQ(slist_peek_head(&list), &node3);
  ASSERT_EQ(slist_peek_tail(&list), &node3);
}

UTEST(slist, for_each)
{
  slist_t list;
  node_t node1;
  node_t node2;
  node_t node3;
  int cnt = 0;

  node_t *it;

  slist_init(&list);
  SLIST_FOR_EACH(&list, it) {
    cnt++;
  }
  ASSERT_EQ(cnt, 0);

  slist_append(&list, &node1);
  slist_append(&list, &node2);
  slist_append(&list, &node3);


  SLIST_FOR_EACH(&list, it) {
    cnt++;
  }
  ASSERT_EQ(cnt, 3);
}

UTEST(slist, two_lists)
{
  slist_t list1;
  slist_t list2;
  node_t node1;
  node_t node2;
  node_t node3;

  slist_init(&list1);
  slist_init(&list2);

  slist_append(&list1, &node1);
  slist_append(&list1, &node2);
  slist_append(&list1, &node3);
  
  node_t *tmp = slist_get(&list1);
  slist_prepend(&list2, tmp);
  ASSERT_EQ(tmp->next, NULL);
  ASSERT_EQ(slist_peek_head(&list1), &node2);
  ASSERT_EQ(slist_peek_tail(&list1), &node3);
  ASSERT_EQ(slist_peek_head(&list2), tmp);
  ASSERT_EQ(slist_peek_tail(&list2), tmp);

  slist_remove(&list1, &node2, &node3);
  slist_append(&list2, &node3);
  ASSERT_EQ(slist_peek_head(&list1), &node2);
  ASSERT_EQ(slist_peek_tail(&list1), &node2);
  ASSERT_EQ(slist_peek_head(&list2), &node1);
  ASSERT_EQ(slist_peek_tail(&list2), &node3);

  int cnt = 0;
  SLIST_FOR_EACH(&list1, tmp) {
    cnt++;
  }
  ASSERT_EQ(cnt, 1);
}
