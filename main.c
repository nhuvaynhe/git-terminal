#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <ncurses/ncurses.h>

#include "enum.h"

#define MAX_ITEMS   100

typedef struct Branches {
    char *branches[MAX_ITEMS];
    int total;
    int currentIdx;
} Branches;

Branches mybranch;
Branches *mybrch = &mybranch; 

static MainProc currentProc = Proc_NONE;
static ProcCommand currentCmd = PROC_CMD_NONE;

static void ClearScreen()
{
    system("cls");
}

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
    for (int i = 0; i < mybrch->total; i++) {
        if (i == mybrch->currentIdx) {
            printf("-->  %d: %s\n", i + 1, mybrch->branches[i]);
        }
        else {
            printf("     %d: %s\n", i + 1, mybrch->branches[i]);
        }
    }
    Sleep(100);
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
            ClearScreen();
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
    char key = getch();
    switch (key)
    {
        case Key_UP:
        {
            SetNextCmd(Proc_CMD_UP);
        } break;

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

DWORD WINAPI ThreadFunc(void* data) {
    while (1) {
        KeyHandler(); 
    }
    return 0;
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

    HANDLE thread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
    if (thread == NULL) {
        printf("Create thread failed");
        exit(1);
    }

    while(1)
    {
        MainProcess();
    }
}