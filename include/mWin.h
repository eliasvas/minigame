#ifndef MWINDOW_H
#define MWINDOW_H
#include "base.h"



typedef enum {
    MWIN_OPT_RESIZABLE =  (1 << 0),
    MWIN_OPT_FULLSCREEN = (1 << 1),
    MWIN_OPT_HIDDEN =     (1 << 2),
}MWIN_OPT;

typedef struct {
    u32 width, height; //width and height (in pixels)
    u32 x, y; //start pos
    MWIN_OPT opt;
}mWinDesc;

typedef struct {
    mWinDesc desc;    
    //specific to each API implementing windows :)
    void *internal_state; 
} mWin;

M_RESULT mwin_create(mWinDesc *desc, mWin *win);
M_RESULT mwin_destroy(mWin *win);

#endif