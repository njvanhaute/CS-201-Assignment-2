#include <stdbool.h>
#include <stdlib.h>
#include "bst.h"
#include "queue.h"
#include "string.h"

struct bstnode {
    void *value;
    BSTNODE *left, *right, *parent;
};

struct bst {
    BSTNODE *root;
    int size;
    void (*display)(FILE *, void *);
    int (*compare)(void *, void *);
    void (*swap)(BSTNODE *, BSTNODE *);
};

static BSTNODE *newBSTNODE(void *value);
static bool isLeaf(BSTNODE *x);
static bool isRoot(BSTNODE *x);
static bool isLeftChild(BSTNODE *x);
static void genericSwap(BSTNODE *a, BSTNODE *b);
static BSTNODE *treeMin(BSTNODE *x);
static BSTNODE *treeMax(BSTNODE *x);
static BSTNODE *successor(BSTNODE *n);
static BSTNODE *predecessor(BSTNODE *n);
static int minDepth(BSTNODE *root);
static int maxDepth(BSTNODE *root);
static int min(int a, int b);

void *getBSTNODE(BSTNODE *n) {
    return n->value;
}

void setBSTNODE(BSTNODE *n, void *value) {
    n->value = value;
}

BSTNODE *getBSTNODEleft(BSTNODE *n) {
    return n->left;
}

void setBSTNODEleft(BSTNODE *n, BSTNODE *replacement) {
    n->left = replacement;
}

BSTNODE *getBSTNODEright(BSTNODE *n) {
    return n->right;
}

void setBSTNODEright(BSTNODE *n, BSTNODE *replacement) {
    n->right = replacement;
}

BSTNODE *getBSTNODEparent(BSTNODE *n) {
    return n->parent;
}

void setBSTNODEparent(BSTNODE *n, BSTNODE *replacement) {
    n->parent = replacement;
}

BST *newBST(void (*display)(FILE *, void *), int (*compare)(void *, void *), void (*swap)(BSTNODE *, BSTNODE *)) {
    BST *bst = (BST *)malloc(sizeof(BST));
    bst->root = NULL;
    bst->size = 0;
    bst->display = display; 
    bst->compare = compare;
    if (swap == NULL) {
        bst->swap = genericSwap;
    }
    else {
        bst->swap = swap;
    }
    return bst;    
}

void setBSTroot(BST *t, BSTNODE *replacement) {
    t->root = replacement;
}

BSTNODE *getBSTroot(BST *t) {
    return t->root;
}

BSTNODE *insertBST(BST *t, void *value) {
    BSTNODE *x = t->root;
    BSTNODE *y = NULL;
    BSTNODE *z = newBSTNODE(value);
    while (x != NULL) {
        y = x;
        if (t->compare(z->value, x->value) < 0) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NULL) { // tree was empty 
        t->root = z;
        z->parent = z;
    }
    else if (t->compare(z->value, y->value) < 0) {
        y->left = z;
    }
    else {
        y->right = z;
    }
    t->size += 1;
    return z;
}

BSTNODE *findBST(BST *t, void *value) {
    BSTNODE *x = t->root;
    while (x != NULL && t->compare(value, x->value) != 0) {
        if (t->compare(value, x->value) < 0) {
            x = x->left;
        }
        else x = x->right;
    } 
    return x;
}

BSTNODE *deleteBST(BST *t, void *value) {
    BSTNODE *candidate = findBST(t, value);
    if (candidate == NULL) {
        printf("Value ");
        t->display(stdout, value);
        printf(" not found.\n");
        return NULL;
    }
    BSTNODE *leaf = swapToLeafBST(t, candidate);
    pruneLeafBST(t, leaf);
    return leaf;
}

BSTNODE *swapToLeafBST(BST *t, BSTNODE *node) {
    BSTNODE *x = node;
    while (!isLeaf(x)) {
        BSTNODE *pre = predecessor(x);
        if (pre != NULL) {
            t->swap(pre, x);
            x = pre;
        }
        else {
            BSTNODE *suc = successor(x);
            t->swap(suc, x);
            x = suc;
        }
    }
    return x;
}

