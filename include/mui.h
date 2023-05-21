#ifndef MUI_H
#define MUI_H

//MUI works in 3 steps..
//1. First pass, All commands are captured in a command buffer to know the exact ui we need to draw
//2. n passes, All commands are processed (for now this just calcs the panel's coords )
//3. last pass, All commands are rendered to final output


//base is used only for vector and rect types
#include "base.h"

#define MUI_TEXT_SIZE 16
#define MUI_SCROLL_SIZE 16
#define MUI_BUTTON_SIZE_X 90 * 1.3
#define MUI_BUTTON_SIZE_Y 40 * 1.3
#define MUI_MAX_LAYOUTS 64
#define MUI_MAX_COMMANDS 64

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
	mColor scroll_bg_color;
	mColor checkbox_border_color;
}muiStyle;




typedef enum {
	MUI_WIDGET_BUTTON = 1,
	MUI_WIDGET_LABEL,
	MUI_WIDGET_SLIDER,
	MUI_WIDGET_CHECKBOX,
}muiWidgetType;

typedef enum {
	MUI_CMD_PUSH_LAYOUT=1,
	MUI_CMD_POP_LAYOUT,
	MUI_CMD_PUSH_WIDGET,
	MUI_CMD_PUSH_PANEL, //for new panels (add windows? popups? later)
	MUI_CMD_POP_PANEL,
	//MUI_CMD_CLIP???
}muiCommandType;
typedef struct{
	char label[32]; //label e.g for buttons and stuff
	muiWidgetType type;
	void* internal_ptr; //pointer to data for slider/checkbox/etc..
	int slider_min;
	int slider_max;
}muiWidgetData;

//tagged union for mui commands
typedef struct {
	u32 id; //TODO this id should only exist for widgets? no?
	union cmd{
		muiLayoutType layout;
		mRect panel;
		muiWidgetData widget_data;
		
		//for widgets we dont currently need any info? at most a label
	}data;
	muiCommandType type;
}muiCommand;



typedef struct {
	mRect clip_rect;
	void *texture_atlas;

	//should items be controls/widgets????
	i32 hot_item; //item below da cursor
	i32 active_item; //item we are inteeracting with
	f32 text_scale;

	b32 lmb_down;
	b32 lmb_up;

	muiLayout layout_stack[MUI_MAX_LAYOUTS];
	i32 layout_stack_size;


	muiCommand cmd_buf[MUI_MAX_COMMANDS];
	u32 cmd_count;

	muiStyle style;
}muiState;

//These need to be implemented by each app using mui
#include "mTex.h"
#include "mqoi.h"
static inline void mui_draw_char(muiState *mui, char l, mRect dest){
	i32 ppl = 16;
	mRect ltc = {((i32)l % 16) * ppl, ((i32)l / 16) * ppl, MUI_TEXT_SIZE, MUI_TEXT_SIZE};
	mtex_render(mui->texture_atlas, ltc, dest);
}
static inline void mui_input_update(muiState *mui){
	mui->lmb_down = mkey_down(MK_LMB);
	mui->lmb_up = mkey_up(MK_LMB);
}
static inline void mui_draw_rect(muiState *mui, mRect rect, mColor col){
	mrend_draw_rect(rect, col);
}
static inline void mui_load_texture_atlas(muiState *mui){
	//you can assign atlas to constant memory if u need so
	if (mui->texture_atlas == NULL)mui->texture_atlas = (mTex*)malloc(sizeof(mTex));
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
	ASSERT(mtex_create(&(mTexDesc){desc.width,desc.height,format},tp, (mTex*)mui->texture_atlas));
}
////////////////////////////////////////////////////


static inline void mui_style_default(muiStyle *style){
	style->scroll_bg_color = (mColor){0x444444};
	style->checkbox_border_color = (mColor){0x444444};
	style->default_color = (mColor){0x343434};
	style->hot_color = (mColor){0xFF2626};
	style->active_color = (mColor){0xCC0000};
	style->border_color = (mColor){0x585858};
}


static inline void mui_init(muiState *mui){
	MEMSET(mui, 0, sizeof(*mui));
	
	mui_load_texture_atlas(mui);

	//set clip rect equal to screen rect (for now)
	mui->clip_rect = (mRect){0,0,600,400};
	//set text scaling
	mui->text_scale = 1.0f;
	mui->hot_item = 0;
	mui->active_item = 0;

	mui_style_default(&mui->style);
	mui->layout_stack_size = 0;
}



