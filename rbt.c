#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bst.h"
#include "rbt.h"

typedef struct rbtvalue {
    void *value;
    char color;
    int freq;
    void (*display)(FILE *, void *);
    int (*compare)(void *, void *);
} RBTVALUE;

struct rbt {
    BST *bst;
    int freqTotal;
    void (*display)(FILE *, void *);
    int (*compare)(void *, void *);
};

static RBTVALUE *newRBTVALUE(RBT *r, void *value);
static void displayRBTVALUE(FILE *fp, void *value);
static int compareRBTVALUE(void *a, void *b);
static void swapRBTVALUE(BSTNODE *a, BSTNODE *b);
static bool isLeftChild(BSTNODE *x);
static bool isRightChild(BSTNODE *x);
static char color(BSTNODE *x);
static void setColor(BSTNODE *x, char color);
static BSTNODE *sibling(BSTNODE *x);
static BSTNODE *nephew(BSTNODE *x);
static BSTNODE *niece(BSTNODE *x);
static BSTNODE *uncle(BSTNODE *x);
static BSTNODE *grandparent(BSTNODE *x);
static bool linear(BSTNODE *x);
static void leftRotate(BST *t, BSTNODE *x);
static void rightRotate(BST *t, BSTNODE *x);
static void rotateToParent(BST *t, BSTNODE *x);
static void insertionFixUp(BST *t, BSTNODE *n);
static void deletionFixUp(BST *t, BSTNODE *n);

// BEGIN RBT METHODS

RBT *newRBT(void (*display)(FILE *, void *), int (*compare)(void *, void *)) {
    RBT *rbt = (RBT *)malloc(sizeof(RBT));
    rbt->display = display;
    rbt->compare = compare;
    rbt->bst = newBST(displayRBTVALUE, compareRBTVALUE, swapRBTVALUE);
    rbt->freqTotal = 0;
    return rbt;
}

void insertRBT(RBT *r, void *value) {
    RBTVALUE *rbtval = newRBTVALUE(r, value); // Wrap value in RBTVALUE...
    BSTNODE *searchNode = findBST(r->bst, rbtval);
    if (searchNode != NULL) {
        RBTVALUE *searchVal = getBSTNODE(searchNode);
        searchVal->freq += 1;
    }
    else {
        BSTNODE *x = insertBST(r->bst, rbtval); // ...and insert into BST.
        insertionFixUp(r->bst, x);
    }
    r->freqTotal += 1;
}

int findRBT(RBT *r, void *value) {
    RBTVALUE *rbtval = newRBTVALUE(r, value);
    BSTNODE *searchNode = findBST(r->bst, rbtval);
    if (searchNode == 0) return 0;
    RBTVALUE *searchVal = getBSTNODE(searchNode);
    return searchVal->freq;
}

void deleteRBT(RBT *r, void *value) {
    RBTVALUE *rbtval = newRBTVALUE(r, value);
    BSTNODE *searchNode = findBST(r->bst, rbtval);
    if (searchNode == 0) {
        printf("Value ");
        rbtval->display(stdout, value);
        printf(" not found.\n");
        return;
    }
    RBTVALUE *searchVal = getBSTNODE(searchNode);
    if (searchVal->freq > 1) {
        searchVal->freq -= 1;
    }
    else {
        BSTNODE *leaf = swapToLeafBST(r->bst, searchNode);
        deletionFixUp(r->bst, leaf);
        pruneLeafBST(r->bst, leaf);
    }
    r->freqTotal -= 1;
}

int sizeRBT(RBT *r) {
    return sizeBST(r->bst);
}

int wordsRBT(RBT *r) {
    return r->freqTotal;
}

void statisticsRBT(FILE *fp, RBT *r) {
    fprintf(fp, "Words/Phrases: %d\n", wordsRBT(r));
    statisticsBST(fp, r->bst);
}

void displayRBT(FILE *fp, RBT *r) {
    displayBST(fp, r->bst);
}

// END RBT METHODS

// BEGIN STATIC METHODS

static RBTVALUE *newRBTVALUE(RBT *r, void *value) {
    RBTVALUE *rbtval = (RBTVALUE *)malloc(sizeof(RBTVALUE));
    rbtval->value = value;
    rbtval->color = 'R'; // all newly inserted nodes start out red
    rbtval->freq = 1;
    rbtval->display = r->display;
    rbtval->compare = r->compare;
    return rbtval;
}

static void displayRBTVALUE(FILE *fp, void *value) {
    RBTVALUE *rbtval = (RBTVALUE *)value;
    rbtval->display(fp, rbtval->value);
    if (rbtval->freq > 1) fprintf(fp, "-%d", rbtval->freq);
    fprintf(fp, "-%c", rbtval->color);
}

static int compareRBTVALUE(void *a, void *b) {
    RBTVALUE *rbtva = (RBTVALUE *)a;
    RBTVALUE *rbtvb = (RBTVALUE *)b;
    return rbtva->compare(rbtva->value, rbtvb->value);
}

static void swapRBTVALUE(BSTNODE *a, BSTNODE *b) {
    RBTVALUE *rbtva = getBSTNODE(a);
    RBTVALUE *rbtvb = getBSTNODE(b);
    void *vtemp = rbtva->value;
    rbtva->value = rbtvb->value;
    rbtvb->value = vtemp;
    int ftemp = rbtva->freq;
    rbtva->freq = rbtvb->freq;
    rbtvb->freq = ftemp;
}

