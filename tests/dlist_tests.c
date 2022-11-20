#include <stdio.h>
#include <string.h>
#include "dlist.h"
#include "utest.h"
#include <stdio.h>

// Uncomment for debug purposes
// static void list_print(dlist_t *list) {
//   printf("head: %p\n", list->head);
//   printf("tail: %p\n", list->tail);
//   printf("size: %d\n", list->size);

//   ddnode_t *node = list->head;
//   while(node) {
//     printf("  node: %p, next: %p\n", node, node->next);
//     node = node->next;
//   }
// }


UTEST(dlist, init)
{
  dlist_t list;

  dlist_init(&list);

  ASSERT_EQ(dlist_size(&list), (uint32_t)0);
  ASSERT_EQ(list.head, NULL);
  ASSERT_EQ(list.tail, NULL);
  ASSERT_EQ(dlist_empty(&list), 1);
}

UTEST(dlist, prepend)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;

  dlist_init(&list);

  dlist_prepend(&list, &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)1);
  ASSERT_EQ(list.head, &node1);
  ASSERT_EQ(list.tail, &node1);
  ASSERT_EQ(node1.next, NULL);
  ASSERT_EQ(node1.prev, NULL);

  dlist_prepend(&list, &node2);
  ASSERT_EQ(list.size, (uint32_t)2);
  ASSERT_EQ(list.head, &node2);
  ASSERT_EQ(list.tail, &node1);
  ASSERT_EQ(node1.next, NULL);
  ASSERT_EQ(node1.prev, &node2);
  ASSERT_EQ(node2.next, &node1);
  ASSERT_EQ(node2.prev, NULL);
}

UTEST(dlist, append)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;

  dlist_init(&list);

  dlist_append(&list, &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)1);
  ASSERT_EQ(list.head, &node1);
  ASSERT_EQ(list.tail, &node1);
  ASSERT_EQ(node1.next, NULL);
  ASSERT_EQ(node1.prev, NULL);

  dlist_append(&list, &node2);
  ASSERT_EQ(dlist_size(&list), (uint32_t)2);
  ASSERT_EQ(list.head, &node1);
  ASSERT_EQ(list.tail, &node2);
  ASSERT_EQ(node1.next, &node2);
  ASSERT_EQ(node1.prev, NULL);
  ASSERT_EQ(node2.next, NULL);
  ASSERT_EQ(node2.prev, &node1);
}

UTEST(dlist, peek)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;

  dlist_init(&list);
  ASSERT_EQ(dlist_peek_head(&list), NULL);
  ASSERT_EQ(dlist_peek_tail(&list), NULL);

  dlist_append(&list, &node1);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node1);

  dlist_append(&list, &node2);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node2);
}

UTEST(dlist, get_head)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;
  dnode_t node3;

  dlist_init(&list);
  ASSERT_EQ(dlist_get_head(&list), NULL);

  dlist_append(&list, &node1);
  dlist_append(&list, &node2);
  dlist_append(&list, &node3);
  ASSERT_EQ(dlist_size(&list), (uint32_t)3);

  ASSERT_EQ(dlist_get_head(&list), &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)2);
  ASSERT_EQ(dlist_peek_head(&list), &node2);
  ASSERT_EQ(dlist_peek_tail(&list), &node3);
  ASSERT_EQ(node2.prev, NULL);
  ASSERT_EQ(node3.next, NULL);
  ASSERT_EQ(dlist_get_head(&list), &node2);
  ASSERT_EQ(dlist_size(&list), (uint32_t)1);
  ASSERT_EQ(dlist_peek_head(&list), &node3);
  ASSERT_EQ(dlist_peek_tail(&list), &node3);
  ASSERT_EQ(node3.prev, NULL);
  ASSERT_EQ(node3.next, NULL);
  ASSERT_EQ(dlist_get_head(&list), &node3);
  ASSERT_EQ(dlist_size(&list), (uint32_t)0);
  ASSERT_EQ(dlist_get_head(&list), NULL);

  dlist_init(&list);
  dlist_append(&list, &node1);
  ASSERT_EQ(dlist_get_head(&list), &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)0);
  ASSERT_EQ(dlist_get_head(&list), NULL);
  ASSERT_EQ(dlist_peek_head(&list), NULL);
  ASSERT_EQ(dlist_peek_tail(&list), NULL);
}

UTEST(dlist, get_tail)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;
  dnode_t node3;

  dlist_init(&list);
  ASSERT_EQ(dlist_get_tail(&list), NULL);

  dlist_append(&list, &node1);
  dlist_append(&list, &node2);
  dlist_append(&list, &node3);
  ASSERT_EQ(dlist_size(&list), (uint32_t)3);

  ASSERT_EQ(dlist_get_tail(&list), &node3);
  ASSERT_EQ(dlist_size(&list), (uint32_t)2);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node2);
  ASSERT_EQ(node1.prev, NULL);
  ASSERT_EQ(node2.next, NULL);
  ASSERT_EQ(dlist_get_tail(&list), &node2);
  ASSERT_EQ(dlist_size(&list), (uint32_t)1);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node1);
  ASSERT_EQ(node1.prev, NULL);
  ASSERT_EQ(node1.next, NULL);
  ASSERT_EQ(dlist_get_tail(&list), &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)0);
  ASSERT_EQ(dlist_get_tail(&list), NULL);

  dlist_init(&list);
  dlist_append(&list, &node1);
  ASSERT_EQ(dlist_get_tail(&list), &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)0);
  ASSERT_EQ(dlist_get_tail(&list), NULL);
  ASSERT_EQ(dlist_peek_head(&list), NULL);
  ASSERT_EQ(dlist_peek_tail(&list), NULL);
}

