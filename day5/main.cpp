#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include <list>
#include <cmath>

using namespace std;

struct Vertex {
	float x;
	float y;
};

class App : public IApp {
public:

	int w, h;

	void init() {
		w = MainLoop::getMainLoop()->getw();
		h = MainLoop::getMainLoop()->geth();
	}

	void update() override {
		frame++;
	}

	int frame = 0;

	int SEG_LENGTH = 48;
	int SEG_WIDTH = 4;
	int GAP = 20;

	void drawSegment(int charIdx, int segIdx, ALLEGRO_COLOR fill) {
		// 14 segment coords
		double SEGMENTS[][4] = {
			{ 0, 0, 1, 0 },
			{ 1, 0, 1, 1 },
			{ 1, 1, 1, 2 },
			{ 1, 2, 0, 2 },
			{ 0, 1, 0, 2 },
			{ 0, 0, 0, 1 },
			{ 0, 1, 0.5, 1 },
			{ 0.5, 1, 1, 1 },
			
			{ 0, 0, 0.5, 1 },
			{ 0.5, 0, 0.5, 1},
			{ 1, 0, 0.5, 1 },
			{ 0, 2, 0.5, 1 },
			{ 0.5, 2, 0.5, 1 },
			{ 1, 2, 0.5, 1 }
		};

		int x0 = -(7 * (SEG_LENGTH + SEG_WIDTH * 2) + 6 * GAP) / 2;
		int x = x0 + charIdx * (SEG_LENGTH + GAP + SEG_WIDTH * 2);
		int y = - SEG_LENGTH;

		float x1 = x + SEG_LENGTH * SEGMENTS[segIdx][0];
		float y1 = y + SEG_LENGTH * SEGMENTS[segIdx][1];
		float x2 = x + SEG_LENGTH * SEGMENTS[segIdx][2];
		float y2 = y + SEG_LENGTH * SEGMENTS[segIdx][3];

		float dx = x2 - x1;
		float dy = y2 - y1;
		float len = sqrt(dx * dx + dy * dy);
		float nx = dx / len;
		float ny = dy / len;

		Vertex vertices[6];
		vertices[0].x = x1 + 1 * nx;
		vertices[0].y = y1 + 1 * ny;
		vertices[1].x = x1 + SEG_WIDTH * nx - SEG_WIDTH * ny;
		vertices[1].y = y1 + SEG_WIDTH * ny + SEG_WIDTH * nx;
		vertices[2].x = x2 - SEG_WIDTH * nx - SEG_WIDTH * ny;
		vertices[2].y = y2 - SEG_WIDTH * ny + SEG_WIDTH * nx;
		vertices[3].x = x2 - 1 * nx;
		vertices[3].y = y2 - 1 * ny;
		vertices[4].x = x2 - SEG_WIDTH * nx + SEG_WIDTH * ny;
		vertices[4].y = y2 - SEG_WIDTH * ny - SEG_WIDTH * nx;
		vertices[5].x = x1 + SEG_WIDTH * nx + SEG_WIDTH * ny;
		vertices[5].y = y1 + SEG_WIDTH * ny - SEG_WIDTH * nx;
		
		al_draw_filled_polygon((float*)vertices, 6, fill);
	}

	void draw(const GraphicsContext &gc) override {
		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);
		al_horizontal_shear_transform(&t, 0.2);
		al_translate_transform(&t, w / 2.0, h / 2.0);
		al_use_transform(&t);

		// draw all segments for each character
		al_clear_to_color(al_map_rgb(0, 0, 0));
		ALLEGRO_COLOR onColor = al_map_rgb(255, 0, 0);
		ALLEGRO_COLOR offColor = al_map_rgb(50, 0, 0);

		int characterData[] = {
			0xBD,
			0xF9,
			0x2136,
			0x3E,
			0xF7,
			0x20F3,
			0x1500
		};

		for (int charIdx = 0; charIdx < sizeof(characterData) / sizeof(characterData[0]); charIdx++) {
			for (int segIdx = 13; segIdx >= 0; segIdx--) {
				drawSegment(charIdx, segIdx, offColor);
			}
			for (int segIdx = 13; segIdx >= 0; segIdx--) {
				int mask = 1 << segIdx;
				int onOff = characterData[charIdx] & mask;
				if (frame % 7 == segIdx && frame % 11 == charIdx) {
					continue;
				}
				if (onOff > 0) {
					drawSegment(charIdx, segIdx, onColor);
				}
			}
		}
	}

	virtual ~App() {}
};

int main(int argc, const char *const *argv)
{
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 5")
		.setAppName("Genuary26.5")
		.setLogicIntervalMsec(200)
		.setPreferredGameResolution(1024, 768);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
