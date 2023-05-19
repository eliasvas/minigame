#include "base.h"
#include "math.h"
#include "mWin.h"
#include "mInput.h"
#include "mRend.h"
#include "mTex.h"
#include "mqoi.h"
#include "mSound.h"
#include "mAlloc.h"
#include "mProfiler.h"
#include "mui.h"


extern muiState m;


static mTex t;
static mSound s;
static b32 play_state = 0;
static int val = 6;

void mg_init(void){
	msound_load(&(mSoundDesc){"../assets/sample.ogg"}, &s);
}
void mg_update(f32 dt){

}
void mg_render(void){
	v2 mp = minput_get_mouse_pos();
	if (!play_state){
		mui_panel_begin(&m, (iv2){100,100});
		mui_layout_push(&m, MUI_HORIZONTAL_LAYOUT);
		if (mui_button(&m,__LINE__, "PRIDE"))printf("PRIDE_0 CLICKED!\n");
		if (mui_button(&m,__LINE__,"NEVER"))printf("NEVER_0 CLICKED!\n");
		if (mui_button(&m,__LINE__, "DIES")){printf("DIES_0 CLICKED!\n");msound_play(&s);}
		mui_layout_pop(&m);

		mui_layout_push(&m,MUI_HORIZONTAL_LAYOUT);
		if (mui_slider(&m,__LINE__, "TEXT", &val, 5, 20)){
			m.text_scale = (val / (f32)20)*1.3;
		}
		static int val2 = 7;
		mui_slider(&m,__LINE__, "SLIDER", &val2, 0, 20);
		if (mui_checkbox(&m,__LINE__, "PLAY", &play_state))printf("BUT CLICKED!\n");
		mui_layout_pop(&m);
		mui_layout_push(&m,MUI_HORIZONTAL_LAYOUT);
		char text[256] = {0};
		sprintf(text, "HOT:%i", m.hot_item);
		mui_label(&m, __LINE__, text);
		sprintf(text, "ACT:%i", m.active_item);
		mui_label(&m, __LINE__, text);
		sprintf(text, "VAL:%i", val);
		mui_label(&m, __LINE__, text);
		mui_layout_pop(&m);
		mui_panel_end(&m);
	}
}