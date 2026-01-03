#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>
#include <list>
#include <cmath>

using namespace std;

double angle = (360 / 5) * (3.14159 / 180.0);
double gravity = 0.2;

int w, h;

class Sprite {
public:
	float x, y;
	float ox, oy;
	float dx, dy;
	float squash = 1.0f;
	float osquash = 1.0f;
	float radius = 50.0f;
	bool alive = true;

	Sprite() : x(0), y(0), ox(0), oy(0), dx(0), dy(0) {}

	void draw() {
		al_draw_filled_ellipse(
			ox, oy - radius * (1.0f - osquash), 
			radius * (2.0f - osquash), 
			radius * osquash, al_map_rgb(128, 128, 128));

		al_draw_filled_ellipse(
			x, y - radius * (1.0f - squash), 
			radius * (2.0f - squash), 
			radius * squash, al_map_rgb(255, 255, 255));
	}

	void update() {
		ox = x;
		oy = y;
		osquash = squash;

		dy += gravity;

		x += dx;
		y += dy;

		if (y > h) {
			// bounce and reduce speed
			dy = dy * -0.9;
			y = h;
		}

		// when near the ground, do squash effect
		squash = 1.0f;
		if (y > h - radius) {
			squash = max(0.5, 0.5 + ((h - y) / radius * 0.5));
		}
	}
};

class App : public IApp {
	list<Sprite> sprites;
public:

	void init() {
		al_clear_to_color(al_map_rgb(0, 0, 0));
		w = al_get_display_width(al_get_current_display());
		h = al_get_display_height(al_get_current_display());
		
		addSprite();
	}

	void addSprite() {
		int radius = rand() % 90 + 10;
		int bottom_margin = 100;
		Sprite sprite;
		sprite.radius = radius;
		sprite.x = radius * -2;
		sprite.y = (rand() % h) - bottom_margin - radius;
		sprite.dx = ((rand() % 50) / 10.0f) + 1.0f;
		sprite.dy = ((rand() % 20) / 5.0f) - 4.0f;
		sprite.ox = sprite.x;
		sprite.oy = sprite.y;
		sprites.push_back(sprite);
	}

	int frame = 0;

	void update() override {
		if (frame++ % 60 == 0) {
			addSprite();
		}

		for (auto &sprite : sprites) {
			sprite.update();
			if (sprite.x > w + sprite.radius * 2) {
				sprite.alive = false;
			}
		}

		// clear out dead sprites
		sprites.remove_if([](const Sprite &s) { return !s.alive; });
	}

	void draw(const GraphicsContext &gc) override {
		for (auto &sprite : sprites) {
			sprite.draw();
		}
	}

	virtual ~App() {}
};

int main(int argc, const char *const *argv)
{
	MainLoop mainloop;
	mainloop
		.setFixedResolution(false)
		.setUsagiMode()
		.setTitle("Genuary26 Day 2")
		.setAppName("Genuary26.2")
		.setPreferredDisplayResolution(1024, 768);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
