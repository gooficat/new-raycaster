#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <math.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 400

bool keys[512];
SDL_Window* win;
SDL_Renderer* ren;

#define MAP_WIDTH 16
#define MAP_HEIGHT 16
int map[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
        {1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
        {1,0,0,0,1,1,1,1,0,0,0,0,0,1,0,1},
        {1,0,0,0,1,0,0,0,0,0,0,1,0,1,0,1},
        {1,0,0,0,1,1,1,0,0,0,0,1,0,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
