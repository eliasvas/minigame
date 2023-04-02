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
	if (mmouse_isect((mRect){100,100,100,100})){
		printf("INTERSECTION!\n");
	}
	//printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	v2 mp = minput_get_mouse_pos();

	mrend_draw_rect((mRect){100,100,100,100}, (mColor){0xFF0000});

	mui_draw_char('P', (mRect){0,0,100,100});
	mui_draw_char('R', (mRect){100,0,100,100});
	mui_draw_char('I', (mRect){200,0,100,100});
	mui_draw_char('D', (mRect){300,0,100,100});
	mui_draw_char('E', (mRect){400,0,100,100});


	mui_draw_char('N', (mRect){0,100,100,100});
	mui_draw_char('E', (mRect){100,100,100,100});
	mui_draw_char('V', (mRect){200,100,100,100});
	mui_draw_char('E', (mRect){300,100,100,100});
	mui_draw_char('R', (mRect){400,100,100,100});

	mui_draw_char('D', (mRect){0,200,200,200});
	mui_draw_char('I', (mRect){200,200,200,200});
	mui_draw_char('E', (mRect){400,200,200,200});
	
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
		mrender();
		MPROFILER_END()
		printf("Execution of tag [%s] : [%f] ms and [%lu] cycles!\n",global_profiler.tags[0].name,global_profiler.tags[0].samples[0],global_profiler.tags[0].cycles[0]); 
	}

	u64 end_timestamp = mtime_now();

	printf("Execution lasted: [%lf] seconds!", mtime_sec(mtime_diff(end_timestamp, start_timestamp)));

	mwin_destroy(mwin_get_instance());
}
