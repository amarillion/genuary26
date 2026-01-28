// compile with HEADLESS set to run from CLI and avoid allegro dependency
// #define HEADLESS

#include "partridge.h"

#ifndef HEADLESS

#include "mainloop.h"
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"


constexpr int SCREEN_W = 1280;
constexpr int SCREEN_H = 800;
constexpr int SCALE = SCREEN_H / ROOT;
constexpr int MARGINX = (SCREEN_W - (SCALE * ROOT)) / 2;
constexpr int MARGINY = (SCREEN_H - (SCALE * ROOT)) / 2;

using namespace std;

class App : public IComponent {
private:
	ALLEGRO_COLOR SQUARE_COLORS[BASE];
	unique_ptr<IPartridgeSolver> solver = IPartridgeSolver::newInstance(true);
public:
	void init() {
		srand(time(0));

		// init palette
		int hue_start = 270;
		int hue_delta = 360 / BASE;
		float s = 0.6;
		float v = 0.9;
		for (int i = 0; i < BASE; ++i) {
			ALLEGRO_COLOR col = al_color_hsv(
				((i * hue_delta) + hue_start) % 360, s, v 
			);
			SQUARE_COLORS[i] = col;
		}
	}

		// recursive descent approach
	void update() override {
		// TODO: adjust to speed of device
		for (int i = 0; i < 100000; ++i) {
			solver->step();
		}
	}

	void draw(const GraphicsContext &gc) override {
		al_clear_to_color(LIGHT_GREY);
		al_draw_rectangle(MARGINX, MARGINY, MARGINX + ROOT * SCALE, MARGINY + ROOT * SCALE, DARK_GREY, 1.0);
		for (const auto &sq : solver->getPlaced()) {
			al_draw_filled_rectangle(
				MARGINX + sq.getx() * SCALE, MARGINY + sq.gety() * SCALE,
				MARGINX + (sq.getx() + sq.msize) * SCALE - 1, MARGINY + (sq.gety() + sq.msize) * SCALE - 1,
				SQUARE_COLORS[sq.msize - 1]
			);
			al_draw_rectangle(
				MARGINX + sq.getx() * SCALE, MARGINY + sq.gety() * SCALE,
				MARGINX + (sq.getx() + sq.msize) * SCALE - 1, MARGINY + (sq.gety() + sq.msize) * SCALE - 1,
				BLACK, 1.0
			);
		}
	}

	bool quit = false;

	bool isAlive() const override {
		return !quit;
	}

	void handleEvent(ALLEGRO_EVENT &event) override {
		if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
			quit = true;
		}
	}

	virtual ~App() {}
};

#else

#include <iostream>
using namespace std;

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

void run_headless() {
	// Uncomment this for headless solver
	bool randomStart = false;
	auto solver = IPartridgeSolver::newInstance(randomStart);
	long counter = 0;
	Clock::time_point start = Clock::now();
	while (!solver->isDone()) {
		solver->step();
		if (counter % 1000000 == 0) {
			cout << counter << endl;
		}
		counter++;
	}
	Clock::time_point end = Clock::now();
	cout << "Solved in " << counter << " steps\n";
	auto d1 = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	cout << "Duration: " << d1.count() << " seconds" << endl;
	cout << (((float)counter / 1000000.0f) / (float)d1.count()) << " million permutations per second." << endl;
}

#endif

int main(int argc, const char *const *argv)
{
#ifdef HEADLESS
	run_headless();
#else
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 14")
		.setAppName("Genuary26.14")
		.setLogicIntervalMsec(20)
		.setPreferredDisplaySize(1280, 800);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
#endif
	return 0;
}
