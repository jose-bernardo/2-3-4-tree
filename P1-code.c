#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node234 *node234;
typedef struct frac frac;

struct frac { /* Stores a fraction */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
  unsigned long long int a; /* Numerator */
  unsigned long long int b; /* Denominator */
#pragma GCC diagnostic pop
};

int n;
struct node234 *A;
node234 S;
node234 root;
static const frac emptyFrac;

struct node234 {
  struct frac V[3]; /* Key values */
  node234 p[4];     /* Pointers */
  int numKeys;
};

int ptr2loc(node234 v) {
  int r;
  r = -1;
  if (NULL != v)
    r = ((size_t)v - (size_t)A) / sizeof(struct node234);

  return (int)r;
}

void vizShow(FILE *f, int n) {
  int i;
  node234 *Q = calloc(n + 1, sizeof(node234)); /* Queue of nodes to print */
  int in = 0;
  int out = 0;

  fprintf(f, "digraph {\n");
  fprintf(f, "node [shape=record];");
  fprintf(f, "splines=false;\n");
  Q[in] = root;
  in++;
  while (NULL != Q[out]) { /* Non-empty Queue */
    i = ptr2loc(Q[out]);
    fprintf(f,
            "A%d [label=\"{<h> A[%d] |{<p0> %llu/%llu |<p1> %llu/%llu |<p2> "
            "%llu/%llu}}\"]\n",
            i, i, A[i].V[0].a, A[i].V[0].b, A[i].V[1].a, A[i].V[1].b,
            A[i].V[2].a, A[i].V[2].b);

    i = 0;
    while (NULL != Q[out]->p[i] && i < 4) {
      Q[in] = Q[out]->p[i];
      in++;
      i++;
    }
    out++;
  }

  in = 0;
  out = 0;
  Q[in] = root;
  in++;
  while (NULL != Q[out]) { /* Non-empty Queue */
    i = 0;
    while (NULL != Q[out]->p[i] && i < 4) {
      if (3 == i)
        fprintf(f, "A%d:p2:se -> A%d:h:n\n", ptr2loc(Q[out]),
                ptr2loc(Q[out]->p[i]));
      else
        fprintf(f, "A%d:p%d:sw -> A%d:h:n\n", ptr2loc(Q[out]), i,
                ptr2loc(Q[out]->p[i]));

      Q[in] = Q[out]->p[i];
      in++;
      i++;
    }
    out++;
  }

  fprintf(f, "}\n");
  free(Q);
}

void showNode(node234 v) {
  int k;
  assert(NULL != v && "Trying to show NULL node.");

  printf("node: %d ", ptr2loc(v));
  k = 0;
  while (true) {
    printf("%d ", ptr2loc(v->p[k]));
    if (3 == k)
      break;
    printf("%llu/%llu ", v->V[k].a, v->V[k].b);
    k++;
  }
  printf("\n");
}

void structLoad(void) {
  int i;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
  unsigned long long int j;
#pragma GCC diagnostic pop
  int k;
  char *tok;
  size_t len = 1 << 8;
  char *line = (char *)malloc(len * sizeof(char));

  line[0] = '#';
  while ('#' == line[0]) {
    getline(&line, &len, stdin);
  }

  tok = strtok(line, " ");
  tok = strtok(NULL, " ");
  sscanf(tok, "%d", &i);
  S = NULL;
  if (-1 != i)
    S = &A[i];

  tok = strtok(NULL, " ");
  tok = strtok(NULL, " ");
  sscanf(tok, "%d", &i);
  root = NULL;
  if (-1 != i)
    root = &A[i];

  while (-1 != getline(&line, &len, stdin) && 'X' != line[0]) {
    char *tok = strtok(line, " ");

    if (0 == strcmp("node:", tok)) {
      tok = strtok(NULL, " ");
      sscanf(tok, "%d", &i);

      k = 0;
      while (true) {
        tok = strtok(NULL, " ");
        sscanf(tok, "%llu", &j);
        A[i].p[k] = NULL;
        if (-1 != j)
          A[i].p[k] = &A[j];

        if (3 == k)
          break;

        tok = strtok(NULL, "/");
        sscanf(tok, "%llu", &j);
        A[i].V[k].a = j;
        tok = strtok(NULL, " ");
        sscanf(tok, "%llu", &j);
        A[i].V[k].b = j;
        k++;

        if (j != 0) {
          A[i].numKeys++;
        }
      }
    }
  }
  free(line);
}

int nodeType(node234 u) {
  int i = 0;
  while (u->V[i].b != 0) {
    i++;
  }

  return i;
}

bool equalFrac(frac x, frac y) { return x.a == y.a && x.b == y.b; }