void pruneLeafBST(BST *t, BSTNODE *leaf) {
    if (isRoot(leaf)) {
        t->root = 0;
    }
    else if (leaf == leaf->parent->left) {
        leaf->parent->left = 0;
    }
    else leaf->parent->right = 0;
    t->size -= 1;
}

int sizeBST(BST *t) {
    return t->size;
}

void statisticsBST(FILE *fp, BST *t) {
    fprintf(fp, "Nodes: %d\n", sizeBST(t));
    fprintf(fp, "Minimum depth: %d\n", minDepth(t->root));
    fprintf(fp, "Maximum depth: %d\n", maxDepth(t->root));
}

void displayBST(FILE *fp, BST *t) {
    if (t->root == NULL) return;
    QUEUE *q = newQUEUE(t->display);
    enqueue(q, t->root);
    int level = 0;
    while (sizeQUEUE(q) > 0) {
        int levelNodes = sizeQUEUE(q);
        fprintf(fp, "%d: ", level);
        while (levelNodes > 0) {
            BSTNODE *node = dequeue(q);
            if (isLeaf(node)) fprintf(fp, "=");
            t->display(fp, node->value);
            fprintf(fp, "(");
            t->display(fp, node->parent->value);
            fprintf(fp, ")");
            if (isRoot(node)) {
                fprintf(fp, "-");
            }
            else if (isLeftChild(node)) {
                fprintf(fp, "-l");
            }
            else fprintf(fp, "-r");
            if (levelNodes > 1) fprintf(fp, " ");
            if (node->left != NULL) enqueue(q, node->left);
            if (node->right != NULL) enqueue(q, node->right);
            levelNodes--;
        }
        level++;
        fprintf(fp, "\n");
    }
}

// STATIC METHODS

static BSTNODE *newBSTNODE(void *value) {
    BSTNODE *bstnode = (BSTNODE *)malloc(sizeof(BSTNODE));
    bstnode->value = value;
    bstnode->left = bstnode->right = bstnode->parent = 0;
    return bstnode;
}

static bool isLeaf(BSTNODE *x) {
    return x->left == NULL && x->right == NULL;
}

static bool isRoot(BSTNODE *x) {
    return x == x->parent;
}

static bool isLeftChild(BSTNODE *x) {
    return x == x->parent->left;
}

static void genericSwap(BSTNODE *a, BSTNODE *b) {
    void *vtemp = a->value;
    a->value = b->value;
    b->value = vtemp;
}

static BSTNODE *treeMin(BSTNODE *x) {
    BSTNODE *n = x;
    while (n->left != NULL) {
        n = n->left;
    }
    return n;
}

static BSTNODE *treeMax(BSTNODE *x) {
    BSTNODE *n = x;
    while (n->right != NULL) {
        n = n->right;
    }
    return n;
}

static BSTNODE *successor(BSTNODE *n) {
    BSTNODE *x = n;
    if (x->right != NULL) {
        return treeMin(x->right);
    }
    return NULL; 
}

static BSTNODE *predecessor(BSTNODE *n) {
    BSTNODE *x = n;
    if (x->left != NULL) {
        return treeMax(x->left);
    }
    return NULL;
}

static int minDepth(BSTNODE *root) {
    if (root == NULL) return 0;
    if (root->left == NULL || root->right == NULL) return 1;
    if (!root->left) return minDepth(root->right) + 1;
    if (!root->right) return minDepth(root->left) + 1;
    return min(minDepth(root->left), minDepth(root->right)) + 1;
}

static int maxDepth(BSTNODE *root) {
    if (root == NULL) return 0;
    int l = maxDepth(root->left);
    int r = maxDepth(root->right);
    return (l > r ? l : r) + 1;
}

static int min(int a, int b) {
    return a < b ? a : b;
}
