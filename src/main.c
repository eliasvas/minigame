#include "base.h"
#include "mWin.h"
#include "mInput.h"
#include "mTex.h"
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
	printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	mtex_render(&t, (mRect){0,0,200,200}, (mRect){100,100,300,200});
}

int main(int argc, char** args) {
	minit();

	
	while (1){
		mupdate();
		mrender();
	}

	mwin_destroy(mwin_get_instance());
}
