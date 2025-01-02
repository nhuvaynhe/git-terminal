#include "keyproc.h"
#include "branches.h"
#include "enum.h"
#include "proc.h"

#include <ncurses/ncurses.h>

void ProcessUp(Branches *mybranch) {
    if (mybranch->currentIdx > 0) {
        mybranch->currentIdx--;
    }

    SetNextCmd(PROC_CMD_NONE);
    SetNextProc(Proc_UPDATE_UI);
}

void ProcessDown(Branches *mybranch) {
    if (mybranch->currentIdx < (mybranch->total - 1)) {
        mybranch->currentIdx++;
    }

    SetNextCmd(PROC_CMD_NONE);
    SetNextProc(Proc_UPDATE_UI);
}

void ProcessSearch(Branches *mybranch, Branches *searchBranch)
{
    /* TODO: Move to a proc file, to handling key up & down */
    char buffer[100] = {0x00};
    int bufferLen = 0;
    int y = mybranch->total;
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
            ProcessEnter(searchBranch);
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
}


int ProcessEnter(Branches *mybranch)
{
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
    }

    char *selectedBranch = malloc(strlen(mybranch->branches[mybranch->currentIdx]) + 1);
    if (selectedBranch == NULL) {
        perror("Malloc failed");
        return -1; 
    }

    strcpy(selectedBranch, mybranch->branches[mybranch->currentIdx]);

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



void KeyHandler()
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