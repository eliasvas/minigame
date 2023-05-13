#include "base.h"
#include "math.h"
#include "mWin.h"
#include "mInput.h"
#include "mRend.h"
#include "mTex.h"
#include "mqoi.h"
#include "mAlloc.h"
#include "mProfiler.h"
#define MTIME_IMPLEMENTATION
#include "mTime.h"
#include "mui.h"
static mTex t;

static muiState m;


void minit(){
	mtime_init();
	mwin_create(&(mWinDesc){100,100,600,400,MWIN_OPT_RESIZABLE | MWIN_OPT_BORDERED}, mwin_get_instance());
	mrend_init(MREND_OPT_ST);

	mui_init(&m);
}
void mupdate(){
	minput_update();
	//printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	v2 mp = minput_get_mouse_pos();

	mui_panel_begin(&m, (mRect){100,100,350,350});

	mui_layout_push(&m, MUI_HORIZONTAL_LAYOUT);
	if (mui_button(&m,__LINE__, "PRIDE"))printf("PRIDE_0 CLICKED!\n");
	if (mui_button(&m,__LINE__,"NEVER"))printf("NEVER_0 CLICKED!\n");
	if (mui_button(&m,__LINE__, "DIES"))printf("DIES_0 CLICKED!\n");
	mui_layout_pop(&m);
	
	
	mui_layout_push(&m,MUI_HORIZONTAL_LAYOUT);
	if (mui_button(&m,__LINE__, "PRIDE"))printf("PRIDE_2 CLICKED!\n");
	if (mui_button(&m,__LINE__,"NEVER"))printf("NEVER_2 CLICKED!\n");
	if (mui_button(&m,__LINE__, "DIES"))printf("DIES_2 CLICKED!\n");
	mui_layout_pop(&m);

	mui_layout_push(&m,MUI_HORIZONTAL_LAYOUT);
	static int val = 6;
	mui_scrollbar(&m,__LINE__, "SCROLL", &val, 3, 15);
	char text[256] = {0};
	sprintf(text, "HOT:%i", m.hot_item);
	mui_label(&m, __LINE__, text);
	sprintf(text, "ACT:%i", m.active_item);
	mui_label(&m, __LINE__, text);
	sprintf(text, "VAL:%i", val);
	mui_label(&m, __LINE__, text);
	mui_layout_pop(&m);

	mui_panel_end(&m);


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
