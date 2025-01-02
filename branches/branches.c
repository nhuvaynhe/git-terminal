#include <stdlib.h>
#include <stdio.h>

#include "branches.h"

Branches mybranch;
Branches searchBranch;

Branches *mybrch = &mybranch;         
Branches *searchBuffer = &searchBranch;  

static void FilterInput(char* s) {
    char* d = s;
    do {
        while (*d == ' ' || *d == '*') {
            ++d;
        }
    } while (*s++ = *d++);
}

static void UpdateRowArrow(int highlightIdx, int row)
{
    if (row == highlightIdx) {
        mvprintw(row, 0, "--->"); 
    } 
    else if (row == highlightIdx - 1 || row == highlightIdx + 1) {
        mvprintw(row, 0, "    "); 
    }
}

int AddBranches(FILE* pPipe, Branches *mybranch)
{
    char buffer[128];
    int i = 0;

    while (fgets(buffer, 128, pPipe)) {
        /* Remove trailing newline */
        buffer[strcspn(buffer, "\n")] = 0;

        if (i < MAX_ITEMS) {
            mybranch->branches[i] = malloc(sizeof(buffer) + 1);
            if (mybranch->branches[i] == NULL) {
                exit(1);
            }

            FilterInput(buffer);
            strcpy(mybranch->branches[i], buffer);

            mybranch->total++;
            i++;
        }
        else {
            printf("Too many items!");
            exit(1);
        }
    }
}

void DumpBranches(const char *query) {
    int idx = 0;

    // Clear search buffer if it contains results
    if (searchBuffer->total > 0) {
        for (int i = 0; i < searchBuffer->total; i++) {
            free(searchBuffer->branches[i]);
            searchBuffer->branches[i] = NULL;
        }
        searchBuffer->total = 0;
        searchBuffer->currentIdx = 0;
    }
    
    // Process query
    for (int row = 0; row < mybrch->total; row++) {
        if (query == NULL) {
            mvprintw(row, 5, "%d: %s", row + 1, mybrch->branches[row]);
            UpdateRowArrow(mybrch->currentIdx, row);
        } else {
            move(row, 0);
            clrtoeol();
            if (strstr(mybrch->branches[row], query) != NULL) {
                searchBuffer->branches[idx] = malloc(strlen(mybrch->branches[row]) + 1);
                if (searchBuffer->branches[idx] == NULL) {
                    perror("Malloc failed");
                    exit(EXIT_FAILURE);
                }
                strcpy(searchBuffer->branches[idx], mybrch->branches[row]);
                searchBuffer->total++;
                idx++;
            }
        }
    }

    // Display search buffer if it contains results
    if (searchBuffer->total > 0) {
        for (int row = 0; row < searchBuffer->total; row++) {
            mvprintw(row, 5, "%d: %s", row + 1, searchBuffer->branches[row]);
            UpdateRowArrow(searchBuffer->currentIdx, row);
        }
    }
}