//LAYOUT

static inline muiLayout *mui_layout_top(muiState *mui){
	ASSERT(mui->layout_stack_size > 0);
	return &mui->layout_stack[mui->layout_stack_size - 1];
}

static inline void mui_layout_start_imm(muiState *mui,muiLayoutType type, iv2 start){
	ASSERT(mui->layout_stack_size < MUI_MAX_LAYOUTS);
	muiLayout layout = {0};
	layout.padding = 5;
	layout.type = type;
	layout.start = start;
	mui->layout_stack[mui->layout_stack_size++] = layout;
}

static inline void mui_layout_push_imm(muiState *mui, muiLayoutType type){
	muiLayout *layout = mui_layout_top(mui);
	mui_layout_start_imm(mui, type, (iv2){layout->start.x, layout->start.y});
}


static inline void mui_layout_pop_imm(muiState *mui){
	muiLayout *child_layout = mui_layout_top(mui);
	mui->layout_stack_size--;
	if (mui->layout_stack_size > 0){
		muiLayout *layout = mui_layout_top(mui);
		if (layout->type == MUI_VERTICAL_LAYOUT){
			layout->start.y += child_layout->size.y;
		}else if (layout->type == MUI_HORIZONTAL_LAYOUT){
			layout->start.x += child_layout->size.x;
		}
	}
	ASSERT(mui->layout_stack_size >= 0);
}





//PANEL

static inline void mui_panel_begin_imm(muiState *mui, mRect r){
	mui_layout_start_imm(mui, MUI_VERTICAL_LAYOUT, (iv2){r.x, r.y});
	f32 pad = mui_layout_top(mui)->padding;
	mRect r1 = (mRect){r.x - pad, r.y - pad, r.w + pad*2, r.h + pad*2};
	mui_draw_rect(mui,r1, mui->style.checkbox_border_color);
	mRect r2 = (mRect){r.x - pad/2, r.y - pad/2, r.w + pad, r.h + pad};;
	mui_draw_rect(mui,r2, mui->style.border_color);
}

static inline void mui_panel_end_imm(muiState *mui){
	mui_layout_pop_imm(mui);
}



//CONTROLS

static inline b32 mmouse_isect(mRect r){
	int m_x = minput_get_mouse_pos().x;
	int m_y = minput_get_mouse_pos().y;
	return (m_x >= r.x && m_x <= r.x + r.w && m_y >= r.y && m_y <= r.y + r.h);
}


static inline iv2 mui_get_label_size(muiState *mui, char *label){
	i32 ppc = 16; //pixels per character
	return (iv2){mui->text_scale * ppc * strlen(label), mui->text_scale *ppc};
}

static inline mRect mui_layout_advance_button_imm(muiState *mui){
	mRect rect = {0};
	muiLayout *current_layout = mui_layout_top(mui);
	if (current_layout->type == MUI_HORIZONTAL_LAYOUT){
		rect = (mRect){current_layout->start.x + current_layout->size.x, current_layout->start.y, MUI_BUTTON_SIZE_X, MUI_BUTTON_SIZE_Y};
		current_layout->size.x += MUI_BUTTON_SIZE_X + current_layout->padding;
		current_layout->size.y = MAX(MUI_BUTTON_SIZE_Y + current_layout->padding, current_layout->size.y);
	}else if (current_layout->type == MUI_VERTICAL_LAYOUT){
		rect = (mRect){current_layout->start.x, current_layout->start.y + current_layout->size.y, MUI_BUTTON_SIZE_X, MUI_BUTTON_SIZE_Y};
		current_layout->size.y += MUI_BUTTON_SIZE_Y + current_layout->padding;
		current_layout->size.x = MAX(MUI_BUTTON_SIZE_X + current_layout->padding, current_layout->size.x);
	}
	return rect;
}

