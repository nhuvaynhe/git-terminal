#include <stdio.h>


#include "proc.h"
#include "keyproc.h"

static MainProc currentProc = Proc_NONE;
static UIState IsUpdateUI = eUPDATE;
static ProcCommand currentCmd = PROC_CMD_NONE;

extern Branches *mybrch;         
extern Branches *searchBuffer;  

void SetNextProc(MainProc proc)
{
    currentProc = proc;
}

void SetNextCmd(ProcCommand cmd)
{
    currentCmd = cmd;
}

void NotifyUpdateUI(UIState state)
{
    IsUpdateUI = state;
}


static void KeyProcess()
{
    switch (currentCmd)
    {
        case Proc_CMD_DOWN:
        {
            ProcessDown(mybrch);

            SetNextCmd(PROC_CMD_NONE);
            SetNextProc(Proc_UPDATE_UI);
        } break;
        
        case Proc_CMD_UP:
        {
            ProcessUp(mybrch);

            SetNextCmd(PROC_CMD_NONE);
            SetNextProc(Proc_UPDATE_UI);
        } break;

        case Proc_CMD_ENTER:
        {
            ProcessEnter(mybrch);
            
            SetNextCmd(PROC_CMD_NONE);
            SetNextProc(Proc_UPDATE_UI);
        } break;

        case Proc_CMD_SEARCH:
        {
            ProcessSearch(mybrch, searchBuffer);
        } break;

        default:
        {
            ;
        }
    }
}

void MainProcess()
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