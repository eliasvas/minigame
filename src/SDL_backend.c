#include <SDL2/SDL.h>

#include "base.h"
#include "mWin.h"

typedef struct {
	SDL_Surface* window_surface;
	SDL_Window* window;
}SDLImplWindow;

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
