#ifndef MUI_H
#define MQUI_H
#include "base.h"
#include "mTex.h"
#include "mqoi.h"

#define MUI_TEXT_SIZE 16
#define MUI_BUTTON_SIZE_X 90
#define MUI_BUTTON_SIZE_Y 60

typedef enum {
	MUI_ALIGN_CENTER,
	MUI_ALIGN_RIGHT,
	MUI_ALIGN_LEFT,
}MUI_ALIGN_OPT;
//this should be for text? inside layout struct?
//firstly, lets implement only align center

typedef struct {
	mColor default_color;
	mColor hot_color;
	mColor active_color;
	mColor border_color;
}muiStyle;


typedef struct {
	mRect clip_rect;
	mTex texture_atlas;



	//should items be controls/widgets????
	i32 hot_item; //item below da cursor
	i32 active_item; //item we are inteeracting with
	f32 text_scale;

	iv2 current_widget_pos;
	mRect current_window_rect;

	muiStyle style;
}muiState;

static muiState mui;

static inline void mui_style_default(muiStyle *style){
	style->default_color = (mColor){0x040404};
	style->hot_color = (mColor){0xFF0000};
	style->active_color = (mColor){0xCC0000};
	style->border_color = (mColor){0x585858};
}


static inline void mui_window_begin(mRect r){
	mui.current_widget_pos = (iv2){r.x, r.y}; //maybe these should be stack based too????
	mui.current_window_rect = r;
	
	mrend_draw_rect(mui.current_window_rect, mui.style.border_color);
}

static inline void mui_window_end(){}

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

	mui_style_default(&mui.style);
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


iv2 mui_get_label_size(char *label){
	i32 ppl = 16; //pixels per letter
	return (iv2){mui.text_scale * ppl * strlen(label), mui.text_scale *ppl};
}


b32 mui_button(u32 id, char *label){
	mRect rect = (mRect){mui.current_widget_pos.x, mui.current_widget_pos.y, MUI_BUTTON_SIZE_X, MUI_BUTTON_SIZE_Y};
	//maybe this += should happen after clipping or sth???
	//mui.current_widget_pos.x += MUI_BUTTON_SIZE_X;
	mui.current_widget_pos.y +=MUI_BUTTON_SIZE_Y;
	if (mmouse_isect(rect)){
		mui.hot_item = id;
		if (mui.active_item == 0 && mkey_down(MK_LMB)){
			mui.active_item = id;
		}
	}

	if (mui.hot_item == id){
		if (mui.active_item == id){
			//button hot and active
			mrend_draw_rect(rect, mui.style.active_color);
		}else {
			//button hot
			mrend_draw_rect(rect, mui.style.hot_color);
		}
	}else {
		mrend_draw_rect(rect, mui.style.default_color);
	} 
	iv2 label_size = mui_get_label_size(label);
	//void mui_draw_char(char l, mRect dest);
	i32 ppl = 16;
	iv2 label_pos = (iv2){rect.x - (label_size.x - rect.w)/(f32)2, rect.y + rect.h/2 - ppl/2};
	for (int i = 0; i < strlen(label); ++i){
		mui_draw_char(label[i], (mRect){label_pos.x + i * ppl, label_pos.y,MUI_TEXT_SIZE,MUI_TEXT_SIZE});
	}

	//layout push / pop
//layout push / pop
//layout push / pop
//layout push / pop
//layout push / pop

	if (mkey_up(MK_LMB) && mui.hot_item == id && mui.active_item == id)
		return 1;
	return 0;
}



#endif