#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include <list>
#include <cmath>

using namespace std;

double angle = (360 / 5) * (3.14159 / 180.0);
double gravity = 0.2;

int w, h;
ALLEGRO_FONT *font = nullptr;

double PHI = sqrt(5.0f) / 2.0f + 0.5f; // golden ratio

class App : public IApp {
public:

	int frame = 0;
	double scale = 250.0;
	double scaleFactor;
	int frames_per_quarter = 50;

	void init() {
		w = al_get_display_width(al_get_current_display());
		h = al_get_display_height(al_get_current_display());
		printf("Golden ratio: %f\n", PHI);
		font = al_create_builtin_font();

		scaleFactor = pow(PHI, 1.0 / -frames_per_quarter);
		printf("Scale factor: %f\n", scaleFactor);
	}

	void update() override {
		frame++;
		// radius increases by PHI every frames_per_quarter frames
		scale *= scaleFactor; // 
	}

	void draw(const GraphicsContext &gc) override {
		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);
		al_scale_transform(&t, scale, scale);
		al_translate_transform(&t, w / 2, h / 2);
		al_use_transform(&t);

		al_clear_to_color(al_map_rgb(0, 0, 0));
		double ox = 0;
		double oy = 0;
		double angle = 0;
		bool first = true;
		int two_rotations = frames_per_quarter * 8;
		for (int i = max(0, frame - two_rotations); i < frame; ++i) {
			double frac = (double)i / (double)frames_per_quarter;
			double angle = frac * 0.5 * M_PI;
			double radius = pow(PHI, frac);
			double x = radius * cos(angle);
			double y = radius * sin(angle);
			if (first) {
				first = false;
			} else {
				al_draw_line(ox, oy, x, y, al_map_rgb(128, 128, 128), 2.0f / scale);
			}

			ox = x;
			oy = y;
		}

		// rectangle version
		int fiba = 1;
		int fibb = 2;
		int dx = 0;
		int dy = 1;
		int xco = 1.1f;
		int yco = PHI;
		int total = frame / frames_per_quarter;
		for (int i = 0; i <= total; ++i) {
			int size = fiba;
			if (i < total + 10) {
				al_draw_rectangle(
					xco - dy * size, 
					yco + dx * size,
					xco - dx * size,
					yco - dy * size,
					al_map_rgb(255, 255, 255), 1.0f / scale
				);
			}
			
			// rotate dx, dy
			int tmp = dx;
			dx = -dy;
			dy = tmp;

			int fibc = fiba + fibb;
			fiba = fibb;
			fibb = fibc;

			xco += dx * fibb;
			yco += dy * fibb;

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
		.setTitle("Genuary26 Day 3")
		.setAppName("Genuary26.3")
		.setPreferredDisplayResolution(1024, 768);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