static inline b32 mui_button_imm(muiState *mui, u32 id, char *label){
	mRect rect =  mui_layout_advance_button_imm(mui);

	if (mmouse_isect(rect)){
		mui->hot_item = id;
		if (mui->active_item == 0 && mui->lmb_down){
			mui->active_item = id;
		}
	}

	if (mui->hot_item == id){
		if (mui->active_item == id){
			//button hot and active
			mui_draw_rect(mui,rect, mui->style.active_color);
		}else {
			//button hot
			mui_draw_rect(mui,rect, mui->style.hot_color);
		}
	}else {
		mui_draw_rect(mui,rect, mui->style.default_color);
	} 
	iv2 label_size = mui_get_label_size(mui, label);
	//void mui_draw_char(char l, mRect dest);
	i32 ppl = 16;
	iv2 label_pos = (iv2){rect.x - (label_size.x - rect.w)/(f32)2, rect.y + rect.h/2 - (ppl/2)*mui->text_scale};
	for (int i = 0; i < strlen(label); ++i){
		mui_draw_char(mui, label[i], (mRect){label_pos.x + i * ppl * mui->text_scale, label_pos.y,MUI_TEXT_SIZE * mui->text_scale,MUI_TEXT_SIZE* mui->text_scale});
	}


	if (mui->lmb_up && mui->hot_item == id && mui->active_item == id)
		return 1;
	return 0;
}

static inline b32 mui_checkbox_imm(muiState *mui, u32 id, char *label, b32 *onoff){
	mRect rect =  mui_layout_advance_button_imm(mui);
	mui_draw_rect(mui,rect, mui->style.checkbox_border_color);


	if (mmouse_isect(rect)){
		mui->hot_item = id;
		if (mui->active_item == 0 && mui->lmb_down){
			mui->active_item = id;
		}
	}
	if (mui->lmb_up && mui->hot_item == id && mui->active_item == id){
		*onoff = (*onoff) ? 0 : 1;
	}
	int pad = mui_layout_top(mui)->padding/2;
	mRect internal_rect = (mRect){rect.x+pad, rect.y+pad, rect.w-pad*2,rect.h-pad*2};
		mui_draw_rect(mui,internal_rect, mui->style.default_color);

	mRect internal_rect2 = (mRect){internal_rect.x+pad, internal_rect.y+pad, internal_rect.w-pad*2,internal_rect.h-pad*2};
	if (*onoff)
		mui_draw_rect(mui,internal_rect2, mui->style.checkbox_border_color);
	else
		mui_draw_rect(mui,internal_rect2, mui->style.default_color);
	iv2 label_size = mui_get_label_size(mui, label);
	i32 ppl = 16;
	iv2 label_pos = (iv2){rect.x - (label_size.x - rect.w)/(f32)2, rect.y + rect.h/2 - (ppl/2)*mui->text_scale};
	for (int i = 0; i < strlen(label); ++i){
		mui_draw_char(mui, label[i], (mRect){label_pos.x + i * ppl * mui->text_scale, label_pos.y,MUI_TEXT_SIZE * mui->text_scale,MUI_TEXT_SIZE* mui->text_scale});
	}


	if (mui->lmb_up && mui->hot_item == id && mui->active_item == id)
		return 1;
	return 0;
}

static inline b32 mui_slider_imm(muiState *mui, u32 id, char *label, int *val, int min, int max){

	muiLayout *current_layout = mui_layout_top(mui);
	mRect bar_rect = (mRect){current_layout->start.x + current_layout->size.x  + (MUI_BUTTON_SIZE_X-MUI_SCROLL_SIZE) * (((*val)-min) / (f32)(max - min)), current_layout->start.y,MUI_SCROLL_SIZE, MUI_BUTTON_SIZE_Y};
	mRect rect =  mui_layout_advance_button_imm(mui);
	//this should happen for bar_rect
	if (mmouse_isect(bar_rect)){
		mui->hot_item = id;
		if (mui->active_item == 0 && mui->lmb_down){
			mui->active_item = id;
		}
	}

	mui_draw_rect(mui,rect, mui->style.scroll_bg_color);
	if (mui->hot_item == id){
		if (mui->active_item == id){
			//button hot and active
			mui_draw_rect(mui,bar_rect, mui->style.active_color);
		}else {
			//button hot
			mui_draw_rect(mui,bar_rect, mui->style.hot_color);
		}
	}else {
		mui_draw_rect(mui,bar_rect, mui->style.default_color);
	} 
	iv2 label_size = mui_get_label_size(mui, label);
	//void mui_draw_char(char l, mRect dest);
	i32 ppl = 16;
	iv2 label_pos = (iv2){rect.x - (label_size.x - rect.w)/(f32)2, rect.y + rect.h/2 - (ppl/2)*mui->text_scale};
	for (int i = 0; i < strlen(label); ++i){
		mui_draw_char(mui, label[i], (mRect){label_pos.x + i * ppl * mui->text_scale, label_pos.y,MUI_TEXT_SIZE * mui->text_scale,MUI_TEXT_SIZE* mui->text_scale});
	}

	//SCETCHY AF
	if (mui->active_item == id){
		int m_x = minput_get_mouse_pos().x;
		m_x = CLAMP(rect.x, m_x, rect.x + rect.w - MUI_SCROLL_SIZE);
		f32 scroll_percent = (m_x -rect.x) / ((f32)rect.w - MUI_SCROLL_SIZE);

		*val = min + scroll_percent * (max - min);
	}


	return 0;
}
static inline void mui_label_imm(muiState *mui, u32 id, char *label){
	mRect rect =  mui_layout_advance_button_imm(mui);

	iv2 label_size = mui_get_label_size(mui, label);
	i32 ppl = 16;
	iv2 label_pos = (iv2){rect.x - (label_size.x - rect.w)/(f32)2, rect.y + rect.h/2 - (ppl/2)*mui->text_scale};
	for (int i = 0; i < strlen(label); ++i){
		mui_draw_char(mui, label[i], (mRect){label_pos.x + i * ppl * mui->text_scale, label_pos.y,MUI_TEXT_SIZE * mui->text_scale,MUI_TEXT_SIZE* mui->text_scale});
	}
}


