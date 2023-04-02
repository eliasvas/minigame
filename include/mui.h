#ifndef MUI_H
#define MQUI_H
#include "base.h"
#include "mTex.h"
#include "mqoi.h"

#define MUI_TEXT_SIZE 16



typedef struct {
	mRect clip_rect;
	mTex texture_atlas;



	//should items be controls/widgets????
	i32 hot_item; //item below da cursor
	i32 active_item; //item we are inteeracting with
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
	mui.hot_item = 0;
	mui.active_item = 0;
}

static inline void mui_start(void){
	mui.hot_item = 0;
}
static inline void mui_finish(void){

	if (mkey_up(MK_LMB)){
		mui.active_item = 0;
	}else {
		if (mui.active_item == 0)
			mui.active_item = -1;
	}
}


//M_RESULT mtex_render(mTex *tex, mRect tex_coords, mRect rect);

static inline void mui_draw_char(char l, mRect dest){
	i32 ppl = 16;
	mRect ltc = {((i32)l % 16) * ppl, ((i32)l / 16) * ppl, MUI_TEXT_SIZE * mui.text_scale, MUI_TEXT_SIZE * mui.text_scale};
	mtex_render(&mui.texture_atlas, ltc, dest);
}


b32 mmouse_isect(mRect r){
	int m_x = minput_get_mouse_pos().x;
	int m_y = minput_get_mouse_pos().y;
	return (m_x >= r.x && m_x <= r.x + r.w && m_y >= r.y && m_y <= r.y + r.h);
}


#define MUI_BUTTON_COLOR (mColor){0x040404}
#define MUI_BUTTON_COLOR_ACTIVE (mColor){0xFF0000}
#define MUI_BUTTON_COLOR_HOT (mColor){0xCC0000}
b32 mui_button(u32 id, mRect rect){
	if (mmouse_isect(rect)){
		mui.hot_item = id;
		if (mui.active_item == 0 && mkey_down(MK_LMB)){
			mui.active_item = id;
		}
	}

	if (mui.hot_item == id){
		if (mui.active_item == id){
			//button hot and active
			mrend_draw_rect(rect, MUI_BUTTON_COLOR_ACTIVE);
		}else {
			//button hot
			mrend_draw_rect(rect, MUI_BUTTON_COLOR_HOT);
		}
	}else {
		mrend_draw_rect(rect, MUI_BUTTON_COLOR);
	}

	if (mkey_up(MK_LMB) && mui.hot_item == id && mui.active_item == id)
		return 1;
	return 0;
}



#endif