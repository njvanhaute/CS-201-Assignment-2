#include <stdlib.h>
#include "bst.h"
#include "gt.h"

typedef struct gtvalue {
    void *value;
    int freq;
    void (*display)(FILE *, void *);
    int (*compare)(void *, void *);
} GTVALUE;

struct gt {
    BST *bst;
    int freqTotal;
    void (*display)(FILE *, void *);
    int (*compare)(void *, void *);
};

static GTVALUE *newGTVALUE(GT *gt, void *value);
static void displayGTVALUE(FILE *fp, void *value);
static int compareGTVALUE(void *a, void *b);
static void swapGTVALUE(BSTNODE *a, BSTNODE *b);

// BEGIN GT METHODS

GT *newGT(void (*display)(FILE *, void *), int (*compare)(void *, void *)) {
    GT *gt = (GT *)malloc(sizeof(GT));
    gt->display = display;
    gt->compare = compare;
    gt->bst = newBST(displayGTVALUE, compareGTVALUE, swapGTVALUE);
    gt->freqTotal = 0;
    return gt;
}

void insertGT(GT *gt, void *value) {
    GTVALUE *gtval = newGTVALUE(gt, value); // Wrap value in GTVALUE
    BSTNODE *searchNode = findBST(gt->bst, gtval);
    if (searchNode != NULL) { // Val is already in tree, so increment freq
        GTVALUE *searchVal = getBSTNODE(searchNode);
        searchVal->freq += 1;
    }
    else {
        insertBST(gt->bst, gtval);
    }
    gt->freqTotal += 1;
}

int findGT(GT *gt, void *value) {
    GTVALUE *gtval = newGTVALUE(gt, value);
    BSTNODE *searchNode = findBST(gt->bst, gtval);
    if (searchNode == 0) return 0;
    GTVALUE *searchVal = getBSTNODE(searchNode);
    return searchVal->freq;
}

void deleteGT(GT *gt, void *value) {
    GTVALUE *gtval = newGTVALUE(gt, value);
    BSTNODE *searchNode = findBST(gt->bst, gtval);
    if (searchNode == 0) {
        printf("Value ");
        gtval->display(stdout, value);
        printf(" not found.\n");
        return;
    }
    GTVALUE *searchVal = getBSTNODE(searchNode);
    if (searchVal->freq > 1) {
        searchVal->freq -= 1;
    }
    else {
        deleteBST(gt->bst, gtval);
    }
    gt->freqTotal -= 1;
}

int sizeGT(GT *gt) {
    return sizeBST(gt->bst);
}

int wordsGT(GT *gt) {
    return gt->freqTotal;
}

void statisticsGT(FILE *fp, GT *gt) {
    fprintf(fp, "Words/Phrases: %d\n", wordsGT(gt));
    statisticsBST(fp, gt->bst);
}

void displayGT(FILE *fp, GT *gt) {
    displayBST(fp, gt->bst);
}
// END GT METHODS

// BEGIN STATIC METHODS

static GTVALUE *newGTVALUE(GT *gt, void *value) {
    GTVALUE *gtval = (GTVALUE *)malloc(sizeof(GTVALUE));
    gtval->value = value;
    gtval->freq = 1;
    gtval->display = gt->display;
    gtval->compare = gt->compare;
    return gtval;
}

static void displayGTVALUE(FILE *fp, void *value) {
    GTVALUE *gtval = (GTVALUE *)value;
    gtval->display(fp, gtval->value);
    if (gtval->freq > 1) fprintf(fp, "-%d", gtval->freq);
}

static int compareGTVALUE(void *a, void *b) {
    GTVALUE *gtva = (GTVALUE *)a;
    GTVALUE *gtvb = (GTVALUE *)b;
    return gtva->compare(gtva->value, gtvb->value);
}

static void swapGTVALUE(BSTNODE *a, BSTNODE *b) {
    GTVALUE *gtva = getBSTNODE(a);
    GTVALUE *gtvb = getBSTNODE(b);
    void *vtemp = gtva->value;
    gtva->value = gtvb->value;
    gtvb->value = vtemp;
    int ftemp = gtva->freq;
    gtva->freq = gtvb->freq;
    gtvb->freq = ftemp;
}
// END STATIC METHODS
