#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bst.h"
#include "gt.h"
#include "rbt.h"
#include "integer.h"
#include "string.h"
#include "options.h"
#include "scanner.h"

static void strcleanspace(char *src);
static void strclean(char *src);
static void removeMultiSpace(char *str);
static char *trimWhiteSpace(char *str);

int main(int argc, char **argv) { 
    ConfigOpts *conf = getOptions(argc, argv);
    if (conf->vflag == 1) {
        printf("Nicholas J. Vanhaute\n");
        return 0;
    }
    getCorpus(conf, argc, argv);
    getCmds(conf, argc, argv);
    RBT *rbt = newRBT(displaySTRING, compareSTRING);
    GT *gt = newGT(displaySTRING, compareSTRING);
    FILE *fp = stdout;
    if (conf->outfile != NULL) fp = fopen(conf->outfile, "w"); 
    FILE *corp = fopen(conf->corpus, "r");
    FILE *cmds = fopen(conf->cmds, "r");
    if (fp == NULL || corp == NULL || cmds == NULL) {
        printf("Error: No such file.\n");
        return 1;
    }
    int isString = stringPending(corp); 
    while (!feof(corp)) {
        char *tok;
        if (isString) {
            tok = readString(corp);
            strcleanspace(tok);
            removeMultiSpace(tok);
            tok = trimWhiteSpace(tok);
        }
        else {
            tok = readToken(corp);
            strclean(tok);
        }
        if (conf->gflag == 1) {
            if (tok[0] != '\0') {
                insertGT(gt, newSTRING(tok));
            }
        }
        else {
            if (tok[0] != '\0') {
                insertRBT(rbt, newSTRING(tok));
            }
        }
        isString = stringPending(corp);
    }
    fclose(corp);
    char *cmd = readToken(cmds);
    while (!feof(cmds)) {
        if (strcmp(cmd, "s") == 0) {
            if (conf->gflag == 1) {
                if (sizeGT(gt) > 0) displayGT(fp, gt);
                else fprintf(fp, "EMPTY\n");
            }
            else {
                if (sizeRBT(rbt) > 0) displayRBT(fp, rbt);
                else fprintf(fp, "EMPTY\n");
            }
        }
        else if (strcmp(cmd, "r") == 0) {
            if (conf->gflag == 1) {
                statisticsGT(fp, gt);
            }
            else {
                statisticsRBT(fp, rbt);
            }
        }
        else if (strcmp(cmd, "f") == 0) {
            isString = stringPending(cmds);
            char *searchTerm;
            if (isString) {
                searchTerm = readString(cmds);
                strcleanspace(searchTerm);
                removeMultiSpace(searchTerm);
                searchTerm = trimWhiteSpace(searchTerm);
            }
            else {
                searchTerm = readToken(cmds);
                strclean(searchTerm);
            }
            if (conf->gflag == 1) {
                fprintf(fp, "Frequency of %s: %d\n", searchTerm, findGT(gt, newSTRING(searchTerm)));
            }
            else {
                fprintf(fp, "Frequency of %s: %d\n", searchTerm, findRBT(rbt, newSTRING(searchTerm)));
            }
        }
        else if (strcmp(cmd, "d") == 0) {
            isString = stringPending(cmds);
            char *deletion;
            if (isString) {
                deletion = readString(cmds);
                strcleanspace(deletion);
                removeMultiSpace(deletion);
                deletion = trimWhiteSpace(deletion);
            }
            else {
                deletion = readToken(cmds);
                strclean(deletion);
            }  
            if (conf->gflag == 1) { 
                deleteGT(gt, newSTRING(deletion));  

            }
            else { 
                deleteRBT(rbt, newSTRING(deletion)); 

            }

        }
        else if (strcmp(cmd, "i") == 0) {
            isString = stringPending(cmds);
            char *insertion;
            if (isString) {
                insertion = readString(cmds);
                strcleanspace(insertion);
                removeMultiSpace(insertion);
                insertion = trimWhiteSpace(insertion);
            }
            else {
                insertion = readToken(cmds);
                strclean(insertion);
            }
            if (conf->gflag == 1) {
                if (insertion[0] != '\0') {
                    insertGT(gt, newSTRING(insertion));
                }
            }
            else {
                if (insertion[0] != '\0') {
                    insertRBT(rbt, newSTRING(insertion));
                }
            }
        }
        free(cmd);
        cmd = readToken(cmds);
    }
    fclose(cmds);
    if (fp != stdout) {
        fclose(fp);
    }
    return 0;
}

static void strcleanspace(char *src) {
    char *dst = src;
    while (*src) {
        if (isupper(*src)) *src = tolower(*src);
        if (isspace(*src)) *src = ' ';
        if (islower(*src) || (*src == ' ')) *dst++ = *src;
        src++;
    }
    *dst = '\0';
}

static void strclean(char *src) {
    char *dst = src;
    while (*src) {
        if (isupper(*src)) *src = tolower(*src);
        if (islower(*src)) *dst++ = *src;
        src++;
    }
    *dst = '\0';
}


static void removeMultiSpace(char *str) {
    char *dest = str;
    while (*str != '\0') {
        while (*str == ' ' && *(str + 1) == ' ')
            str++;

        *dest++ = *str++;
    }
    *dest = '\0';
}

static char *trimWhiteSpace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    return str;
}
