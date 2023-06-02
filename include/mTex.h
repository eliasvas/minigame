#ifndef MTEX_H
#define MTEX_H
#include "base.h"

typedef enum {
    MTEX_FORMAT_RGBA8U = (1 << 0),
    MTEX_FORMAT_RGB8U =  (1 << 1),
    MTEX_FORMAT_RGBA8S = (1 << 2),
    MTEX_FORMAT_RGB8S =  (1 << 3),
    MTEX_FORMAT_RGBA32F = (1 << 4),
}MTEX_FORMAT;

typedef struct {
    u32 width, height;
    MTEX_FORMAT format;
}mTexDesc;

typedef struct {
    mTexDesc desc;    
    //specific to each API implementing textures :)
    void *internal_state; 
} mTex;

//clips a textured* quad against the dst_rect so that only visible parts will be shown
static inline M_RESULT mtex_clip(mRect *tex_coords, mRect *rect, mRect dst_rect){
    mRect tex_quad = (mRect){tex_coords->x,tex_coords->y,tex_coords->w + tex_coords->x, tex_coords->h +tex_coords->y};
    mRect rect_quad = (mRect){rect->x,rect->y,rect->w + rect->x, rect->h +rect->y};


    int rect_w = MIN(dst_rect.w+dst_rect.x, rect_quad.w);
    f32 percent_w = (rect_quad.w- rect_w) / ((f32)rect_quad.w - rect_quad.x);
    rect_quad.w = rect_w;
    tex_quad.w -= (i32)(tex_quad.w * percent_w);

    int rect_h = MIN(dst_rect.h+dst_rect.y, rect_quad.h);
    f32 percent_h = (rect_quad.h- rect_h) / ((f32)rect_quad.h - rect_quad.y);
    rect_quad.h = rect_h;
    tex_quad.h -= (i32)(tex_quad.h * percent_h);


    int rect_x = MAX(dst_rect.x, rect_quad.x);
    f32 percent_x = (rect_x - rect_quad.x) / ((f32)rect_quad.w - rect_quad.x);
    rect_quad.x = rect_x;
    tex_quad.x += (tex_quad.w - tex_quad.x) * percent_x;

    int rect_y = MAX(dst_rect.y, rect_quad.y);
    f32 percent_y = (rect_y - rect_quad.y) / ((f32)rect_quad.h - rect_quad.y);
    rect_quad.y = rect_y;
    tex_quad.y += (tex_quad.h - tex_quad.y) * percent_y;


    *tex_coords = (mRect){tex_quad.x, tex_quad.y, tex_quad.w - tex_quad.x, tex_quad.h - tex_quad.y};
    *rect = (mRect){rect_quad.x, rect_quad.y, rect_quad.w - rect_quad.x, rect_quad.h - rect_quad.y};
    

    return M_OK;
}

M_RESULT mtex_create(mTexDesc *desc, void *tex_data, mTex *tex);
M_RESULT mtex_destroy(mTex *tex);
M_RESULT mtex_render(mTex *tex, mRect tex_coords, mRect rect);

#endif