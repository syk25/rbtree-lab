#include "rbtree.h"

#include <stdlib.h>
#include <assert.h>
// #include <rbtree.h>
#include <stdbool.h>
#include <stdio.h>
// #include <stdlib.h>

////////////////////////--------프로토타입추가------------//////////////////////////////
void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *y);
void insert_fixup(rbtree *t, node_t *z);
void rb_transplant(rbtree *t, node_t *u, node_t *v);
node_t *rbtree_insert(rbtree *t, const key_t key);
rbtree *new_rbtree(void);
void delete_fixup(rbtree *t, node_t *x);
void post_order_traversal_for_delete(node_t *n, node_t *nil);
void delete_rbtree(rbtree *t);
node_t *rbtree_min(const rbtree *t);
node_t *rbtree_max(const rbtree *t);
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n);

void inorder_traversal_to_array(const node_t *n, key_t *arr, int *index, node_t *nil);

////////////////////////////////////////////////////////////////////////////////////
node_t *rb_successor(rbtree *t, node_t *x)
{
  node_t *y = x;
  while (y->left != t->nil)
  {
    y = y->left;
  }
  return y;
}
////////////////////////////////////////////////////////////////////////////////////

// int rbtree_erase(rbtree *t, node_t *z) {

//   node_t *x;                              // 노드 x
//   node_t *y = z;                          // y는 삭제할 노드
//   color_t y_color = y->color;             // y_color는 y의 색
//   if (z->left == t->nil) {                // 삭제할 노드의 왼쪽이 nil일때
//     x = z->right;                         // x는 삭제할 노드의 오른쪽
//     rb_transplant(t, z, z->right);    // 삭제할 노드의 부모와 삭제할 노드의 오른쪽을 연결
//   } else if (z->right == t->nil) {        // 삭제할 노드의 오른쪽이 nill일때
//     x = z->left;                          // x는 삭제할 노드의 왼쪽
//     rb_transplant(t, z, z->left);     // 삭제할 노드의 부모와 삭제할 노드의 왼쪽을 연결
//   } else {                                // 삭제할 노드의 왼쪽, 오른쪽 자식이 둘다 있을 때
//     y = rb_successor(t, z->right);    // y는 직후 원소
//     y_color = y->color;                   // y_color는 직후 원소의 색
//     x = y->right;                         // x는 y의 오른쪽 자식
//     if (y->parent == z) {                 // y의 부모가 삭제할 노드일 때
//       x->parent = y;                      // x의 부모는 y
//     } else {                              // y의 부모가 삭제할 노드가 아닐 때
//       rb_transplant(t, y, y->right);  // y의 부모와 y의 오른쪽 자식을 연결
//       y->right = z->right;                // y의 오른쪽 자식은 삭제할 노드의 오른쪽 자식
//       y->right->parent = y;               // y의 오른쪽 자식의 부모는 y
//     }
//     rb_transplant(t, z, y);           // 삭제할 노드 부모와 y를 연결
//     y->left = z->left;                    // y의 왼쪽 자식은 삭제할 노드의 왼쪽 자식
//     y->left->parent = y;                  // y의 왼쪽 자식의 부모는 y
//     y->color = z->color;                  // y의 색은 삭제할 노드의 색
//   }
// }
////////////////////////////////////////////////////////////////////////////////////

// u를 루트로하는 서브트리를 v를 루트로하는 서브트리로 바꾸는 함수
void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

////////////////////////////////////////////////////////////////////////////////////
// 좌회전함수(우노드 -> 부모, 부모 -> 좌 노드)
void left_rotate(rbtree *t, node_t *x)
{
  node_t *y;

  y = x->right; // 올릴 노드 지정

  // y 노드의 왼쪽과 x의 관계 설정
  x->right = y->left;
  if (y->left != t->nil)
  {
    // y->left->parent = x->right; //TODO
    y->left->parent = x; //TODO
  }

  // x의 부모와 y의 부모 교환
  y->parent = x->parent;
  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->left)
  {
    x->parent->left = y;
  }
  else
  {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}
