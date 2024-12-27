typedef enum MainProc {
    Proc_UPDATE_UI,
    Proc_PROCESS,
    Proc_SEARCH,

    Proc_NONE,
} MainProc;

typedef enum ProcCommand {
    Proc_CMD_UP,
    Proc_CMD_DOWN,

    PROC_CMD_NONE,
} ProcCommand;

typedef enum KeyInput {
    Key_UP      = 'j',
    Key_DOWN    = 'k',
    Key_SELECT  = 13,
    Key_SEARCH  = '/',
    Key_QUIT    = 'q',

    Key_NONE,
} KeyInput;
