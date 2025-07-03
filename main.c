#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <math.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 600

bool keys[512];
SDL_Window* win;
SDL_Renderer* ren;

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
int map[MAP_HEIGHT][MAP_WIDTH] = {
	{1,1,1,1,1,1,1,1},
	{1,0,1,0,0,0,0,1},
	{1,0,0,1,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,1,1,0,1},
	{1,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1}
};

typedef struct {
	float x, y, r;
} player_t;

player_t p = {
	0, 0, 0
};

void drawMap2D() {
	SDL_FRect rec;
	for(int y = 0; y < 8; y++) {
		for(int x = 0; x < 8; x++) {
			if (map[y][x]==1) {
				rec = (SDL_FRect){x * HEIGHT/MAP_HEIGHT, y * HEIGHT/MAP_HEIGHT, HEIGHT/MAP_HEIGHT, HEIGHT/MAP_HEIGHT};
				SDL_RenderFillRect(ren, &rec);
			}
		}
	}
}

float castRay(float a) {
	float x = p.x, y = p.y;
	float dx = cos(a),
		  dy = sin(a);
	
	int i = 0;
	while (map[(int)y][(int)x] == 0) {
		x += dx * 0.1;
		y += dy * 0.1;
		i++;
		if (i > 400) break;
	}
	
	float dist = sqrt(((x-p.x)*(x-p.x)) + ((y-p.y)*(y-p.y)));
	return dist;
}

void drawLine(int i, int h, int w) {
	for (int j = 0; j < h; j++) {
		float y = floor(300 - h / 2 + j);
		SDL_FRect r = (SDL_FRect){i * w, y, w, 1};
		SDL_RenderFillRect(ren, &r);
	}
}
#define FOV 120
void rayCast() {
	int rays = 200;
	float sliceWidth = WIDTH / rays;
	float step = FOV / rays;
	
	for (int i = 0; i < rays; i++) {
		float angle = p.r - (FOV/2) + i * step;
		float dist = castRay(angle);
		float wh = 300/dist;
		drawLine(i, wh, sliceWidth);
	}
}


int main() {
	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return -1;
	SDL_CreateWindowAndRenderer("", WIDTH, HEIGHT, 0, &win, &ren);
	
	bool running = true;
	SDL_Event e;
	while (running) {
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				return false;
			}
			else if (e.type == SDL_EVENT_KEY_DOWN) {
				keys[e.key.scancode] = true;
			}
			else if (e.type == SDL_EVENT_KEY_UP) {
				keys[e.key.scancode] = false;				
			}
		}
		
		if (keys[SDL_SCANCODE_W])
			p.y -= 1;
		if (keys[SDL_SCANCODE_S])
			p.y += 1;
		if (keys[SDL_SCANCODE_A])
			p.x -= 1;
		if (keys[SDL_SCANCODE_D])
			p.x += 1;
		
		
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		rayCast();
		
		SDL_RenderPresent(ren);
		SDL_Delay(16);
	}
	SDL_Quit();
	return 0;
}
