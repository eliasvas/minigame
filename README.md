# Minigame
## _A minigame made whenever I feel like it._

This is a 2D minigame I am making in C99 with most of the Platform dependant stuff done in SDL2.
It will propably take a long time because I don't want to rush it. Should be fun.

## Architecture
The whole game is inside _mg.c_ everything else is the framework to make it.
In _SDL_Backend.c_ is all the external code.. it can be changed seamlessly to any library, the code is minimal.
Most functions in the codebase are **static internal**, because I'm too bored to write good code. I freeballed it.
## Build
CMake is used to bulid the project for both windows and linux.
#### Some Things to get you started on building with CMake
- https://code.visualstudio.com/docs/cpp/cmake-linux
- https://cmake.org/cmake/help/latest/guide/tutorial/index.html