////////////////////////////////////////////////////////////////////////////////////
// 우회전함수(좌노드 -> 부모, 부모 -> 우 노드)
void right_rotate(rbtree *t, node_t *y)
{
  node_t *x;

  x = y->left; // 올릴 노드 지정

  // y노드의 오른쪽과 x의 관계 설정
  y->left = x->right;
  if (x->right != t->nil)
  {
    // x->right->parent = y->left; //TODO
    x->right->parent = y; //TODO
  }

  // x 부모와 y 부모의 관계 설정
  x->parent = y->parent;
  if (y->parent == t->nil)
  {
    t->root = x;
  }
  else if (y == y->parent->left)
  {
    y->parent->left = x;
  }
  else
  {
    y->parent->right = x;
  }

  // x와 y의 관계 반전(x가 부모되고 y오른쪽 자식이 됨)
  x->right = y;
  y->parent = x;
}
////////////////////////////////////////////////////////////////////////////////////
// 삽입시 속성 위반 조정함수(z는 추가되는 노드)
void insert_fixup(rbtree *t, node_t *z)
{
  // implement fix-up function
  node_t *y; // 삼촌 노드 추적
  while (z->parent->color == RBTREE_RED)
  {
    // 부모가 할아버지의 좌측 노드인 경우
    if (z->parent == z->parent->parent->left)
    {
      y = z->parent->parent->right;
      // case1
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->right) // case2
        {
          z = z->parent;
          left_rotate(t, z);
        }
        // case3
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else // 부모가 할아버지의 우측 노드인 경우
    {
      y = z->parent->parent->left; // 삼촌 노드 설정
      // case1
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->left)
        { // case2
          z = z->parent;
          right_rotate(t, z);
        }
        // case3
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }

  t->root->color = RBTREE_BLACK; // 속성2 위반 보충

  return;
}
////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_insert(rbtree *t, const key_t key)
{

  // 초기 설정
  node_t *x, *y;
  node_t *z = calloc(1, sizeof(node_t));
  z->key = key;
  y = t->nil;
  x = t->root;

  // 리프노드로 이동
  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }

  // 삽입노드의 위치 설정
  z->parent = y;
  if (y == t->nil)
  {
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }

  // 삽입된 노드의 초기화
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;
  insert_fixup(t, z);
  return z; // 삽입된 노드의 주소 반환
}
////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  // 트리가 없는 경우
  if (t == NULL)
  {
    return NULL;
  }

  // 트리는 있지만 루트가 없는 경우
  if (t->root == t->nil)
  {
    return NULL;
  }

  // 트리도 있고 루트도 존재하는 경우
  node_t *cur = t->root;
  while (!(cur->left == t->nil && cur->right == t->nil)) // 자식의 수가 둘다 0이 아닌 경우
  {
    if (cur->key == key) // 키값이 현재 보는 노드의 키와 같은 경우
    {
      return cur;
    }
    else if (cur->key > key) // 키값이 큰 경우 트리의 오른쪽으로 이동
    {
      cur = cur->left;
    }
    else if (cur->key < key) // 키값이 작은 경우 트리의 왼쪽으로 이동
    {
      cur = cur->right;
    }
  }

  // 값이 존재하는 리프에서 값이 존재하는 지 확인
  if (cur->key == key)
  {
    return cur;
  }
  else
  {
    return NULL;
  }
}
////////////////////////////////////////////////////////////////////////////////////
rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t *)calloc(1, sizeof(node_t));

  p->nil->color = RBTREE_BLACK;
  p->nil->parent = NULL;
  p->nil->left = NULL;
  p->nil->right = NULL;

  p->root = p->nil;

  return p;
}
////////////////////////////////////////////////////////////////////////////////////
void post_order_traversal_for_delete(node_t *n, node_t *nil)
{
  if (n != nil)
  {
    post_order_traversal_for_delete(n->left, nil);
    post_order_traversal_for_delete(n->right, nil);
    free(n);
    n = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////
void delete_rbtree(rbtree *t)
{
  post_order_traversal_for_delete(t->root, t->nil);
  free(t->nil);
  free(t);
}

////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_min(const rbtree *t)
{
  node_t *cur;
  cur = t->root;

  while (cur->left != t->nil)
  {
    cur = cur->left;
  }

  return cur;
}
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
node_t *rbtree_max(const rbtree *t)
{
  node_t *cur;
  cur = t->root;
  while (cur->right != t->nil)
  {
    cur = cur->right;
  }

  return cur;
}
////////////////////////////////////////////////////////////////////////////////////
int rbtree_erase(rbtree *t, node_t *z)
{
  node_t *y, *x;
  // rbtree *sub = malloc(sizeof(rbtree));
  y = z;

  color_t original = y->color;

  if (z->left == t->nil)
  {
    x = z->right;
    rb_transplant(t, z, z->right);
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    rb_transplant(t, z, z->left);
  }
  else
  {
    // 오른쪽 서브트리를 트리로 잠깐 바꾸기
    y = rb_successor(t, z->right);
    // sub->root = t->root->right;
    // sub->nil = t->nil;
    // y = rbtree_min(sub); // 후임자를 찾는 부분: 우측 브랜치의 최소값 찾기

    original = y->color;
    x = y->right;
    if (y->parent == z)
    {
      x->parent = y;
    }
    else
    {
      rb_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rb_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (original == RBTREE_BLACK)
  {
    delete_fixup(t, x);
  }
  free(z);
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////

void delete_fixup(rbtree *t, node_t *x)
{
  node_t *w;
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      w = x->parent->right;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      w = x->parent->left;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}
////////////////////////////////////////////////////////////////////////////////////
void inorder_traversal_to_array(const node_t *n, key_t *arr, int *index, node_t *nil)
{
  if (n != nil)
  {
    inorder_traversal_to_array(n->left, arr, index, nil);
    arr[(*index)++] = n->key;
    inorder_traversal_to_array(n->right, arr, index, nil);
  }
}

////////////////////////////////////////////////////////////////////////////////////
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // t: 트리 포인터, arr: 배열 포인터, n: 배열의 크기이자 트리의 원소
  int *index = malloc(sizeof(int));
  *index = 0;
  inorder_traversal_to_array(t->root, arr, index, t->nil);
  free(index);
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////




// //TODO: 시작부분

// // new_rbtree should return rbtree struct with null root node
// void test_init(void) {
//   rbtree *t = new_rbtree();
//   assert(t != NULL);
// #ifdef SENTINEL
//   assert(t->nil != NULL);
//   assert(t->root == t->nil);
// #else
//   assert(t->root == NULL);
// #endif
//   delete_rbtree(t);
// }

// // root node should have proper values and pointers
// void test_insert_single(const key_t key) {
//   rbtree *t = new_rbtree();
//   node_t *p = rbtree_insert(t, key);
//   assert(p != NULL);
//   assert(t->root == p);
//   assert(p->key == key);
//   // assert(p->color == RBTREE_BLACK);  // color of root node should be black
// #ifdef SENTINEL
//   assert(p->left == t->nil);
//   assert(p->right == t->nil);
//   assert(p->parent == t->nil);
// #else
//   assert(p->left == NULL);
//   assert(p->right == NULL);
//   assert(p->parent == NULL);
// #endif
//   delete_rbtree(t);
// }

// // find should return the node with the key or NULL if no such node exists
// void test_find_single(const key_t key, const key_t wrong_key) {
//   rbtree *t = new_rbtree();
//   node_t *p = rbtree_insert(t, key);

//   node_t *q = rbtree_find(t, key);
//   assert(q != NULL);
//   assert(q->key == key);
//   assert(q == p);

//   q = rbtree_find(t, wrong_key);
//   assert(q == NULL);

//   delete_rbtree(t);
// }

// // erase should delete root node
// void test_erase_root(const key_t key) {
//   rbtree *t = new_rbtree();
//   node_t *p = rbtree_insert(t, key);
//   assert(p != NULL);
//   assert(t->root == p);
//   assert(p->key == key);

//   rbtree_erase(t, p);
// #ifdef SENTINEL
//   assert(t->root == t->nil);
// #else
//   assert(t->root == NULL);
// #endif

//   delete_rbtree(t);
// }

// static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
//   for (size_t i = 0; i < n; i++) {
//     rbtree_insert(t, arr[i]);
//   }
// }

// static int comp(const void *p1, const void *p2) {
//   const key_t *e1 = (const key_t *)p1;
//   const key_t *e2 = (const key_t *)p2;
//   if (*e1 < *e2) {
//     return -1;
//   } else if (*e1 > *e2) {
//     return 1;
//   } else {
//     return 0;
//   }
// };

// // min/max should return the min/max value of the tree
// void test_minmax(key_t *arr, const size_t n) {
//   // null array is not allowed
//   assert(n > 0 && arr != NULL);

//   rbtree *t = new_rbtree();
//   assert(t != NULL);

//   insert_arr(t, arr, n);
//   assert(t->root != NULL);
// #ifdef SENTINEL
//   assert(t->root != t->nil);
// #endif

//   qsort((void *)arr, n, sizeof(key_t), comp);
//   node_t *p = rbtree_min(t);
//   assert(p != NULL);
//   assert(p->key == arr[0]);

//   node_t *q = rbtree_max(t);
//   assert(q != NULL);
//   assert(q->key == arr[n - 1]);

//   rbtree_erase(t, p);
//   p = rbtree_min(t);
//   assert(p != NULL);
//   assert(p->key == arr[1]);

//   if (n >= 2) {
//     rbtree_erase(t, q);
//     q = rbtree_max(t);
//     assert(q != NULL);
//     assert(q->key == arr[n - 2]);
//   }

//   delete_rbtree(t);
// }

// void test_to_array(rbtree *t, const key_t *arr, const size_t n) {
//   assert(t != NULL);

//   insert_arr(t, arr, n);
//   qsort((void *)arr, n, sizeof(key_t), comp);

//   key_t *res = calloc(n, sizeof(key_t));
//   rbtree_to_array(t, res, n);
//   for (int i = 0; i < n; i++) {
//     assert(arr[i] == res[i]);
//   }
//   free(res);
// }

// void test_multi_instance() {
//   rbtree *t1 = new_rbtree();
//   assert(t1 != NULL);
//   rbtree *t2 = new_rbtree();
//   assert(t2 != NULL);

//   key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
//   const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
//   insert_arr(t1, arr1, n1);
//   qsort((void *)arr1, n1, sizeof(key_t), comp);

//   key_t arr2[] = {4, 8, 10, 5, 3};
//   const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
//   insert_arr(t2, arr2, n2);
//   qsort((void *)arr2, n2, sizeof(key_t), comp);

//   key_t *res1 = calloc(n1, sizeof(key_t));
//   rbtree_to_array(t1, res1, n1);
//   for (int i = 0; i < n1; i++) {
//     assert(arr1[i] == res1[i]);
//   }

//   key_t *res2 = calloc(n2, sizeof(key_t));
//   rbtree_to_array(t2, res2, n2);
//   for (int i = 0; i < n2; i++) {
//     assert(arr2[i] == res2[i]);
//   }

//   free(res2);
//   free(res1);
//   delete_rbtree(t2);
//   delete_rbtree(t1);
// }

// // Search tree constraint
// // The values of left subtree should be less than or equal to the current node
// // The values of right subtree should be greater than or equal to the current
// // node

// static bool search_traverse(const node_t *p, key_t *min, key_t *max,
//                             node_t *nil) {
//   if (p == nil) {
//     return true;
//   }

//   *min = *max = p->key;

//   key_t l_min, l_max, r_min, r_max;
//   l_min = l_max = r_min = r_max = p->key;

//   const bool lr = search_traverse(p->left, &l_min, &l_max, nil);
//   if (!lr || l_max > p->key) {
//     return false;
//   }
//   const bool rr = search_traverse(p->right, &r_min, &r_max, nil);
//   if (!rr || r_min < p->key) {
//     return false;
//   }

//   *min = l_min;
//   *max = r_max;
//   return true;
// }

// void test_search_constraint(const rbtree *t) {
//   assert(t != NULL);
//   node_t *p = t->root;
//   key_t min, max;
// #ifdef SENTINEL
//   node_t *nil = t->nil;
// #else
//   node_t *nil = NULL;
// #endif
//   assert(search_traverse(p, &min, &max, nil));
// }

// // Color constraint
// // 1. Each node is either red or black. (by definition)
// // 2. All NIL nodes are considered black.
// // 3. A red node does not have a red child.
// // 4. Every path from a given node to any of its descendant NIL nodes goes
// // through the same number of black nodes.

// bool touch_nil = false;
// int max_black_depth = 0;

// static void init_color_traverse(void) {
//   touch_nil = false;
//   max_black_depth = 0;
// }

// static bool color_traverse(const node_t *p, const color_t parent_color,
//                            const int black_depth, node_t *nil) {
//   if (p == nil) {
//     if (!touch_nil) {
//       touch_nil = true;
//       max_black_depth = black_depth;
//     } else if (black_depth != max_black_depth) {
//       return false;
//     }
//     return true;
//   }
//   if (parent_color == RBTREE_RED && p->color == RBTREE_RED) {
//     return false;
//   }
//   int next_depth = ((p->color == RBTREE_BLACK) ? 1 : 0) + black_depth;
//   return color_traverse(p->left, p->color, next_depth, nil) &&
//          color_traverse(p->right, p->color, next_depth, nil);
// }

// void test_color_constraint(const rbtree *t) {
//   assert(t != NULL);
// #ifdef SENTINEL
//   node_t *nil = t->nil;
// #else
//   node_t *nil = NULL;
// #endif
//   node_t *p = t->root;
//   assert(p == nil || p->color == RBTREE_BLACK);

//   init_color_traverse();
//   assert(color_traverse(p, RBTREE_BLACK, 0, nil));
// }

// // rbtree should keep search tree and color constraints
// void test_rb_constraints(const key_t arr[], const size_t n) {
//   rbtree *t = new_rbtree();
//   assert(t != NULL);

//   insert_arr(t, arr, n);
//   assert(t->root != NULL);

//   test_color_constraint(t);
//   test_search_constraint(t);

//   delete_rbtree(t);
// }

// // rbtree should manage distinct values
// void test_distinct_values() {
//   const key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
//   const size_t n = sizeof(entries) / sizeof(entries[0]);
//   test_rb_constraints(entries, n);
// }

// // rbtree should manage values with duplicate
// void test_duplicate_values() {
//   const key_t entries[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
//   const size_t n = sizeof(entries) / sizeof(entries[0]);
//   test_rb_constraints(entries, n);
// }

// void test_minmax_suite() {
//   key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
//   const size_t n = sizeof(entries) / sizeof(entries[0]);
//   test_minmax(entries, n);
// }

// void test_to_array_suite() {
//   rbtree *t = new_rbtree();
//   assert(t != NULL);

//   key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
//   const size_t n = sizeof(entries) / sizeof(entries[0]);
//   test_to_array(t, entries, n);

//   delete_rbtree(t);
// }

// void test_find_erase(rbtree *t, const key_t *arr, const size_t n) {
//   for (int i = 0; i < n; i++) {
//     node_t *p = rbtree_insert(t, arr[i]);
//     assert(p != NULL);
//   }

//   for (int i = 0; i < n; i++) {
//     node_t *p = rbtree_find(t, arr[i]);
//     printf("arr[%d] = %d\n", i, arr[i]);
//     assert(p != NULL);
//     assert(p->key == arr[i]);
//     rbtree_erase(t, p);
//   }

//   for (int i = 0; i < n; i++) {
//     node_t *p = rbtree_find(t, arr[i]);
//     assert(p == NULL);
//   }

//   for (int i = 0; i < n; i++) {
//     node_t *p = rbtree_insert(t, arr[i]);
//     assert(p != NULL);
//     node_t *q = rbtree_find(t, arr[i]);
//     assert(q != NULL);
//     assert(q->key == arr[i]);
//     assert(p == q);
//     rbtree_erase(t, p);
//     q = rbtree_find(t, arr[i]);
//     assert(q == NULL);
//   }
// }

// void test_find_erase_fixed() {
//   const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
//   const size_t n = sizeof(arr) / sizeof(arr[0]);
//   rbtree *t = new_rbtree();
//   assert(t != NULL);

//   test_find_erase(t, arr, n);

//   delete_rbtree(t);
// }

// void test_find_erase_rand(const size_t n, const unsigned int seed) {
//   srand(seed);
//   rbtree *t = new_rbtree();
//   key_t *arr = calloc(n, sizeof(key_t));
//   for (int i = 0; i < n; i++) {
//     arr[i] = rand();
//   }

//   test_find_erase(t, arr, n);

//   free(arr);
//   delete_rbtree(t);
// }

// int main(void) {
//   // test_init();
//   // test_insert_single(1024);
//   // test_find_single(512, 1024);
//   // test_erase_root(128);
//   // test_find_erase_fixed();
//   // test_minmax_suite();
//   // test_to_array_suite();
//   // test_distinct_values();
//   // test_duplicate_values();
//   test_multi_instance();
//   // test_find_erase_rand(10000, 17);
//   printf("Passed all tests!\n");
// }