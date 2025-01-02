#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <ncurses/ncurses.h>

#include "enum.h"
#include "branches.h"
#include "keyproc.h"
#include "proc.h"

extern Branches *mybrch;         
extern Branches *searchBuffer;  

static void NcursesInit(void);
static void NcursesExit(void);


int main(void)
{
    FILE* pPipe;

    if ((pPipe = _popen("git branch -a", "rt")) == NULL) {
        exit(1);
    }
    
    AddBranches(pPipe, mybrch);

    feof(pPipe);
    _pclose(pPipe);

    NcursesInit();
    SetNextProc(Proc_UPDATE_UI);

    while(1)
    {
        MainProcess();
        KeyHandler();
    }

    NcursesExit();

    return 0;
}

static void NcursesInit(void)
{
    /* Init ncurses */
    initscr();             // Initialize ncurses
    cbreak();              // Disable line buffering
    noecho();              // Disable echoing of input
    keypad(stdscr, TRUE);  // Enable special keys (arrows, etc.)
    curs_set(0);           // Hide the cursor
    nodelay(stdscr, TRUE);
}

static void NcursesExit(void)
{
    endwin();
}