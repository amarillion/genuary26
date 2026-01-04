#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include <list>
#include <cmath>

using namespace std;

void do_bresenham(int x0, int y0, int x1, int y1, function<void(int, int)> cb){
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	for (;;) {
		cb(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

ALLEGRO_COLOR ZX_PALETTE[8] {
	al_map_rgb(0, 0, 0),
	al_map_rgb(255, 0, 0),
	al_map_rgb(0, 255, 0),
	al_map_rgb(0, 0, 255),
	al_map_rgb(255, 255, 0),
	al_map_rgb(255, 0, 255),
	al_map_rgb(0, 255, 255),
	al_map_rgb(255, 255, 255),
};

class App : public IApp {
public:

	ALLEGRO_BITMAP *buf;

	ALLEGRO_COLOR bg = al_map_rgb(0, 0, 255);
	ALLEGRO_COLOR fg = al_map_rgb(255, 255, 0);

	int w, h;

	void init() {
		w = MainLoop::getMainLoop()->getw();
		h = MainLoop::getMainLoop()->geth();

		// due to frequent use of GET_PIXEL, we need a memory buffer.
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
		buf = al_create_bitmap(256, 192);
		al_set_target_bitmap(buf);
		al_clear_to_color(bg);
	}


	void update() override {
	}

	void currentLine(int frame, function<void(int, int, int, int)> cb) {
		int step = 2;
		int cx = 128; int cy = 192;
		int counter = 0;
		for (int y = 192; y > 0; y -= step, counter++) {
			if (counter == frame) {
				cb(cx, cy, 0, y);
				return;
			}
		}
		for (int x = 0; x < 256; x += step, counter++) {
			if (counter == frame) {
				cb(cx, cy, x, 0);
				return;
			}
		}
		for (int y = 0; y <= 192; y += step, counter++) {
			if (counter == frame) {
				cb(cx, cy, 256, y);
				return;
			}
		}
	}

	int frame = 0;
	bool first = true;

	void draw(const GraphicsContext &gc) override {
		frame++;
		al_set_target_bitmap(buf);
		if (first) {
			// randomize bg / fg
			int bgIndex = rand() % 8;
			int fgIndex;
			do {
				fgIndex = rand() % 8;
			} while (fgIndex == bgIndex);
			bg = ZX_PALETTE[bgIndex];
			fg = ZX_PALETTE[fgIndex];

			al_clear_to_color(bg);
			first = false;
		}

		if (frame > 720) {
			first = true;
			frame = 0;
		}

		// auto draw_bresenham = [&](int x0, int y0, int x1, int y1, ALLEGRO_COLOR color) {
		// 	do_bresenham(x0, y0, x1, y1, [&](int x, int y) {
		// 		al_put_pixel(x, y, color);
		// 	});
		// };

		auto xor_bresenham = [&](int x0, int y0, int x1, int y1) {
			do_bresenham(x0, y0, x1, y1, [&](int x, int y) {
				ALLEGRO_COLOR existing = al_get_pixel(gc.buffer, x, y);
				bool equal = (existing.r == fg.r && existing.g == fg.g && existing.b == fg.b);
				al_put_pixel(x, y, equal ? bg : fg);
			});
		};

		currentLine(frame % 360, [&](int cx, int cy, int x1, int y1) {
			xor_bresenham(cx, cy, x1, y1);
		});

		al_set_target_bitmap(gc.buffer);
		al_draw_bitmap(buf, 0, 0, 0);
	}

	virtual ~App() {}
};

int main(int argc, const char *const *argv)
{
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 4")
		.setAppName("Genuary26.4")
		.setLogicIntervalMsec(33) // ~30 fps
		.setPreferredGameResolution(256, 192)
		.setPreferredDisplayResolution(1024, 768);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
