#ifndef MREND_H
#define MREND_H
#include "base.h"
#include "mTex.h"
//SHOULD RENDERER DO CLIPPING ? SOHULD HE HAVE A CLIP RECT OR STH???
//MAYBE A LAST CLIP SHOULD BE DONE AGAINST SCREEN????????? (SLOW)



//TODO: make a command list for all different commands
//and draw at end of frame..

typedef enum {
    MREND_OPT_ST = (1 << 0),
    MREND_OPT_MT = (1 << 1),
    MREND_OPT_COUNT
}mRendOptions;

typedef union {
    u32 col;
    struct {u8 r;u8 g; u8 b;};
    u8 elem[3];
}mColor;

typedef struct {
    mRendOptions opt;
    u32 line_size;
}mRend;

static mRend rend; 

//maybe also implemented by SDL_backend??
static inline void mrend_init(mRendOptions opt){
    rend.opt = opt;
}

void mrend_clear();
void mrend_draw_rect(mRect rect, mColor col);
void mrend_draw_tex(mTex *tex, mRect tex_coords, mRect rect);
#endif