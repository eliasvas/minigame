#ifndef MTEX_H
#define MTEX_H
#include "base.h"

typedef enum {
    MTEX_FORMAT_RGBA8U = (1 << 0),
    MTEX_FORMAT_RGBA32F = (1 << 1),
}MTEX_FORMAT;

typedef struct {
    char filename[64];
    u32 width, height; //width and height (in pixels)
    MTEX_FORMAT format;
}mTexDesc;

typedef struct {
    mTexDesc desc;    
    //specific to each API implementing textures :)
    void *internal_state; 
} mTex;

M_RESULT mtex_create(mTexDesc *desc, mTex *tex);
M_RESULT mtex_destroy(mTex *tex);
M_RESULT mtex_render(mTex *tex, mRect sub_area, mRect render_area);

#endif