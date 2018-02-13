#ifndef __OPTIONS_INCLUDED__
#define __OPTIONS_INCLUDED__

typedef struct ConfigOpts {
    int gflag, oflag, vflag;
    int optind;
    char *corpus, *cmds, *outfile;
} ConfigOpts;

ConfigOpts *getOptions(int argc, char **argv);
void getCorpus(ConfigOpts *conf, int argc, char **argv);
void getCmds(ConfigOpts *conf, int argc, char **argv);
void getOutfile(ConfigOpts *conf, char **argv);

#endif // options.h
