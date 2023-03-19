#include "base.h"
#include "mWin.h"
#include "mInput.h"
#include "mTex.h"
#include "mAlloc.h"
#include "mProfiler.h"
#define MTIME_IMPLEMENTATION
#include "mTime.h"
static mTex t;

void minit(){
	mtime_init();
	mWinDesc wd = {100,100,600,400,MWIN_OPT_RESIZABLE};
	mwin_create(&wd, mwin_get_instance());

	mTexDesc td = {"../assets/image.bmp", 200,200,MTEX_FORMAT_RGBA8U};
    mtex_create(&td, &t);

}
void mupdate(){
	minput_update();
	//printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	mtex_render(&t, (mRect){0,0,200,200}, (mRect){100,100,300,200});
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