static void mui_cmd_push(muiState *mui, muiCommand cmd);

static inline void mui_layout_push(muiState *mui, muiLayoutType type){
	muiCommand c = {0};
	c.type = MUI_CMD_PUSH_LAYOUT;
	c.data.layout = type;
	mui_cmd_push(mui, c);
}

static inline void mui_layout_pop(muiState *mui){
	muiCommand c = {0};
	c.type = MUI_CMD_POP_LAYOUT;
	mui_cmd_push(mui, c);
}

static inline void mui_label(muiState *mui, u32 id, char *label){
	muiCommand c = {0};
	c.id = id;
	c.type = MUI_CMD_PUSH_WIDGET;
	if (label)
		memcpy(c.data.widget_data.label, label, strlen(label));
	c.data.widget_data.type = MUI_WIDGET_LABEL;
	mui_cmd_push(mui, c);
}

static inline b32 mui_button(muiState *mui, u32 id, char *label){
	muiCommand c = {0};
	c.id = id;
	c.type = MUI_CMD_PUSH_WIDGET;
	if (label)
		memcpy(c.data.widget_data.label, label, strlen(label));
	c.data.widget_data.type = MUI_WIDGET_BUTTON;
	mui_cmd_push(mui, c);

	//printf("HOT: %i", mui->hot_item);
	if (mui->lmb_up && mui->hot_item == id && mui->active_item == id)
		return 1;
	return 0;
}

static inline b32 mui_checkbox(muiState *mui, u32 id, char *label, b32 *onoff){
	muiCommand c = {0};
	c.id = id;
	c.type = MUI_CMD_PUSH_WIDGET;
	if (label)
		memcpy(c.data.widget_data.label, label, strlen(label));
	c.data.widget_data.internal_ptr = onoff;
	c.data.widget_data.type = MUI_WIDGET_CHECKBOX;
	mui_cmd_push(mui, c);

	//printf("HOT: %i", mui->hot_item);
	if (mui->lmb_up && mui->hot_item == id && mui->active_item == id)
		return 1;
	return 0;
}

static inline b32 mui_slider(muiState *mui, u32 id, char *label, int *val, int min, int max){
	muiCommand c = {0};
	c.id = id;
	c.type = MUI_CMD_PUSH_WIDGET;
	if (label)
		memcpy(c.data.widget_data.label, label, strlen(label));
	c.data.widget_data.type = MUI_WIDGET_SLIDER;
	c.data.widget_data.internal_ptr = val;
	c.data.widget_data.slider_min = min;
	c.data.widget_data.slider_max = max;
	mui_cmd_push(mui, c);


	if (mui->hot_item == id && mui->active_item == id)
		return 1;
	return 0;
}

