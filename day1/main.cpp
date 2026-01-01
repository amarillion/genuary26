#include "simpleloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>

using namespace std;
using namespace Simple;

double angle = (360 / 5) * (3.14159 / 180.0);
double speed = 5.0;
double radius = 20.0;

class App : public IApp {
	float x, y;
	float ox, oy;
	float dx, dy;
	int w, h;
public:

	void init() {
		al_clear_to_color(al_map_rgb(0, 0, 0));
		w = al_get_display_width(al_get_current_display());
		h = al_get_display_height(al_get_current_display());
		x = w / 2; 
		y = h / 2;
		dx = cos(angle) * speed;
		dy = sin(angle) * speed;
		ox = x;
		oy = y;
	}

	void handleEvent(ALLEGRO_EVENT &evt) override {
	}

	bool update() override {
		ox = x;
		oy = y;
		
		x += dx;
		y += dy;

		if (x < radius || x > w - radius) dx = -dx;
		if (y < radius || y > h - radius) dy = -dy;

		return true;
	}

	void draw(const GraphicsContext &gc) override {
		al_draw_filled_circle(ox, oy, radius, al_map_rgb(128, 128, 128));
		al_draw_filled_circle(x, y, radius, al_map_rgb(255, 255, 255));
	}
};

int main(int argc, const char *const *argv)
{
	Simple::MainLoop mainloop = Simple::MainLoop();
	auto engine = make_shared<App>();

	mainloop
		.setFixedResolution(false)
		.setUsagiMode()
		.setTitle("Genuary26 Day 1")
		.setAppName("Genuary26.1")
		.setApp(engine);

	mainloop.setPreferredDisplayResolution(1024, 768);

	mainloop.init(argc, argv);
	engine->init();
	mainloop.run();
	return 0;
}
