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
#define MTIME_IMPLEMENTATION
#include "mTime.h"
muiState m;

extern void mg_init(void);
extern void mg_update(f32 dt);
extern void mg_render(void);

void minit(){
	mtime_init();
	ASSERT(mbackend_init());
	mwin_create(&(mWinDesc){100,100,600,600,MWIN_OPT_RESIZABLE | MWIN_OPT_BORDERED}, mwin_get_instance());
	mrend_init(MREND_OPT_ST);

	mui_init(&m);
	mg_init();

}
void mupdate(){
	minput_update();
	mg_update(0);
}
void mrender(){
	mg_render();
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
