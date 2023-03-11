#include "base.h"
#include "mWin.h"
#include "mInput.h"

mWin window;
int main(int argc, char** args) {
	mWinDesc wd = {0};
	wd.x = 100;
	wd.y = 100;
	wd.width = 600;
	wd.height = 400;
	wd.opt |= MWIN_OPT_RESIZABLE;

	mwin_create(&wd, &window);
	
	while (1){
		minput_update();
	}

	mwin_destroy(&window);
}