static bool isLeftChild(BSTNODE *x) {
    return x == getBSTNODEleft(getBSTNODEparent(x));
}

static bool isRightChild(BSTNODE *x) {
    return !isLeftChild(x);
}

static char color(BSTNODE *x) {
    if (x == NULL) return 'B';
    RBTVALUE *rbtv = getBSTNODE(x);
    return rbtv->color;
}

static void setColor(BSTNODE *x, char color) {
    assert(color == 'R' || color == 'B');
    RBTVALUE *rbtv = getBSTNODE(x);
    rbtv->color = color;
}

static BSTNODE *sibling(BSTNODE *x) {
    if (isLeftChild(x)) return getBSTNODEright(getBSTNODEparent(x));
    else return getBSTNODEleft(getBSTNODEparent(x));
}

static BSTNODE *nephew(BSTNODE *x) {
    if (isLeftChild(x)) return getBSTNODEright(sibling(x));
    else return getBSTNODEleft(sibling(x));
}

static BSTNODE *niece(BSTNODE *x) {
    if (isLeftChild(x)) return getBSTNODEleft(sibling(x));
    else return getBSTNODEright(sibling(x));
}

static BSTNODE *uncle(BSTNODE *x) {
    if (isLeftChild(getBSTNODEparent(x))) return getBSTNODEright(grandparent(x));
    else return getBSTNODEleft(grandparent(x));
}

static BSTNODE *grandparent(BSTNODE *x) {
    return getBSTNODEparent(getBSTNODEparent(x));
}

static bool linear(BSTNODE *x) {
    return (isLeftChild(x) && isLeftChild(getBSTNODEparent(x))) || (isRightChild(x) && isRightChild(getBSTNODEparent(x)));
}

static void leftRotate(BST *t, BSTNODE *x) {
    BSTNODE *y = getBSTNODEright(x);
    setBSTNODEright(x, getBSTNODEleft(y));
    if (getBSTNODEleft(y) != NULL) {
        BSTNODE *yLeft = getBSTNODEleft(y);
        setBSTNODEparent(yLeft, x);
    }
    setBSTNODEparent(y, getBSTNODEparent(x));
    if (getBSTNODEparent(x) == x) {
        setBSTroot(t, y);
        setBSTNODEparent(y, y);
    }
    else if (isLeftChild(x)) {
        setBSTNODEleft(getBSTNODEparent(x), y);
    }
    else {
        setBSTNODEright(getBSTNODEparent(x), y);
    }
    setBSTNODEleft(y, x);
    setBSTNODEparent(x, y);
}

static void rightRotate(BST *t, BSTNODE *x) {
    BSTNODE *y = getBSTNODEleft(x);
    setBSTNODEleft(x, getBSTNODEright(y));
    if (getBSTNODEright(y) != NULL) {
        BSTNODE *yRight = getBSTNODEright(y);
        setBSTNODEparent(yRight, x);
    }
    setBSTNODEparent(y, getBSTNODEparent(x));
    if (getBSTNODEparent(x) == x) {
        setBSTroot(t, y);
        setBSTNODEparent(y, y);
    }
    else if (isLeftChild(x)) {
        setBSTNODEleft(getBSTNODEparent(x), y);
    }
    else {
        setBSTNODEright(getBSTNODEparent(x), y);
    }
    setBSTNODEright(y, x);
    setBSTNODEparent(x, y);
}

static void rotateToParent(BST *t, BSTNODE *x) {
    if (isLeftChild(x)) rightRotate(t, getBSTNODEparent(x));
    else leftRotate(t, getBSTNODEparent(x));
}

static void insertionFixUp(BST *t, BSTNODE *n) {
    BSTNODE *x = n;
    while (true) {
        if (x == getBSTroot(t)) break;
        if (color(getBSTNODEparent(x)) == 'B') break;
        if (color(uncle(x)) == 'R') {
            setColor(getBSTNODEparent(x), 'B');
            setColor(uncle(x), 'B');
            setColor(grandparent(x), 'R');
            x = grandparent(x);
        }
        else {
            BSTNODE *parent = getBSTNODEparent(x);
            if (!linear(x)) {
                BSTNODE *oldX = x; 
                rotateToParent(t, x);
                parent = oldX;
            }
            setColor(parent, 'B');
            setColor(getBSTNODEparent(parent), 'R');
            rotateToParent(t, parent);
            break;
        }
    }
    setColor(getBSTroot(t), 'B');
}

static void deletionFixUp(BST *t, BSTNODE *n) {
    BSTNODE *x = n;
    while (true) {
        if (x == getBSTroot(t)) break;
        if (color(x) == 'R') break;
        if (color(sibling(x)) == 'R') {
            setColor(getBSTNODEparent(x), 'R');
            setColor(sibling(x), 'B');
            rotateToParent(t, sibling(x));
        } 
        else if (color(nephew(x)) == 'R') {
            setColor(sibling(x), color(getBSTNODEparent(x)));
            setColor(getBSTNODEparent(x), 'B');
            setColor(nephew(x), 'B');
            rotateToParent(t, sibling(x));
            break;
        }
        else if (color(niece(x)) == 'R') {
            setColor(niece(x), 'B');
            setColor(sibling(x), 'R');
            rotateToParent(t, niece(x));
        }
        else {
            setColor(sibling(x), 'R');
            x = getBSTNODEparent(x);
        }
    }
    setColor(x, 'B');
}
// END STATIC METHODS
