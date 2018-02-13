#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "options.h"
#include "scanner.h"

ConfigOpts *getOptions(int argc, char **argv) {
    ConfigOpts *conf = (ConfigOpts*)malloc(sizeof(ConfigOpts));
    conf->gflag = conf->oflag = conf->vflag = 0;
    conf->corpus = conf->cmds = conf->outfile = 0;
    int c;
    while ((c = getopt(argc, argv, "grv")) != -1) {
        switch(c) {
            case 'g':
                conf->gflag = 1;
                break;
            case 'r':
                conf->gflag = 0;
                break;
            case 'v':
                conf->vflag = 1;
                break;
        }
    }
    conf->optind = optind;
    return conf;       
}

void getCorpus(ConfigOpts *conf, int argc, char **argv) {
    if (conf->optind == argc) {
        printf("Error: No corpus supplied.\n");
        exit(1);
    }
    int len = strlen(argv[conf->optind]) + 1;
    conf->corpus = (char *)malloc(len * sizeof(char));
    strcpy(conf->corpus, argv[conf->optind]);
    conf->optind += 1;
}

void getCmds(ConfigOpts *conf, int argc, char **argv) {
    if (conf->optind == argc) {
        printf("Error: No commands supplied.\n");
        exit(1);
    }
    int len = strlen(argv[conf->optind]) + 1;
    conf->cmds = (char *)malloc(len * sizeof(char));
    strcpy(conf->cmds, argv[conf->optind]);
    conf->optind += 1;
    if (conf->optind != argc) {
        getOutfile(conf, argv);
    }
}

void getOutfile(ConfigOpts *conf, char **argv) {
    conf->oflag = 1;
    int len = strlen(argv[conf->optind]) + 1;
    conf->outfile = (char *)malloc(len * sizeof(char));
    strcpy(conf->outfile, argv[conf->optind]);
    conf->optind += 1;    
}
