#include "base.h"
#include "math.h"
#include "mWin.h"
#include "mInput.h"
#include "mTex.h"
#include "mqoi.h"
#include "mAlloc.h"
#include "mProfiler.h"
#define MTIME_IMPLEMENTATION
#include "mTime.h"
static mTex t;

void minit(){
	mtime_init();
	mwin_create(&(mWinDesc){100,100,600,400,MWIN_OPT_RESIZABLE | MWIN_OPT_BORDERED}, mwin_get_instance());
    
	mqoiDesc desc;
	u8 *tp = mqoi_load("../assets/testcard.qoi", &desc);
	MTEX_FORMAT format;
	if (desc.colorspace){
		if (desc.channels == 4)format = MTEX_FORMAT_RGBA8U;
		else format = MTEX_FORMAT_RGB8U;
	}else {
		if (desc.channels == 4)format = MTEX_FORMAT_RGBA8S;
		else format = MTEX_FORMAT_RGB8S;
	}
	mtex_create(&(mTexDesc){desc.width,desc.height,format},tp, &t);
}
void mupdate(){
	minput_update();
	//printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	v2 mp = minput_get_mouse_pos();
	mtex_render(&t, (mRect){0,0,t.desc.width,t.desc.height}, (mRect){mp.x - 50,mp.y - 50,100,100});
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
		//printf("Execution of tag [%s] : [%f] ms and [%lu] cycles!\n",global_profiler.tags[0].name,global_profiler.tags[0].samples[0],global_profiler.tags[0].cycles[0]); 
	}

	u64 end_timestamp = mtime_now();

	printf("Execution lasted: [%lf] seconds!", mtime_sec(mtime_diff(end_timestamp, start_timestamp)));

	mwin_destroy(mwin_get_instance());
}