bool greaterFrac(frac _x, frac _y) {
  int i = 0;
  frac x = _x, y = _y;
  while (x.b != 0 && y.b != 0) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
    unsigned long long int cfx = x.a / x.b, cfy = y.a / y.b;
#pragma GCC diagnostic pop
    if (cfx > cfy) {
      return i % 2 == 0;
    } else if (cfx < cfy) {
      return i % 2 != 0;
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
      unsigned long long int r_x = x.a % x.b, r_y = y.a % y.b;
#pragma GCC diagnostic pop
      x.a = x.b;
      y.a = y.b;
      x.b = r_x;
      y.b = r_y;
    }
    i++;
  }

  if (x.b == 0) {
    return i % 2 == 0;
  } else {
    return i % 2 != 0;
  }
}

int find(frac x, node234 v) {
  int i;
  if (v == NULL)
    return -1;

  i = 0;
  while (i < 3 && v->V[i].b != 0) {
    if (equalFrac(v->V[i], x)) {
      return (v - A);
    }

    if (greaterFrac(v->V[i], x)) {
      return find(x, v->p[i]);
    }

    i++;
  }

  return find(x, v->p[i]);
}

void inorder(node234 v) {
  int i;
  if (v == NULL) {
    return;
  }

  i = 0;
  while (i < v->numKeys) {
    inorder(v->p[i]);
    printf("%llu/%llu ", v->V[i].a, v->V[i].b);
    i++;
  }
  inorder(v->p[i]);
}

void preorder(node234 v) {
  int i;
  if (v == NULL) {
    return;
  }

  if (v->V[0].b != 0) {
    printf("%d ", ptr2loc(v));
  }

  i = 0;
  while (i < 4) {
    preorder(v->p[i]);
    i++;
  }
}

void insertInNode(frac x, node234 v) {
  int i = v->numKeys - 1;
  /* shift right keys and pointers */
  while (i >= 0 && greaterFrac(v->V[i], x)) {
    v->V[i + 1] = v->V[i];
    i--;
  }

  v->V[i + 1] = x;
  v->numKeys++;
}

node234 splitRoot() {
  node234 v = root;
  node234 newNode;

  root = S;
  S = S->p[3];
  root->V[0] = v->V[1];
  root->p[3] = NULL;
  root->numKeys = 1;

  newNode = S;
  S = S->p[3];
  newNode->V[0] = v->V[2];
  newNode->p[0] = v->p[2];
  newNode->p[1] = v->p[3];
  newNode->p[3] = NULL;
  newNode->numKeys = 1;

  /* Erase split part from child */
  v->p[2] = v->p[3] = NULL;
  v->V[1] = v->V[2] = emptyFrac;
  v->numKeys = 1;

  root->p[0] = v;
  root->p[1] = newNode;

  return root;
}

node234 splitNode(node234 parent, node234 child) {
  int i;
  node234 newNode;

  i = parent->numKeys - 1;
  /* Insert child middle fraction on parent. */
  while (i >= 0 && greaterFrac(parent->V[i], child->V[1])) {
    parent->V[i + 1] = parent->V[i];
    parent->p[i + 2] = parent->p[i + 1];
    parent->p[i + 1] = parent->p[i];
    i--;
  }
  parent->V[i + 1] = child->V[1];
  parent->numKeys++;

  /* New node to the right of the middle fraction. */
  newNode = S;
  S = S->p[3];
  newNode->V[0] = child->V[2];
  newNode->p[0] = child->p[2];
  newNode->p[1] = child->p[3];
  newNode->p[3] = NULL;
  newNode->numKeys = 1;

  /* Erase split part from child. */
  child->p[2] = child->p[3] = NULL;
  child->V[1] = child->V[2] = emptyFrac;
  child->numKeys = 1;

  parent->p[i + 1] = child;
  parent->p[i + 2] = newNode;

  return parent;
}

void insert(frac x, node234 v, node234 parent) {
  int i;
  /* No root */
  if (v == NULL) {
    v = S;
    S = v->p[3];
    v->p[3] = NULL;
    root = v;
  }
  /* Found 4-node. Split before entering. */
  if (v->numKeys == 3) {
    printf("Spliting node %d\n", ptr2loc(v));
    if (parent == NULL) {
      v = splitRoot();
    } else {
      v = splitNode(parent, v);
    }
  }

  i = 0;
  /* Search for possible interval. */
  while (i < v->numKeys) {
    if (v->p[i] != NULL && greaterFrac(v->V[i], x)) {
      insert(x, v->p[i], v);
      return;
    }
    i++;
  }

  /* Also check right-most child if it exists. */
  if (v->p[i] != NULL) {
    insert(x, v->p[i], v);
    return;
  }

  /* Leaf node can accomodate new fraction */
  insertInNode(x, v);
  printf("%d\n", ptr2loc(v));
}

