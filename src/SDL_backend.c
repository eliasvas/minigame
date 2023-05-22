#ifdef __gnu_linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#else
#include <SDL.h>
#include <SDL_mixer.h>
#endif
#include "base.h"

M_RESULT mbackend_init(void){
	if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
		printf("Error initializing SDL: [%s]\n", SDL_GetError());
		return M_ERR;
	}
	i32 audio_rate = 22050;
	u16 audio_format = AUDIO_S16SYS;
	i32 audio_channels = 2;
	i32 audio_buffers = 4096;
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0){
		printf("Error initializing SDL audio: [%s]\n", Mix_GetError());
		return M_ERR;
	}
	return M_OK;
}

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

	sdl_win->window = SDL_CreateWindow( "minigame", desc->x, desc->y, desc->width, desc->height, SDL_WINDOW_SHOWN);

	if ( !sdl_win->window ) {
		printf("Error creating window: [%s]\n", SDL_GetError());
		return M_ERR;
	}
	sdl_win->window_surface = SDL_GetWindowSurface( sdl_win->window );

	if ( !sdl_win->window_surface ) {
		printf("Error getting surface: [%s]\n",SDL_GetError());
		
		return M_ERR;
	}


	SDL_SetWindowResizable(sdl_win->window, (desc->opt & MWIN_OPT_RESIZABLE) > 0);
	SDL_SetWindowBordered(sdl_win->window, (desc->opt & MWIN_OPT_BORDERED) > 0);
	if (SDL_SetSurfaceBlendMode(sdl_win->window_surface,SDL_BLENDMODE_BLEND) < 0){
		printf("Error setting blend mode: [%s]\n", SDL_GetError());
	}
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
mInputState mis;
void minput_update(void)
{
    MEMCPY(mis.prev_keys,mis.keys, sizeof(mis.keys[0]) * MK_MAX);


	//SDL_PumpEvents();
	//Poll all SDL_Events. Because.
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_WINDOWEVENT:
				if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
					SDLImplWindow *sdl_win = window.internal_state;
					SDL_Surface* surf = SDL_GetWindowSurface(sdl_win->window);
					int w, h;
					SDL_GetWindowSize(sdl_win->window, &w, &h);
					sdl_win->window_surface = surf;
					window.desc.width = w;
					window.desc.height = h;
				}
			break;
			case SDL_QUIT:
				exit(1);
			break;
		}
	}
	const u8 *keystate = SDL_GetKeyboardState(NULL);

	for (int i = (MK_A - MK_A); i <= (MK_Z - MK_A); ++i){
		if (keystate[SDL_SCANCODE_A + i] > 0){
			//printf("ON! [%i]\n", i);
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

/*
	//Poll all SDL_Events. Because.
	SDL_Event e;
	while (SDL_PollEvent(&e)) {}
*/

}

//RENDERER
#include "mRend.h"
#include "mTex.h"

typedef struct {
	SDL_Surface* image;
	void *texture_data;
	SDL_PixelFormat format;
}SDLImplTexture;

void mrend_clear(void){
	SDLImplWindow *sdl_win = window.internal_state;
	SDL_UpdateWindowSurface(sdl_win->window);
	SDL_FillRect( sdl_win->window_surface, NULL, SDL_MapRGBA( sdl_win->window_surface->format, 16, 16, 16, 255 ) );
}


void mrend_draw_rect(mRect rect, mColor col){
	SDLImplWindow *sdl_win = window.internal_state;
	SDL_Rect r = (SDL_Rect){rect.x, rect.y, rect.w, rect.h};
	SDL_FillRect(sdl_win->window_surface, &r, col.col);
}

void mrend_draw_tex(mTex *tex, mRect tex_coords, mRect rect){
	SDLImplTexture *sdl_tex = tex->internal_state;
	SDLImplWindow *dest_tex = window.internal_state;


	SDL_Rect tc = (SDL_Rect){tex_coords.x, tex_coords.y, tex_coords.w, tex_coords.h};
	SDL_Rect r = (SDL_Rect){rect.x, rect.y,rect.w, rect.h};

	SDL_BlitScaled(sdl_tex->image,&tc,dest_tex->window_surface,&r);
}


//TEXTURES
#include "mTex.h"
#include "mqoi.h"

M_RESULT mtex_create(mTexDesc *desc, void *tex_data, mTex *tex){
	MEMZERO_STRUCT((mTex*)tex);
	tex->desc = *desc;

	SDLImplTexture *sdl_tex = malloc(sizeof(SDLImplTexture));
	MEMZERO_STRUCT((SDLImplTexture*)sdl_tex);

	u32 channels = ((desc->format & MTEX_FORMAT_RGBA8U) || (desc->format & MTEX_FORMAT_RGBA8S)) ? 4 : 3;
	if (channels == 4)
		sdl_tex->image = SDL_CreateRGBSurfaceFrom(tex_data, desc->width, desc->height, 32,desc->width * sizeof(u8) * channels,0xFF000000, 0x00FF0000,0x0000FF00, 0x000000FF);
	else 
		sdl_tex->image = SDL_CreateRGBSurfaceFrom(tex_data, desc->width, desc->height, 24,desc->width * sizeof(u8) * channels,0xFF0000, 0x00FF00,0x0000FF, 0x000000);
	if (SDL_SetSurfaceBlendMode(sdl_tex->image,SDL_BLENDMODE_BLEND) < 0){
		printf("Error setting blend mode: [%s]\n", SDL_GetError());
	}
	tex->desc.width = desc->width;
	tex->desc.height = desc->height;
	sdl_tex->texture_data = tex_data;

	if (sdl_tex->image == NULL) {
		printf("Error loading image : [%s]\n", SDL_GetError());
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
	if (sdl_texture->texture_data != NULL)
		FREE(sdl_texture->texture_data);
	FREE(sdl_texture);
	MEMZERO_STRUCT(tex);
	return res;
}
M_RESULT mtex_render(mTex *tex, mRect tex_coords, mRect rect){
	M_RESULT res = M_OK;
	SDLImplTexture *sdl_tex = tex->internal_state;
	SDLImplWindow *dest_tex = window.internal_state;
	//clip the rect
	mtex_clip(&tex_coords, &rect, (mRect){0,0,600,600});
	mrend_draw_tex(tex,tex_coords,rect);
	return res;
}


//AUDIO
#include "mSound.h"

M_RESULT msound_load(mSoundDesc *desc, mSound *s){
	Mix_Chunk *sound = NULL;
	sound = Mix_LoadWAV(desc->filename);
	if(sound == NULL)
	{
		printf("Couldn't load sound file: %s\n", Mix_GetError());
		return M_ERR;
	}
	s->desc = *desc;
	s->internal_state = sound;
}

M_RESULT msound_play(mSound *s){
	Mix_Chunk *sound = (Mix_Chunk*)(s->internal_state);
	i32 channel;
	channel = Mix_PlayChannel(-1, sound, 0);
	return M_OK;
}

M_RESULT msound_destroy(mSound *s){
	Mix_Chunk *sound = (Mix_Chunk*)(s->internal_state);
	Mix_FreeChunk(sound); 
	memset(s, 0, sizeof(mSound));
	return M_OK;
}
