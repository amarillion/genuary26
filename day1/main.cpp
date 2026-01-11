#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>

using namespace std;

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

	void update() override {
		ox = x;
		oy = y;
		
		x += dx;
		y += dy;

		if (x < radius || x > w - radius) dx = -dx;
		if (y < radius || y > h - radius) dy = -dy;
	}

	bool isDone() override {
		return false;
	}

	void draw(const GraphicsContext &gc) override {
		al_draw_filled_circle(ox, oy, radius, al_map_rgb(128, 128, 128));
		al_draw_filled_circle(x, y, radius, al_map_rgb(255, 255, 255));
	}

	virtual ~App() {}
};

int main(int argc, const char *const *argv)
{
	MainLoop mainloop = MainLoop();

	mainloop
		.setTitle("Genuary26 Day 1")
		.setAppName("Genuary26.1")
		.setPreferredDisplaySize(1024, 768);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