void shiftNodeLeft(node234 u, int start) {
  int i = start;
  while (i < u->numKeys - 1) {
    u->V[i] = u->V[i + 1];
    u->p[i] = u->p[i + 1];
    i++;
  }
  u->p[i] = u->p[i + 1];
  u->V[u->numKeys - 1] = emptyFrac;
  u->p[u->numKeys--] = NULL;
}

node234 joinRoot() {
  node234 left = root->p[0];
  node234 right = root->p[1];
  printf("Joining nodes %d %d\n", ptr2loc(left), ptr2loc(right));

  left->V[1] = root->V[0];
  left->V[2] = right->V[0];

  left->p[2] = right->p[0];
  left->p[3] = right->p[1];

  /* Reset nodes before pushing them to the stack. */
  root->V[0] = emptyFrac;
  root->p[0] = root->p[1] = NULL;
  right->V[0] = emptyFrac;
  right->p[0] = right->p[1] = NULL;

  /* Push old root and right node back to the stack and change it */
  right->p[3] = S;
  root->p[3] = right;
  S = root;
  root = left;
  root->numKeys = 3;

  return root;
}

/* v is the right sibling */
node234 joinStealingFromRightSibling(node234 u, node234 v, node234 parent,
                                     int idx) {
  /* printf("Joining right %d %d\n", ptr2loc(v), ptr2loc(u)); */
  u->V[1] = parent->V[idx];
  u->p[2] = v->p[0];
  u->numKeys++;
  parent->V[idx] = v->V[0];

  /* Fix right node. */
  shiftNodeLeft(v, 0);

  return u;
}

/* v is the left sibling */
node234 joinStealingFromLeftSibling(node234 u, node234 v, node234 parent,
                                    int idx) {
  /* printf("Joining left %d %d\n", ptr2loc(u), ptr2loc(v)); */

  /* Insert sibling fraction in first position and shift existing to the
   * right.
   */
  u->V[1] = u->V[0];
  u->p[2] = u->p[1];
  u->p[1] = u->p[0];
  u->p[0] = v->p[v->numKeys];
  u->V[0] = parent->V[idx - 1];
  u->numKeys++;
  parent->V[idx - 1] = v->V[v->numKeys - 1];

  /* Fix left node. No shift needed. */
  v->V[v->numKeys - 1] = emptyFrac;
  v->p[v->numKeys--] = NULL;

  return u;
}

/* v is the right sibling */
node234 joinStealingFromParent(node234 u, node234 v, node234 parent, int idx) {
  int i;
  printf("Joining nodes %d %d\n", ptr2loc(u), ptr2loc(v));

  u->V[1] = parent->V[idx];
  u->V[2] = v->V[0];
  u->p[2] = v->p[0];
  u->p[3] = v->p[1];
  u->numKeys += 2;

  parent->p[idx + 1] = u;

  /* Push v into the stack. */
  v->V[0] = emptyFrac;
  v->p[0] = v->p[1] = NULL;
  v->p[3] = S;
  S = v;

  /* Fix parent node */
  i = idx;
  while (i < parent->numKeys - 1) {
    parent->V[i] = parent->V[i + 1];
    parent->p[i] = parent->p[i + 1];
    i++;
  }
  parent->V[i] = emptyFrac;
  parent->p[i] = parent->p[i + 1];
  parent->p[i + 1] = NULL;
  parent->numKeys--;

  return u;
}

bool hasRightSibling(node234 parent, int idx) { return idx < parent->numKeys; }

bool hasLeftSibling(node234 parent, int idx) { return idx > 0; }

bool hasTypeTwoRightSibling(node234 parent, int idx) {
  return parent->p[idx + 1]->numKeys == 1;
}

bool hasTypeTwoLeftSibling(node234 parent, int idx) {
  return parent->p[idx - 1]->numKeys == 1;
}

bool hasTypeTwoParentRootAndSibling(node234 parent, int idx) {
  if (parent != root || root->numKeys > 1) {
    return false;
  }

  if (hasRightSibling(parent, idx)) {
    return hasTypeTwoRightSibling(parent, idx);
  } else {
    return hasTypeTwoLeftSibling(parent, idx);
  }
}

node234 joinNodes(node234 u, node234 parent, int idx) {
  /* Root is the parent and is 2-node. */
  if (hasTypeTwoParentRootAndSibling(parent, idx)) {
    u = joinRoot();
    return u;
  }

  if (hasRightSibling(parent, idx)) {
    if (!hasTypeTwoRightSibling(parent, idx)) {
      /* Right sibling is not 2-node. */
      u = joinStealingFromRightSibling(u, parent->p[idx + 1], parent, idx);
      return u;
    } else {
      /* Right sibling is 2-node. */
      u = joinStealingFromParent(u, parent->p[idx + 1], parent, idx);
      return u;
    }
  }

  if (hasLeftSibling(parent, idx)) {
    if (!hasTypeTwoLeftSibling(parent, idx)) {
      /* Left sibling is not 2-node. */
      u = joinStealingFromLeftSibling(u, parent->p[idx - 1], parent, idx);
      return u;
    } else {
      /* Left sibling is 2-node. */
      u = joinStealingFromParent(parent->p[idx - 1], u, parent, idx - 1);
      return u;
    }
  }

  printf("AUCH\n");
  return u;
}

