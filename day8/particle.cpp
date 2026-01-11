#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include "mainloop.h"
#include "pixelbuffer.h"
#include <memory>
#include "color.h"

using namespace std;

constexpr int COLOR_COUNT = 7; // number of fireworks color variants
constexpr int GRADE_COUNT = 8; // number of grades in fireworks gradient
constexpr int PARTICLE_MAX = 300;

constexpr int SKY_GRADE_COUNT = 40; // number of grades in sky gradient

constexpr int ARROW_MAX = 7;
constexpr int ARROW_MIN = 2;

constexpr double GRAVITY = 0.05;
constexpr int EXPLOSION_TIME = 70;
constexpr int EXPLOSION_PARTICLES = 10;

constexpr int SKYLINE_MAX_HEIGHT = 60; // height of buildings
constexpr int NUM_STARS = 300;

constexpr int SCREEN_W = 320;
constexpr int SCREEN_H = 200;
constexpr int HORIZON_RATIO = 3;
constexpr int HORIZON = SCREEN_H / (HORIZON_RATIO + 1) * HORIZON_RATIO;

constexpr double WAVE_SPEED = 5.0 * M_PI / 180.0;

enum ParticleType { PT_DISABLED, PT_ARROW, PT_EXPLOSION };

struct Particle {
	double x, y, dx, dy;
	int type = PT_DISABLED;
	int time = 0;
	int color = 0;
	int intensity = 0;
};

std::vector<Particle> particles(PARTICLE_MAX);

int arrow_count = 0;
int particle_count = 0;

double angle_wave = 0.0;

ALLEGRO_BITMAP* background = nullptr;
ALLEGRO_BITMAP* skyline = nullptr;
ALLEGRO_BITMAP* mirror = nullptr;
ALLEGRO_FONT* font = nullptr;

bool show_fps = false;
bool do_vsync = false;

/* ------------------------------------------------------------ */

ALLEGRO_COLOR firework_color(int base, int intensity) {
	static ALLEGRO_COLOR base_colors[COLOR_COUNT] = {
		{1,1,1,1}, {1,1,0,1}, {1,0,1,1},
		{1,0,0,1}, {0,1,0,1}, {0,1,1,1}, {0,0,1,1}
	};
	float scale = float(intensity) / (GRADE_COUNT - 1);
	ALLEGRO_COLOR c = base_colors[base];
	return al_map_rgb_f(c.r * scale, c.g * scale, c.b * scale);
}

int find_free_particle() {
	for (int i = 0; i < PARTICLE_MAX; i++)
		if (particles[i].type == PT_DISABLED)
			return i;
	return 0;
}

void init_particles() {
	for (auto& p : particles)
		p.type = PT_DISABLED;
}

/* ------------------------------------------------------------ */

void create_arrow() {
	int idx = find_free_particle();
	Particle& p = particles[idx];
	if (p.type == PT_DISABLED) particle_count++;

	p.x = SCREEN_W / 4 + rand() % (SCREEN_W / 2);
	p.y = HORIZON;
	p.time = rand() % 50 + 50;
	p.dy = (rand() % 100 / 50.0 - 4) * sqrt(HORIZON) / 13;
	p.dx = ((rand() % 100) / 50.0 - 1) * 300 / SCREEN_W;
	p.type = PT_ARROW;
	p.color = rand() % COLOR_COUNT;
	p.intensity = GRADE_COUNT - 1;

	arrow_count++;
}

void explode(Particle& p) {
	p.type = PT_DISABLED;
	arrow_count--;
	particle_count--;

	double adelta = (M_PI * 2.0) / (double)EXPLOSION_PARTICLES;
	for (int i = 0; i < EXPLOSION_PARTICLES; ++i) {
		double alpha = (double)(i) * M_PI * 2.0 / (double)EXPLOSION_PARTICLES;
		for (int j = 0; j < 2; j++) {
			int idx = find_free_particle();
			Particle& q = particles[idx];
			if (q.type == PT_DISABLED) particle_count++;

			double speed = j + double(rand() % 40) / 40.0;
			q.x = p.x;
			q.y = p.y;
			q.dx = speed * sin(alpha);
			q.dy = speed * cos(alpha);
			q.type = PT_EXPLOSION;
			q.time = EXPLOSION_TIME;
			q.color = p.color;
			q.intensity = p.intensity;
		}
	}
}

void move_particles() {
	for (auto& p : particles) {
		if (p.type == PT_ARROW) {
			p.dy += GRAVITY;
			p.x += p.dx;
			p.y += p.dy;
			if (--p.time <= 0)
				explode(p);
		}
		else if (p.type == PT_EXPLOSION) {
			p.dy += GRAVITY;
			p.dx *= 0.98;
			p.dy *= 0.98;
			p.x += p.dx;
			p.y += p.dy;
			if (--p.time <= 0) {
				p.type = PT_DISABLED;
				particle_count--;
			}
			p.intensity = p.time * GRADE_COUNT / EXPLOSION_TIME;
		}
	}
}

/* ------------------------------------------------------------ */

void draw_particles() {
	for (auto& p : particles) {
		if (p.type == PT_DISABLED) continue;
		if (p.x < 1 || p.y < 1 || p.x >= SCREEN_W - 1 || p.y >= SCREEN_H - 1)
			continue;

		ALLEGRO_COLOR c = firework_color(p.color, p.intensity);
		al_draw_pixel(p.x, p.y, c);

		if (p.intensity > 1) {
			ALLEGRO_COLOR c2 = firework_color(p.color, p.intensity - 2);
			al_draw_pixel(p.x + 1, p.y, c2);
			al_draw_pixel(p.x - 1, p.y, c2);
			al_draw_pixel(p.x, p.y + 1, c2);
			al_draw_pixel(p.x, p.y - 1, c2);
		}
	}
}

