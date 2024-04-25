#include "rbtree.h"

#include <stdlib.h>
#include <assert.h>
// #include <rbtree.h>
#include <stdbool.h>
#include <stdio.h>
// #include <stdlib.h>

////////////////////////--------프로토타입추가------------//////////////////////////////
node_t *rb_successor(rbtree *t, node_t *x);
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