static inline void mui_panel_begin(muiState *mui, iv2 sp){
	muiCommand c = {0};
	c.type = MUI_CMD_PUSH_PANEL;
	c.data.panel = (mRect){sp.x,sp.y,0,0};
	mui_cmd_push(mui, c);
}
static inline void mui_panel_end(muiState *mui){
	muiCommand c = {0};
	c.type = MUI_CMD_POP_PANEL;
	mui_cmd_push(mui, c);
}




//CMD BUFF

static inline void mui_cmd_push(muiState *mui, muiCommand cmd){
	ASSERT(mui->cmd_count <= MUI_MAX_COMMANDS);
	mui->cmd_buf[mui->cmd_count++] = cmd;
}

static inline void mui_cmd_clear(muiState *mui){
	mui->cmd_count = 0;
}


static inline void mui_cmd_calc_panel_rects(muiState *mui){
	mRect *current_panel_rect;
	for (u32 i = 0; i < mui->cmd_count; ++i)
	{
		muiCommand c = mui->cmd_buf[i];

		switch (c.type){
			case MUI_CMD_PUSH_PANEL:	
				mui_layout_start_imm(mui, MUI_VERTICAL_LAYOUT, (iv2){mui->cmd_buf[i].data.panel.x, mui->cmd_buf[i].data.panel.y});
				current_panel_rect = &mui->cmd_buf[i].data.panel;
				current_panel_rect->w = 0;
				current_panel_rect->h = 0;
				break;
			case MUI_CMD_POP_PANEL:
				current_panel_rect->w -= mui_layout_top(mui)->padding;
				current_panel_rect->h -= mui_layout_top(mui)->padding;
				mui_panel_end_imm(mui);
				break;
			case MUI_CMD_PUSH_LAYOUT:
				mui_layout_push_imm(mui, c.data.layout);
				break;
			case MUI_CMD_POP_LAYOUT:
				mui_layout_pop_imm(mui);
				break;
			case MUI_CMD_PUSH_WIDGET:
				mRect r = mui_layout_advance_button_imm(mui);
				
				muiLayout *current_layout = mui_layout_top(mui);
				int current_max_width = current_layout->start.x + current_layout->size.x;
				int current_max_height = current_layout->start.y + current_layout->size.y;
				current_panel_rect->w = MAX(current_panel_rect->w, current_max_width - current_panel_rect->x);
				current_panel_rect->h = MAX(current_panel_rect->h, current_max_height - current_panel_rect->y);
				break;
			default:
				break;
		}
	}
}

//renders the current ui captured in the commands, for now this is done 
//in linear fashion, almost immediate style
static inline void mui_cmd_update(muiState *mui){
	mui_cmd_calc_panel_rects(mui);
	for (u32 i = 0; i < mui->cmd_count; ++i)
	{
		muiCommand c = mui->cmd_buf[i];

		switch (c.type){
			case MUI_CMD_PUSH_PANEL:
				mui_panel_begin_imm(mui, c.data.panel);
				break;
			case MUI_CMD_POP_PANEL:
				mui_panel_end_imm(mui);
				break;
			case MUI_CMD_PUSH_LAYOUT:
				mui_layout_push_imm(mui, c.data.layout);
				break;
			case MUI_CMD_POP_LAYOUT:
				mui_layout_pop_imm(mui);
				break;
			case MUI_CMD_PUSH_WIDGET:
				switch(c.data.widget_data.type){
					case MUI_WIDGET_BUTTON:
						mui_button_imm(mui, c.id, c.data.widget_data.label);
						break;
					case MUI_WIDGET_LABEL:
						mui_label_imm(mui, c.id, c.data.widget_data.label);
						break;
					case MUI_WIDGET_CHECKBOX:
						mui_checkbox_imm(mui, c.id, c.data.widget_data.label, c.data.widget_data.internal_ptr);
						break;
					case MUI_WIDGET_SLIDER:
						static int plchldr = 4;
						mui_slider_imm(mui, c.id, c.data.widget_data.label,c.data.widget_data.internal_ptr, c.data.widget_data.slider_min, c.data.widget_data.slider_max);
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
}

static inline void mui_start(muiState *mui){
	mui->cmd_count = 0;
	mui_input_update(mui);
}
static inline void mui_finish(muiState *mui){
	mui->hot_item = 0;
	mui_cmd_update(mui);

	if (mui->lmb_up){
		mui->active_item = 0;
	}else {
		if (mui->active_item == 0)
			mui->active_item = -1;
	}
}
#endif