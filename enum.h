#ifndef __ENUM_H__
#define __ENUM_H__

typedef enum {
    eFALSE,
    eTRUE,
} enumbool;

typedef enum {
    eSTATIC,
    eUPDATE,
} UIState;

typedef enum MainProc {
    Proc_UPDATE_UI,
    Proc_PROCESS,
    Proc_SEARCH,

    Proc_NONE,
} MainProc;

typedef enum ProcCommand {
    Proc_CMD_UP,
    Proc_CMD_DOWN,
    Proc_CMD_ENTER,
    Proc_CMD_SEARCH,

    PROC_CMD_NONE,
} ProcCommand;

typedef enum KeyInput {
    Key_UP      = 'j',
    Key_DOWN    = 'k',
    Key_SELECT  = '\n',
    Key_SEARCH  = '/',
    Key_QUIT    = 'q',
    Key_ESCAPE  = 27,
    Key_BACKSPACE = 7,

    Key_NONE,
} KeyInput;

#endif