#ifndef MUI_H
#define MQUI_H
#include "base.h"
#include "mTex.h"
#include "mqoi.h"

#define MUI_TEXT_SIZE 16



typedef struct {
	mRect clip_rect;
	mTex texture_atlas;
	f32 text_scale;
}muiState;

static muiState mui;

static inline void mui_init(void){
	MEMSET(&mui, 0, sizeof(mui));
	
	//Load the texture atlas
	mqoiDesc desc = {0};
	u8 *tp = mqoi_load("../assets/atlas.qoi", &desc);
	MTEX_FORMAT format;
	if (desc.colorspace){
		if (desc.channels == 4)format = MTEX_FORMAT_RGBA8U;
		else format = MTEX_FORMAT_RGB8U;
	}else {
		if (desc.channels == 4)format = MTEX_FORMAT_RGBA8S;
		else format = MTEX_FORMAT_RGB8S;
	}
	ASSERT(mtex_create(&(mTexDesc){desc.width,desc.height,format},tp, &mui.texture_atlas));

	//set clip rect equal to screen rect (for now)
	mui.clip_rect = (mRect){0,0,600,400};
	//set text scaling
	mui.text_scale = 1.0f;
}


//M_RESULT mtex_render(mTex *tex, mRect tex_coords, mRect rect);

static inline void mui_draw_char(char l, mRect dest){
	i32 ppl = 16;
	mRect ltc = {((i32)l % 16) * ppl, ((i32)l / 16) * ppl, MUI_TEXT_SIZE * mui.text_scale, MUI_TEXT_SIZE * mui.text_scale};
	mtex_render(&mui.texture_atlas, ltc, dest);
}


#endif