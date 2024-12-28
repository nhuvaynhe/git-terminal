#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <ncurses/ncurses.h>

#include "enum.h"

#define MAX_ITEMS   100

#define ARROW_UP       KEY_UP
#define ARROW_DOWN     KEY_DOWN

typedef struct Branches {
    char *branches[MAX_ITEMS];
    int total;
    int currentIdx;
} Branches;

Branches mybranch;
Branches *mybrch = &mybranch; 

static MainProc currentProc = Proc_NONE;
static ProcCommand currentCmd = PROC_CMD_NONE;

#ifdef _WIN32
#include <windows.h>
void hide_cursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
#endif

static void SetNextProc(MainProc proc)
{
    currentProc = proc;
}

static void SetNextCmd(ProcCommand cmd)
{
    currentCmd = cmd;
}

static int AddBranches(FILE* pPipe)
{
    char buffer[128];
    int i = 0;

    while (fgets(buffer, 128, pPipe)) {
        /* Remove trailing newline */
        buffer[strcspn(buffer, "\n")] = 0;

        if (i < MAX_ITEMS) {
            mybrch->branches[i] = malloc(sizeof(buffer) + 1);
            if (mybrch->branches[i] == NULL) {
                printf("Malloc failed.\n");
                exit(1);
            }

            strcpy(mybrch->branches[i], buffer);

            mybrch->total++;
            i++;
        }
        else {
            printf("Too many items!");
            exit(1);
        }
    }
}


static void DumpBranches()
{
    static int displayAllBranches = 1;
    
    for (int i = 0; i < mybrch->total; i++) {
        if (i == mybrch->currentIdx) {
            mvprintw(i, 0, "--->"); // Draw the marker for the current index
        } else if (i == mybrch->currentIdx - 1 || i == mybrch->currentIdx + 1) {
            mvprintw(i, 0, "    "); // Only clear the marker for adjacent rows
        }
        
        if (displayAllBranches) {
            mvprintw(i, 5, "%d: %s",  i+1, mybrch->branches[i]);
        }
    }
    
    displayAllBranches = 0;
    refresh();
}

static void KeyProcess()
{
    switch (currentCmd)
    {
        case Proc_CMD_DOWN:
        {
            if (mybrch->currentIdx >= mybrch->total-1) {
                break;
            }
            mybrch->currentIdx++;

            SetNextCmd(PROC_CMD_NONE);
            SetNextProc(Proc_UPDATE_UI);
        } break;
        
        case Proc_CMD_UP:
        {
            if (mybrch->currentIdx < 1) {
                break;
            }
            mybrch->currentIdx--;

            SetNextCmd(PROC_CMD_NONE);
            SetNextProc(Proc_UPDATE_UI);
        } break;

        default:
        {
            ;
        }
    }
}

static void MainProcess()
{
    switch(currentProc)
    {
        case Proc_UPDATE_UI:
        {
            DumpBranches();
            SetNextProc(Proc_PROCESS);
        } break;

        case Proc_PROCESS:
        {
            KeyProcess();
        } break;

        default:
        {
            ;
        }
    }
}

static void KeyHandler()
{
    int key = getch();
    switch (key)
    {
        case ARROW_UP: 
        case Key_UP:
        {
            SetNextCmd(Proc_CMD_UP);
        } break;

        case ARROW_DOWN:
        case Key_DOWN:
        {
            SetNextCmd(Proc_CMD_DOWN);
        } break;

        case Key_SELECT:
        {
            printf("You press ENTER.\n");
        } break;

        case Key_SEARCH:
        {
        } break;

        case Key_QUIT:
        {
            exit(1);
        } break;

        default:
        {
        } break;
    }
}


int main(void)
{
    FILE* pPipe;

    if ((pPipe = _popen("git branch -a", "rt")) == NULL) {
        exit(1);
    }
    
    AddBranches(pPipe);
    SetNextProc(Proc_UPDATE_UI);

    feof(pPipe);
    _pclose(pPipe);

    /* Init ncurses */
    initscr();             // Initialize ncurses
    cbreak();              // Disable line buffering
    noecho();              // Disable echoing of input
    keypad(stdscr, TRUE);  // Enable special keys (arrows, etc.)
    curs_set(0);           // Hide the cursor
    nodelay(stdscr, TRUE);
    
    while(1)
    {
        MainProcess();
        KeyHandler();
    }

    endwin();
    return 0;
}