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

void minit(){
	mtime_init();
	mwin_create(&(mWinDesc){100,100,600,400,MWIN_OPT_RESIZABLE | MWIN_OPT_BORDERED}, mwin_get_instance());
	mrend_init(MREND_OPT_ST);

	mui_init();
}
void mupdate(){
	minput_update();
	//printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	v2 mp = minput_get_mouse_pos();

	mui_window_begin((mRect){100,100,350,350});

	mui_layout_push(MUI_HORIZONTAL_LAYOUT);
	if (mui_button(__LINE__, "PRIDE"))printf("PRIDE_0 CLICKED!\n");
	if (mui_button(__LINE__,"NEVER"))printf("NEVER_0 CLICKED!\n");
	if (mui_button(__LINE__, "DIES"))printf("DIES_0 CLICKED!\n");
	mui_layout_pop();
	
	mui_layout_push(MUI_HORIZONTAL_LAYOUT);
	if (mui_button(__LINE__, "PRIDE"))printf("PRIDE_1 CLICKED!\n");
	if (mui_button(__LINE__,"NEVER"))printf("NEVER_1 CLICKED!\n");
	if (mui_button(__LINE__, "DIES"))printf("DIES_1 CLICKED!\n");
	mui_layout_pop();
	
	mui_layout_push(MUI_HORIZONTAL_LAYOUT);
	if (mui_button(__LINE__, "PRIDE"))printf("PRIDE_2 CLICKED!\n");
	if (mui_button(__LINE__,"NEVER"))printf("NEVER_2 CLICKED!\n");
	if (mui_button(__LINE__, "DIES"))printf("DIES_2 CLICKED!\n");
	mui_layout_pop();

	mui_window_end();


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
		mui_start();
		mrender();
		mui_finish();
		MPROFILER_END()
		//printf("Execution of tag [%s] : [%f] ms and [%lu] cycles!\n",global_profiler.tags[0].name,global_profiler.tags[0].samples[0],global_profiler.tags[0].cycles[0]); 
	}

	u64 end_timestamp = mtime_now();

	printf("Execution lasted: [%lf] seconds!", mtime_sec(mtime_diff(end_timestamp, start_timestamp)));

	mwin_destroy(mwin_get_instance());
}
