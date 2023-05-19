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
#define MTIME_IMPLEMENTATION
#include "mTime.h"
#include "mui.h"
static mTex t;
static mSound s;
static b32 play_state = 0;

static int val = 6;

static muiState m;

void minit(){
	mtime_init();
	ASSERT(mbackend_init());
	mwin_create(&(mWinDesc){100,100,600,600,MWIN_OPT_RESIZABLE | MWIN_OPT_BORDERED}, mwin_get_instance());
	mrend_init(MREND_OPT_ST);
	msound_load(&(mSoundDesc){"../assets/sample.ogg"}, &s);

	mui_init(&m);
}
void mupdate(){
	minput_update();
}
void mrender(){
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


	mrend_clear();
}

extern mProfiler global_profiler;

int main(int argc, char** args) {
	minit();

	u64 start_timestamp = mtime_now();

	while (1){
		MPROFILER_START("update");
		mupdate();
		if (mkey_down(MK_A))
			exit(43);
		mui_start(&m);
		mrender();
		mui_finish(&m);
		MPROFILER_END()
		//printf("Execution of tag [%s] : [%f] ms and [%lu] cycles!\n",global_profiler.tags[0].name,global_profiler.tags[0].samples[0],global_profiler.tags[0].cycles[0]); 
	}

	u64 end_timestamp = mtime_now();

	printf("Execution lasted: [%lf] seconds!", mtime_sec(mtime_diff(end_timestamp, start_timestamp)));

	mwin_destroy(mwin_get_instance());
}