node234 replaceWithInorderSuccessor(frac x, node234 u, node234 parent, int idx,
                                    node234 delPtr, int delIdx) {
  int i;
  if (u->numKeys == 1) {
    u = joinNodes(u, parent, idx);
  }

  i = 0;
  while (i < u->numKeys) {
    if (equalFrac(x, u->V[i])) {
      if (u->p[0] != NULL) {
        return replaceWithInorderSuccessor(x, u->p[i + 1], u, i + 1, u, i);
      } else {
        shiftNodeLeft(u, i);
        return u;
      }
    }
    i++;
  }

  if (u->p[0] == NULL) {
    delPtr->V[delIdx] = u->V[0];
    shiftNodeLeft(u, 0);
    return u;
  }

  return replaceWithInorderSuccessor(x, u->p[0], u, 0, delPtr, delIdx);
}

void delete(frac x, node234 u, node234 parent, int idx) {
  int i;
  /* Found 2-node. Joining before entering */
  if (u != root && u->numKeys == 1) {
    u = joinNodes(u, parent, idx);
  }

  /* Search for node to delete */
  i = 0;
  while (i < u->numKeys) {
    if (equalFrac(x, u->V[i])) {
      if (u->p[0] == NULL) {
        /* If its a leaf just delete the fraction */
        shiftNodeLeft(u, i);
        printf("%d\n", ptr2loc(u));
        /* Last fraction of root */
        if (u->numKeys == 0) {
          u->p[3] = S;
          S = u;
          root = NULL;
        }
      } else {
        /* If its a inner node replace with inorder successor. */
        node234 successor =
            replaceWithInorderSuccessor(x, u->p[i + 1], u, i + 1, u, i);
        printf("%d\n", ptr2loc(successor));
      }
      return;
    }

    if (u->p[i] != NULL && greaterFrac(u->V[i], x)) {
      delete (x, u->p[i], u, i);
      return;
    }
    i++;
  }

  /* Also check right-most child if it exists. Decrement i for last parent
   * fraction.
   */
  if (u->p[i] != NULL) {
    delete (x, u->p[i], u, i);
    return;
  }

  printf("-1\n");
}

frac parseFraction(char *line) {
  frac x;
  char *tok = strtok(line, " ");
  tok = strtok(NULL, "/");
  sscanf(tok, "%llu", &x.a);
  tok = strtok(NULL, " ");
  sscanf(tok, "%llu", &x.b);
  return x;
}

void parseInput() {
  size_t len = 1 << 8;
  char *line = (char *)malloc(len * sizeof(char));
  char command;
  frac x;
  int i;

  while (-1 != getline(&line, &len, stdin) && (command = line[0])) {
    switch (command) {
    case 'S':
      sscanf(line, "S %d", &i);
      showNode(&A[i]);
      break;
    case 'F':
      x = parseFraction(line);
      printf("%d\n", find(x, root));
      break;
    case 'N':
      inorder(root);
      printf("\n");
      break;
    case 'P':
      preorder(root);
      printf("\n");
      break;
    case 'I':
      x = parseFraction(line);
      insert(x, root, NULL);
      break;
    case 'D':
      x = parseFraction(line);
      delete (x, root, NULL, -1);
      break;
    case '#':
      printf("%s", line);
      break;
    case 'X':
      free(line);
      return;
    case 'L':
      structLoad();
      break;
    default:
      printf("this should never happen\n");
    }
  }
  free(line);
}

int main() {
  int fc = 0, i;
  char fname[20];
  FILE *fistr;
  scanf("%d\n", &n);
  A = (node234)calloc(n, sizeof(struct node234));
  /* Prepare stack */
  for (i = n - 1; i > 0; i--) {
    A[i].p[3] = &A[i - 1];
  }
  S = &A[n - 1];
  parseInput();
  printf("Final configuration:\n");
  printf("S: %d ", ptr2loc(S));
  printf("root: %d \n", ptr2loc(root));
  for (i = 0; i < n; i++) {
    showNode(&A[i]);
  }
  printf("X\n");
#ifndef NDEBUG
  sprintf(fname, "tree%.3d.dot", fc);
  fistr = fopen(fname, "w");
  vizShow(fistr, n);
  fclose(fistr);
  fc++;
#endif /* NDEBUG */
  free(A);
  return 0;
}
