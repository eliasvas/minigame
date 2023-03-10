#include <SDL2/SDL.h>

#include "base.h"

//WINDOW IMPLEMENTATION
#include "mWin.h"
typedef struct {
	SDL_Surface* window_surface;
	SDL_Window* window;
}SDLImplWindow;

mWin window;

mWin *mwin_get_instance() {
	return &window;
}
M_RESULT mwin_create(mWinDesc *desc, mWin *win){
	//SDL window implementation
	win->desc = *desc;

	SDLImplWindow *sdl_win = malloc(sizeof(SDLImplWindow));
	MEMZERO_STRUCT(sdl_win);

	//should SDL_Init have its own API????
	if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
		printf("Error initializing SDL: [%s]\n", SDL_GetError());
		return M_ERR;
	}
	sdl_win->window = SDL_CreateWindow( "minigame", desc->x, desc->y, desc->width, desc->height, SDL_WINDOW_SHOWN );

	if ( !sdl_win->window ) {
		printf("Error creating window: [%s]\n", SDL_GetError());
		return M_ERR;
	}
	sdl_win->window_surface = SDL_GetWindowSurface( sdl_win->window );

	if ( !sdl_win->window_surface ) {
		printf("Error getting surface: [%s]\n",SDL_GetError());
		
		return M_ERR;
	}
	SDL_FillRect( sdl_win->window_surface, NULL, SDL_MapRGB( sdl_win->window_surface->format, 255, 64, 64 ) );
	SDL_UpdateWindowSurface( sdl_win->window );

	//SDL_DestroyWindow( sdl_win->window );

	//SDL_Quit();

	win->internal_state = (void*)sdl_win;
	return M_OK;
}


M_RESULT mwin_destroy(mWin *win){
	SDLImplWindow *internal_data = win->internal_state;
	SDL_DestroyWindow(internal_data->window);
	MEMZERO_STRUCT(win);
	//what we gonna do about alloc and frees of the object tho?
	//is this destroy just for internal data? then we would just:
	free(internal_data);
}

//INPUT IMPLEMENTATION
#include "mInput.h"
struct mInputState;
extern mInputState mis;
void minput_update(void)
{
    MEMCPY(mis.prev_keys,mis.keys, sizeof(mis.keys[0]) * MK_MAX);

	SDL_PumpEvents();
	const u8 *keystate = SDL_GetKeyboardState(NULL);

	for (int i = (MK_A - MK_A); i <= (MK_Z - MK_A); ++i){
		if (keystate[SDL_SCANCODE_A + i] > 0){
			mis.keys[MK_A + i] = 1;
		}else{
			mis.keys[MK_A + i] = 0;
		}
	}
	for (int i = (MK_0 - MK_0); i <= (MK_9 - MK_A); ++i){
		if (keystate[SDL_SCANCODE_0 + i] > 0){
			mis.keys[MK_0 + i] = 1;
		}else{
			mis.keys[MK_0 + i] = 0;
		}
	}
	int x;
	int y;
	u32 mouse_state = SDL_GetMouseState(&x, &y);
	mis.mouse_delta_x = x - mis.mouse_pos_x;
	mis.mouse_delta_y = y - mis.mouse_pos_y;
	mis.mouse_pos_x = x;
	mis.mouse_pos_y = y;
	if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)){
		mis.keys[MK_LMB] = 1;
	}else {
		mis.keys[MK_LMB] = 0;
	}
	//printf("Mouse: [%i:%i], [%i,%i]\n", mis.mouse_pos_x, mis.mouse_pos_y, mis.mouse_delta_x, mis.mouse_delta_y);

	if (mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT)){
		mis.keys[MK_RMB] = 1;
	}else {
		mis.keys[MK_RMB] = 0;
	}

	if (mouse_state & SDL_BUTTON(SDL_BUTTON_MIDDLE)){
		mis.keys[MK_MMB] = 1;
	}else {
		mis.keys[MK_MMB] = 0;
	}
	
}



//TEXTURES
#include "mTex.h"

typedef struct {
	SDL_Surface* image;
	SDL_PixelFormat format;
}SDLImplTexture;
//SDL_CreateRGBSurfaceFrom(data, IMAGE_WIDTH, IMAGE_HEIGHT, 8, IMAGE_WIDTH /*pitch*/, 0, 0, 0, 0);
M_RESULT mtex_create(mTexDesc *desc, mTex *tex){
	MEMZERO_STRUCT((mTex*)tex);
	tex->desc = *desc;

	SDLImplTexture *sdl_tex = malloc(sizeof(SDLImplTexture));
	MEMZERO_STRUCT((SDLImplTexture*)sdl_tex);

	sdl_tex->image = SDL_LoadBMP(tex->desc.filename);

	if (sdl_tex->image == NULL) {
		printf("Error loading image: [%s]\n", tex->desc.filename);
		return M_ERR;
	}
	tex->internal_state = sdl_tex;

	return M_OK;
}
M_RESULT mtex_destroy(mTex *tex){
	M_RESULT res = M_OK;
	//code
	SDLImplTexture *sdl_texture = (SDLImplTexture*)tex->internal_state;
	SDL_FreeSurface(sdl_texture->image);
	free(sdl_texture);
	MEMZERO_STRUCT(tex);
	return res;
}
M_RESULT mtex_render(mTex *tex, mRect sub_area, mRect render_area){
	M_RESULT res = M_OK;
	SDLImplTexture *sdl_tex = tex->internal_state;
	SDLImplWindow *dest_tex = window.internal_state;
	SDL_Rect sub_rect = {sub_area.x, sub_area.y,sub_area.w, sub_area.h};
	SDL_Rect dst_rect = {render_area.x, render_area.y,render_area.w, render_area.h};
	SDL_BlitScaled(sdl_tex->image,&sub_rect,dest_tex->window_surface,&dst_rect);

	SDL_UpdateWindowSurface(dest_tex->window);

	return res;
}