#ifndef __KEYPROC_H__
#define __KEYPROC_H__

#include "branches.h"

#define ARROW_UP       KEY_UP
#define ARROW_DOWN     KEY_DOWN

void ProcessUp(Branches *mybranch);
void ProcessDown(Branches *mybranch);
void ProcessSearch(Branches *mybranch, Branches *searchBranch);
int ProcessEnter(Branches *mybranch);

void KeyHandler();

#endif