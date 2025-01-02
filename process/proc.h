#ifndef __PROC_H__
#define __PROC_H__

#include "enum.h"

void SetNextProc(MainProc proc);
void SetNextCmd(ProcCommand cmd);
void NotifyUpdateUI(UIState state);

void MainProcess();

#endif