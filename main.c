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

static void DumpBranches(const char *query)
{
    int y = 0;
    static int displayAllBranches = 1;
    
    for (int i = 0; i < mybrch->total; i++) {
        if (query == NULL) {
            if (displayAllBranches) {
                mvprintw(i, 5, "%d: %s",  i+1, mybrch->branches[i]);
            }
        }
        else {
            move(i, 0); 
            clrtoeol();

            if (strstr(mybrch->branches[i], query) != NULL) {
                mvprintw(y++, 5, "%d: %s", y, mybrch->branches[i]);
            }
        }

        if (i == mybrch->currentIdx) {
            mvprintw(i, 0, "--->"); 
        } 
        else if (i == mybrch->currentIdx - 1 || 
                 i == mybrch->currentIdx + 1) {
            mvprintw(i, 0, "    "); // Only clear the marker for adjacent rows
        }
    }

    displayAllBranches = (query == NULL) ? 0 : 1;
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

        case Proc_CMD_SEARCH:
        {
            char buffer[100] = {0x00};
            int bufferLen = 0;
            int y = mybrch->total;
            int update = 0;

            curs_set(1);      
            noecho();
            refresh();        

            nodelay(stdscr, FALSE);  // Disable non-blocking mod

            while (1)
            {
                if (update) {
                    move(y+1, 0); 
                    clrtoeol();
                    update = 0;
                }

                mvprintw(y+1, 0, ":");
                mvprintw(y+1, 1, "%s", buffer);

                refresh();

                char key = getch();
                if (key == '\n')  {
                    break;
                } 
                else if (key == Key_ESCAPE) {
                    buffer[0] = '\0'; 
                    break;
                } 
                else if (key == KEY_BACKSPACE || key == Key_BACKSPACE)  {
                    if (bufferLen > 0) {
                        buffer[--bufferLen] = '\0'; 
                    }
                    update = 1;
                } 
                else if (bufferLen < sizeof(buffer) - 1)  {
                    buffer[bufferLen++] = key;
                    buffer[bufferLen] = '\0'; 
                }
                
                DumpBranches(buffer);
                refresh();
            }

            nodelay(stdscr, TRUE); 
            curs_set(0);

            clear();
            refresh();        

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
            DumpBranches(NULL);
            refresh();
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
            SetNextCmd(Proc_CMD_SEARCH);
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