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
	{1,0,0,0,1,0,0,0,0,0,0,110,1,0,1},
	{1,0,0,0,1,1,1,0,0,0,0,1,0,1,0,1},
	{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1},
	{1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

typedef struct {
	float x, y, r;
	float vx, vy;
} player_t;

player_t p = {
	8.0f, 8.0f, -(float)M_PI / 2.0f
};

SDL_FRect rec;
void drawMap2D() {
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
		SDL_SetRenderDrawColor(ren, 0, h/3, h/3, 255);
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
		float wh = 300/dist;
		drawLine(i, wh, sliceWidth);
	}
}

bool point_in_rect(float px, float py, int x, int y, int w, int h) {
	if (px > x && px < x + w &&
		py > y && py < y + h)
		return true;
	return false;
}

#define PLAYER_SPEED 0.04f
int main() {
	if(!SDL_Init(SDL_INIT_VIDEO)) return -1;
	win = SDL_CreateWindow("", WIDTH, HEIGHT, 0);
	ren = SDL_CreateRenderer(win, NULL);

	float frameTime = SDL_GetTicks();
	float lastFrameTime = frameTime;
	float dt = 0;
	bool running = true;
	SDL_Event e;
	while (running) {
		frameTime = SDL_GetTicks();
		dt = frameTime - lastFrameTime;
		lastFrameTime = frameTime;
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
		p.vx = 0;
		p.vy = 0;
		if (keys[SDL_SCANCODE_W])
			p.vy -= PLAYER_SPEED;
		if (keys[SDL_SCANCODE_S])
			p.vy += PLAYER_SPEED;
		if (keys[SDL_SCANCODE_A])
			p.vx -= PLAYER_SPEED;
		if (keys[SDL_SCANCODE_D])
			p.vx += PLAYER_SPEED;
		
		if (keys[SDL_SCANCODE_Q])
			p.r -= PLAYER_SPEED/20 * dt;
		if (keys[SDL_SCANCODE_E])
			p.r += PLAYER_SPEED/20 * dt;

		bool col = false;

		if (!col) {
			p.x += p.vx;
			p.y += p.vy;
		}
		
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		rayCast();
		
		if (keys[SDL_SCANCODE_M]) {
			SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
			drawMap2D();
		}
		
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}