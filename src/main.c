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

	mui_window_begin((mRect){100,100,250,250});
	//mrend_draw_rect((mRect){100,100,100,100}, (mColor){0xFF0000});
	if (mui_button(__LINE__, "PRIDE"))printf("BUTTON CLICKED!\n");
	mui_window_end((mRect){100,100,400,400});

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
