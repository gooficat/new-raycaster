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
	rec.w = HEIGHT/MAP_HEIGHT;
	rec.h = HEIGHT/MAP_HEIGHT;
	for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			if (map[y][x]==1) {
				rec.x = x * HEIGHT/MAP_HEIGHT;
				rec.y = y * HEIGHT/MAP_HEIGHT;
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
		if (i > 800) break;
	}
	
	float dist = sqrt(pow(x-p.x, 2) + pow(y-p.y, 2));
	return dist;
}
SDL_FRect r;
void drawLine(int i, int h, int w) {
	SDL_SetRenderDrawColor(ren, 0, h/3, h/3, 255);
	r = (SDL_FRect){i * w, HEIGHT/2 - h/2, w, h};
	SDL_RenderFillRect(ren, &r);
}
#define FOV 1.5f
void rayCast() {
	int rays = 200;
	float sliceWidth = WIDTH / rays;
	float step = FOV / rays;
	
	for (int i = 0; i < rays; i++) {
		float angle = (p.r-(float)M_PI / 2.0f) - (FOV/2) + i * step;
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

void normalize(float *x, float *y) {
	float w = sqrt((*x) * (*x) + (*y) * (*y));
	*x /= w;
	*y /= w;
}

#define PLAYER_SPEED 0.004f
int main() {
	if(!SDL_Init(SDL_INIT_VIDEO)) return -1;
	win = SDL_CreateWindow("", WIDTH, HEIGHT, 0);
	ren = SDL_CreateRenderer(win, NULL);

	SDL_FRect floorRect = {0, HEIGHT/2, WIDTH, HEIGHT/2};

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
			p.vy += PLAYER_SPEED;
		if (keys[SDL_SCANCODE_S])
			p.vy -= PLAYER_SPEED;
		if (keys[SDL_SCANCODE_A])
			p.vx += PLAYER_SPEED;
		if (keys[SDL_SCANCODE_D])
			p.vx -= PLAYER_SPEED;
		
		if (keys[SDL_SCANCODE_Q])
			p.r -= PLAYER_SPEED/2 * dt;
		if (keys[SDL_SCANCODE_E])
			p.r += PLAYER_SPEED/2 * dt;

		SDL_FRect i = {-p.vx * 5000 + 100, -p.vy * 5000 + 100, 10, 10};

		bool col = false;
		
		//normalize(&(p.vx), &(p.vy));
		
		float ca = cos(p.r);
		float sa = sin(p.r);
		float nX = ca * p.vx + sa * p.vy;
		p.vy = -(ca*p.vy + sa * p.vx);
		p.vx = nX;
		
		if (!col) {
			p.x += p.vx * dt;
			p.y += p.vy * dt;
		}
		
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren, 0, 100, 0, 255);
		SDL_RenderFillRect(ren, &floorRect);
		
		rayCast();
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_RenderFillRect(ren, &i);
		if (keys[SDL_SCANCODE_M]) {
			SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
			drawMap2D();
		}
		
		SDL_RenderPresent(ren);
	}
	SDL_Quit();
	return 0;
}