UTEST(dlist, remove)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;
  dnode_t node3;
  dnode_t node4;

  dlist_init(&list);
  dlist_append(&list, &node1);
  dlist_append(&list, &node2);
  dlist_append(&list, &node3);
  dlist_append(&list, &node4);

  dlist_remove(&list, &node2);
  ASSERT_EQ(dlist_size(&list), (uint32_t)3);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node4);

  dlist_remove(&list, &node1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)2);
  ASSERT_EQ(dlist_peek_head(&list), &node3);
  ASSERT_EQ(dlist_peek_tail(&list), &node4);
  ASSERT_EQ(node3.prev, NULL);
  ASSERT_EQ(node4.next, NULL);

  dlist_remove(&list, &node4);
  ASSERT_EQ(dlist_size(&list), (uint32_t)1);
  ASSERT_EQ(dlist_peek_head(&list), &node3);
  ASSERT_EQ(dlist_peek_tail(&list), &node3);
  ASSERT_EQ(node3.prev, NULL);
  ASSERT_EQ(node3.next, NULL);

  dlist_remove(&list, &node3);
  ASSERT_EQ(dlist_size(&list), (uint32_t)0);
  ASSERT_EQ(dlist_peek_head(&list), NULL);
  ASSERT_EQ(dlist_peek_tail(&list), NULL);
}

UTEST(dlist, find_and_remove)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;
  dnode_t node3;
  dnode_t node4;

  dlist_init(&list);
  dlist_append(&list, &node1);
  dlist_append(&list, &node2);
  dlist_append(&list, &node3);
  dlist_append(&list, &node4);

  ASSERT_EQ(dlist_find_and_remove(&list, &node2), 1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)3);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node4);

  ASSERT_EQ(dlist_find_and_remove(&list, &node2), 0);
  ASSERT_EQ(dlist_size(&list), (uint32_t)3);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node4);

  ASSERT_EQ(dlist_find_and_remove(&list, &node4), 1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)2);
  ASSERT_EQ(dlist_peek_head(&list), &node1);
  ASSERT_EQ(dlist_peek_tail(&list), &node3);

  ASSERT_EQ(dlist_find_and_remove(&list, &node1), 1);
  ASSERT_EQ(dlist_size(&list), (uint32_t)1);
  ASSERT_EQ(dlist_peek_head(&list), &node3);
  ASSERT_EQ(dlist_peek_tail(&list), &node3);
}

UTEST(dlist, for_each)
{
  dlist_t list;
  dnode_t node1;
  dnode_t node2;
  dnode_t node3;
  int cnt = 0;

  dnode_t *it;

  dlist_init(&list);
  DLIST_FOR_EACH(&list, it) {
    cnt++;
  }
  ASSERT_EQ(cnt, 0);

  dlist_append(&list, &node1);
  dlist_append(&list, &node2);
  dlist_append(&list, &node3);


  DLIST_FOR_EACH(&list, it) {
    cnt++;
  }
  ASSERT_EQ(cnt, 3);
}

UTEST(dlist, two_lists)
{
  dlist_t list1;
  dlist_t list2;
  dnode_t node1;
  dnode_t node2;
  dnode_t node3;

  dlist_init(&list1);
  dlist_init(&list2);

  dlist_append(&list1, &node1);
  dlist_append(&list1, &node2);
  dlist_append(&list1, &node3);
  
  dnode_t *tmp = dlist_get_head(&list1);
  dlist_prepend(&list2, tmp);
  ASSERT_EQ(tmp->next, NULL);
  ASSERT_EQ(dlist_peek_head(&list1), &node2);
  ASSERT_EQ(dlist_peek_tail(&list1), &node3);
  ASSERT_EQ(dlist_peek_head(&list2), tmp);
  ASSERT_EQ(dlist_peek_tail(&list2), tmp);

  dlist_remove(&list1, &node3);
  dlist_append(&list2, &node3);
  ASSERT_EQ(dlist_peek_head(&list1), &node2);
  ASSERT_EQ(dlist_peek_tail(&list1), &node2);
  ASSERT_EQ(dlist_peek_head(&list2), &node1);
  ASSERT_EQ(dlist_peek_tail(&list2), &node3);

  int cnt = 0;
  DLIST_FOR_EACH(&list1, tmp) {
    cnt++;
  }
  ASSERT_EQ(cnt, 1);
}