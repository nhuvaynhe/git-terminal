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
Branches searchBranch;

Branches *mybrch = &mybranch; 
Branches *searchBuffer = &searchBranch; 

static MainProc currentProc = Proc_NONE;
static ProcCommand currentCmd = PROC_CMD_NONE;
static UIState IsUpdateUI = eUPDATE;

void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

static void SetNextProc(MainProc proc)
{
    currentProc = proc;
}

static void SetNextCmd(ProcCommand cmd)
{
    currentCmd = cmd;
}

static void NotifyUpdateUI(UIState state)
{
    IsUpdateUI = state;
}

static int SwitchBranch(Branches *branchList)
{
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
    }

    char *selectedBranch = malloc(strlen(branchList->branches[branchList->currentIdx]) + 1);
    if (selectedBranch == NULL) {
        perror("Malloc failed");
        return -1; 
    }

    strcpy(selectedBranch, branchList->branches[branchList->currentIdx]);

    nodelay(stdscr, FALSE);  /* for waiting getch() */

    int rows, cols;
    getmaxyx(stdscr, rows, cols); 

    attron(COLOR_PAIR(1)); // Enable the green color pair
    mvprintw(rows/2, (cols-strlen(selectedBranch)-12)/2, 
             "git checkout %s", selectedBranch);
    attroff(COLOR_PAIR(1)); // Disable the green color pair
    
    refresh();
    
    getch();
    erase();

    nodelay(stdscr, TRUE); 

    free(selectedBranch);

    return 0; 
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
                exit(1);
            }

            remove_spaces(buffer);
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

static void UpdateRowArrow(int highlightIdx, int row)
{
    if (row == highlightIdx) {
        mvprintw(row, 0, "--->"); 
    } 
    else if (row == highlightIdx - 1 || row == highlightIdx + 1) {
        mvprintw(row, 0, "    "); 
    }
}

static void DumpBranches(const char *query)
{
    int idx = 0;

    if (IsUpdateUI & (searchBuffer->total > 0)) {
        for (int i = 0; i < searchBuffer->total; i++) {
            free(searchBuffer->branches[i]);
            searchBuffer->branches[i] = NULL;
        }
        searchBuffer->total = 0;
        searchBuffer->currentIdx = 0;
    }
    
    for (int row = 0; row < mybrch->total; row++) {
        if (query == NULL) {
            mvprintw(row, 5, "%d: %s",  row+1, mybrch->branches[row]);
            UpdateRowArrow(mybrch->currentIdx, row);

        }
        else {
            move(row, 0); 
            clrtoeol();

            if (strstr(mybrch->branches[row], query) != NULL) {
                searchBuffer->branches[idx] = malloc(sizeof(mybrch->branches[row]) + 1);
                if (searchBuffer->branches[idx] == NULL) {
                    printf("Malloc failed.\n");
                    exit(1);
                }

                strcpy(searchBuffer->branches[idx], mybrch->branches[row]);
                searchBuffer->total++;
                idx++;
            }
        }
    }

    if (searchBuffer->total > 0 && IsUpdateUI) {
        for (int row = 0; row < searchBuffer->total; row++) {
            mvprintw(row, 5, "%d: %s", row+1, searchBuffer->branches[row]);
            UpdateRowArrow(searchBuffer->currentIdx, row);
        }
    }

    NotifyUpdateUI(eSTATIC);
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

        case Proc_CMD_ENTER:
        {
            SwitchBranch(&mybranch);
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
                    SwitchBranch(&searchBranch);
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
                    NotifyUpdateUI(eUPDATE);
                } 
                else if (bufferLen < sizeof(buffer) - 1)  {
                    buffer[bufferLen++] = key;
                    buffer[bufferLen] = '\0'; 
                    NotifyUpdateUI(eUPDATE);
                }
                
                DumpBranches(buffer);
                refresh();
            }

            nodelay(stdscr, TRUE); 
            curs_set(0);

            clear();
            refresh();        

            NotifyUpdateUI(eUPDATE);
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
            SetNextCmd(Proc_CMD_ENTER);
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