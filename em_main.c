#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <emscripten/emscripten.h>
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
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

typedef struct {
	float x, y, r;
} player_t;

player_t p = {
	4.0f, 4.0f, -(float)M_PI / 2.0f
};

void drawMap2D() {
	SDL_FRect rec;
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
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
		x += dx * 0.01f;
		y += dy * 0.01f;
		i++;
		if (i > 400) break;
	}
	
	float dist = sqrt(pow(x-p.x, 2) + pow(y-p.y, 2));
	return dist;
}

void drawLine(int i, int h, int w) {
	for (int j = 0; j < h; j++) {
		float y = floor(250 - h / 2.0f + j);
		SDL_SetRenderDrawColor(ren, h/3, h/3, 0, 255);
		SDL_FRect r = (SDL_FRect){i * w, y, w, 1};
		SDL_RenderFillRect(ren, &r);
	}
}
#define FOV 1.5f
void rayCast() {
	int rays = 200;
	float sliceWidth = WIDTH / rays;
	float step = FOV / rays;
	
	for (int i = 0; i < rays; i++) {
		float angle = p.r - (FOV/2) + i * step;
		float dist = castRay(angle);
		float wh = 200/dist;
		drawLine(i, wh, sliceWidth);
	}
}

SDL_Event e;
float frameTime = SDL_GetTicks();
float lastFrameTime = frameTime;
float dt = 0;
void game_loop() {
	frameTime = SDL_GetTicks();
	dt = frameTime - lastFrameTime;
	lastFrameTime = frameTime;
	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_KEY_DOWN) {
			keys[e.key.scancode] = true;
		}
		else if (e.type == SDL_EVENT_KEY_UP) {
			keys[e.key.scancode] = false;				
		}
	}
	
	if (keys[SDL_SCANCODE_W])
		p.y -= 0.004f * dt;
	if (keys[SDL_SCANCODE_S])
		p.y += 0.004f * dt;
	if (keys[SDL_SCANCODE_A])
		p.x -= 0.004f * dt;
	if (keys[SDL_SCANCODE_D])
		p.x += 0.004f * dt;
	
	if (keys[SDL_SCANCODE_Q])
		p.r -= 0.002f * dt;
	if (keys[SDL_SCANCODE_E])
		p.r += 0.002f * dt;
	
	if (keys[SDL_SCANCODE_M])
		drawMap2D();
	
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
	rayCast();
	
	SDL_RenderPresent(ren);
}
	
int main() {
	if(!SDL_Init(SDL_INIT_VIDEO)) return -1;
	SDL_CreateWindowAndRenderer("", WIDTH, HEIGHT, 0, &win, &ren);

	emscripten_set_main_loop(game_loop, 0, 1);
	SDL_Quit();
	return 0;
}