#ifndef __BRANCHES_H__
#define __BRANCHES_H__

#include <string.h>
#include <stdlib.h>
#include <ncurses/ncurses.h>

#define MAX_ITEMS   100

typedef struct Branches {
    char *branches[MAX_ITEMS];
    int total;
    int currentIdx;
} Branches;


int AddBranches(FILE* pPipe, Branches *mybranch);
void DumpBranches(const char *query);

#endif