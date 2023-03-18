#include "base.h"
#include "mWin.h"
#include "mInput.h"
#include "mTex.h"
#include "mAlloc.h"
#define MTIME_IMPLEMENTATION
#include "mTime.h"
static mTex t;

typedef struct Node {
	int x;
	struct Node *prev;
	struct Node *next;
}Node;

void minit(){
	mtime_init();
	mWinDesc wd = {100,100,600,400,MWIN_OPT_RESIZABLE};
	mwin_create(&wd, mwin_get_instance());

	mTexDesc td = {"../assets/image.bmp", 200,200,MTEX_FORMAT_RGBA8U};
    mtex_create(&td, &t);

	void *node_mem = malloc(sizeof(Node)*10);
	mArena arena = {0};
	marena_init(&arena, node_mem, sizeof(Node)*10);
	Node *nodes = marena_alloc(&arena, sizeof(Node)*5);
	nodes = marena_resize(&arena, nodes, sizeof(Node)*5, sizeof(Node)*10);
	for (int i = 0; i < 10; ++i){
		nodes[i].x = i;
	}
	Node *first = 0;
	Node *last = 0;
	for (int i = 0; i < 5; ++i){
		dll_push_back(first, last, &nodes[i]);
	}
	int sum = 0;
	for (Node *node = first; node != 0; node = node->next){
		ASSERT(sum++ == node->x);
	}

}
void mupdate(){
	minput_update();
	//printf("Current time: [%f] seconds\n", mtime_sec(mtime_now()));
}
void mrender(){
	mtex_render(&t, (mRect){0,0,200,200}, (mRect){100,100,300,200});
}

int main(int argc, char** args) {
	minit();

	u64 start_timestamp = mtime_now();

	while (1){
		mupdate();
		if (mkey_down(MK_A))
			exit(43);
		mrender();
	}

	u64 end_timestamp = mtime_now();

	printf("Execution lasted: [%lf] seconds!", mtime_sec(mtime_diff(end_timestamp, start_timestamp)));

	mwin_destroy(mwin_get_instance());
}