void create_background() {
	background = al_create_bitmap(SCREEN_W, HORIZON);
	al_set_target_bitmap(background);

	ALLEGRO_COLOR sky_base = al_map_rgb( 60, 60, 255 ); 

	double h = 2.0 * HORIZON / SKY_GRADE_COUNT;
	double dh = h / SKY_GRADE_COUNT;
	double y1 = 0;

	for (int i = 0; i < SKY_GRADE_COUNT; i++) {
		double fraction = (double)i / double(SKY_GRADE_COUNT-1); 
		ALLEGRO_COLOR sky = al_map_rgb_f(
			fraction * sky_base.r,
			fraction * sky_base.g,
			fraction * sky_base.b
		);
		double y2 = y1 + h;
		h -= dh;
		al_draw_filled_rectangle(
			0, y1, SCREEN_W, y2, sky
		);
		y1 = y2;
	}

	for (int i = 0; i < NUM_STARS; i++) {
		int star_gray = ((rand() % 4) * 32) + 127;
		ALLEGRO_COLOR star_color = al_map_rgb(star_gray, star_gray, star_gray);
		al_draw_pixel(rand() % SCREEN_W, rand() % HORIZON, star_color);
	}

	mirror = al_create_bitmap(SCREEN_W, SCREEN_H - HORIZON);
	skyline = al_create_bitmap(SCREEN_W, SKYLINE_MAX_HEIGHT);
	al_set_target_bitmap(skyline);
	al_clear_to_color(TRANSPARENT);

	ALLEGRO_COLOR building_color = al_map_rgb(28, 28, 28);
	ALLEGRO_COLOR window_color = al_map_rgb(127, 127, 0);

	for (int x = 0; x < SCREEN_W; x += 15) {
		// int h = rand() % 50 + 5;
		// draw skyscrapers
		// for (int y = SKYLINE_MAX_HEIGHT - h; y < SKYLINE_MAX_HEIGHT; y += 5) {
		// 	al_draw_filled_rectangle(x, y, x + 15, y + 5,
		// 		al_map_rgb(50, 50, 50));

			
		// }

		int h = rand () % 50 + 5; // must be smaller than background_max_height
		int step = rand () % 3 + 2;
		for (int j = SKYLINE_MAX_HEIGHT - h;
			j < SKYLINE_MAX_HEIGHT; j += step)
		{
			// draw the building
			al_draw_filled_rectangle (x, j, x + 15, j + 5, building_color);
			// and some windows
			if (rand() % 100 > 50)
				al_draw_filled_rectangle(x + 2, j + 1, x + 5, j + 2, window_color);
			if (rand() % 100 > 50)
				al_draw_filled_rectangle(x + 6, j + 1, x + 9, j + 2, window_color);
			if (rand() % 100 > 50)
				al_draw_filled_rectangle(x + 10, j + 1, x + 13, j + 2, window_color);
		}

	}

	al_set_target_backbuffer(al_get_current_display());
}

void move_fireworks() {
	if (arrow_count < ARROW_MIN) create_arrow();
	if (arrow_count < ARROW_MAX && rand() % 100 > 95) create_arrow();
	move_particles();
	angle_wave += WAVE_SPEED;
}

void draw_mirror() {
	ALLEGRO_BITMAP *cur = al_get_target_bitmap();
	al_set_target_bitmap(mirror);
	for (int i = 0; i < SCREEN_H - HORIZON; i++) {
		// variation for this line for the wave movement.
		double angle = angle_wave + ((double)i / 20.0);
		int var = sin(angle) * (double)(SCREEN_H - HORIZON - i) * 0.1;

		al_draw_bitmap_region(
			cur, 
			0, i * HORIZON_RATIO + 1, SCREEN_W, i * HORIZON_RATIO + 2,
			0 + var, i,
			0
		);
	}

	al_set_target_bitmap(cur);
	al_draw_tinted_bitmap(mirror, GREY, 0, HORIZON, ALLEGRO_FLIP_VERTICAL);
}

class Fireworks : public IComponent {
public:
	Fireworks() {
		init_particles();
		create_background();
	}

	void draw(const GraphicsContext &gc) override {
		al_draw_bitmap(background, 0, 0, 0);
		draw_particles();
		al_draw_bitmap(skyline, 0, HORIZON - SKYLINE_MAX_HEIGHT, 0);
		draw_mirror();
	};

	void update() {
		move_fireworks();
	}
};

class App: public IApp {

	std::shared_ptr<PixelBuffer> pixelBuffer;
	std::shared_ptr<Fireworks> fireworks;

public:
	App() {
		fireworks = make_shared<Fireworks>();
		pixelBuffer = make_shared<PixelBuffer>(fireworks, SCREEN_W, SCREEN_H);
	}

	void draw(const GraphicsContext &gc) override {
		pixelBuffer->draw(gc);
	};

	void update() override {
		fireworks->update();
	}
};

int main(int argc, char** argv) {
	MainLoop mainloop;
	srand(time(nullptr));

	mainloop
		.setTitle("Genuary26 Day 8")
		.setAppName("Genuary26.8")
		.setPreferredDisplaySize(1280, 800);

	if (!mainloop.init(argc, argv)) {
		App app;
		mainloop.run(&app);
	}
	return 0;
}
