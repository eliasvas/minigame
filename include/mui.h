#ifndef MUI_H
#define MQUI_H
#include "base.h"
#include "mTex.h"
#include "mqoi.h"

#define MUI_TEXT_SIZE 16
#define MUI_BUTTON_SIZE_X 90
#define MUI_BUTTON_SIZE_Y 60
#define MUI_MAX_LAYOUTS 64

typedef enum {
	MUI_ALIGN_CENTER,
	MUI_ALIGN_RIGHT,
	MUI_ALIGN_LEFT,
}MUI_ALIGN_OPT;
//this should be for text? inside layout struct?
//firstly, lets implement only align center

typedef enum{
	MUI_HORIZONTAL_LAYOUT,
	MUI_VERTICAL_LAYOUT,
}muiLayoutType;

typedef struct{
	iv2 start;
	iv2 size;
	i32 padding;
	muiLayoutType type;
}muiLayout;


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

	//iv2 current_widget_pos;
	//mRect current_window_rect;

	muiLayout layout_stack[MUI_MAX_LAYOUTS];
	i32 layout_stack_size;

	muiStyle style;
}muiState;

static muiState mui;

static inline void mui_style_default(muiStyle *style){
	style->default_color = (mColor){0x343434};
	style->hot_color = (mColor){0xFF2626};
	style->active_color = (mColor){0xCC0000};
	style->border_color = (mColor){0x585858};
}


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
	mui.layout_stack_size = 0;
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

static inline muiLayout *mui_layout_top(){
	ASSERT(mui.layout_stack_size > 0);
	return &mui.layout_stack[mui.layout_stack_size - 1];
}

static inline void mui_layout_start(muiLayoutType type, iv2 start){
	ASSERT(mui.layout_stack_size < MUI_MAX_LAYOUTS);
	muiLayout layout = {0};
	layout.padding = 5;
	layout.type = type;
	layout.start = start;
	mui.layout_stack[mui.layout_stack_size++] = layout;
}

static inline void mui_layout_push(muiLayoutType type){
	muiLayout *layout = mui_layout_top();
	mui_layout_start(type, (iv2){layout->start.x, layout->start.y});
}


static inline void mui_layout_pop(void){
	muiLayout *child_layout = mui_layout_top();
	mui.layout_stack_size--;
	if (mui.layout_stack_size > 0){
		muiLayout *layout = mui_layout_top();
		if (layout->type == MUI_VERTICAL_LAYOUT){
			layout->start.y += child_layout->size.y;
		}else if (layout->type == MUI_HORIZONTAL_LAYOUT){
			layout->start.x += child_layout->size.x;
		}
	}
	ASSERT(mui.layout_stack_size >= 0);
}


static inline void mui_window_begin(mRect r){
	mui_layout_start(MUI_VERTICAL_LAYOUT, (iv2){r.x, r.y});
	
	mrend_draw_rect(r, mui.style.border_color);
}

static inline void mui_window_end(){
	mui_layout_pop();
}


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


	muiLayout *current_layout = mui_layout_top();
	mRect rect;

	if (current_layout->type == MUI_HORIZONTAL_LAYOUT){
		rect = (mRect){current_layout->start.x + current_layout->size.x, current_layout->start.y, MUI_BUTTON_SIZE_X, MUI_BUTTON_SIZE_Y};
		current_layout->size.x += MUI_BUTTON_SIZE_X + current_layout->padding;
		current_layout->size.y = MAX(MUI_BUTTON_SIZE_Y + current_layout->padding, current_layout->size.y);
	}else if (current_layout->type == MUI_VERTICAL_LAYOUT){
		rect = (mRect){current_layout->start.x, current_layout->start.y + current_layout->size.y, MUI_BUTTON_SIZE_X, MUI_BUTTON_SIZE_Y};
		current_layout->size.y += MUI_BUTTON_SIZE_Y + current_layout->padding;
		current_layout->size.x = MAX(MUI_BUTTON_SIZE_X + current_layout->padding, current_layout->size.x);
	}

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


	if (mkey_up(MK_LMB) && mui.hot_item == id && mui.active_item == id)
		return 1;
	return 0;
}



#endif