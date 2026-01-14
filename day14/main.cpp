#include "mainloop.h"

#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"

#include <list>
#include "map2d.h"

using namespace std;

int randomInt(int max) {
	return rand() % max; 
}

float randomFloat(float max = 1.0) {
	return max * (float)(rand()) / (float)RAND_MAX;
}

struct Square {
	int mx;
	int my;	
	int msize;

	bool overlaps(const Square &other) {
		return !(
			(mx >= other.mx + other.msize) || 
			(other.mx >= mx + msize) || 
			(my >= other.my + other.msize) || 
			(other.my >= my + msize)
		);
	}

	int overlapSize(const Square &other) {
		int overlapX = min(mx + msize, other.mx + other.msize) - max(mx, other.mx);
		int overlapY = min(my + msize, other.my + other.msize) - max(my, other.my);
		int total = max(0, overlapX) * max(0, overlapY);
		// if (total > 0) {
		// 	printf("Rect %i,%i %ix%i overlaps with %i,%i %ix%i overlapX=%i overlapY=%i total=%i\n", 
		// 		mx, my, msize, msize, other.mx, other.my, other.msize, other.msize, overlapX, overlapY, total);
		// }
		return total;
	}
};

inline bool operator <(const Square& lhs, const Square& rhs)
{
	return lhs.msize > rhs.msize; 
}

constexpr int ROOT = 45;
constexpr int BASE = 9;

constexpr int SCREEN_W = 1280;
constexpr int SCREEN_H = 800;
constexpr int SCALE = SCREEN_H / 45;
constexpr int MARGINX = (SCREEN_W - (SCALE * ROOT)) / 2;
constexpr int MARGINY = (SCREEN_H - (SCALE * ROOT)) / 2;

ALLEGRO_COLOR SQUARE_COLORS[BASE];

class App : public IApp {
public:
	int frame;
	
	vector<Square> squares;
	vector<Square> placed;
	
	Map2D<bool> grid { 45, 45, false };

	void init() {
		srand(time(0));

		// init palette
		int hue_start = randomInt(360);
		int hue_delta = 360 / BASE;
		float s = randomFloat(0.5) + 0.5;
		float v = randomFloat(0.5) + 0.5;
		for (int i = 0; i <= BASE; ++i) {
			ALLEGRO_COLOR col = al_color_hsv(
				((i * hue_delta) + hue_start) % 360, s, v 
			);
			SQUARE_COLORS[i] = col;
		}

		// initialize squares

		for (int i = 1; i <= BASE; ++i) {
			for (int j = 0; j < i; ++j) {
				Square sq {
					randomInt(ROOT - i),
					randomInt(ROOT - i),
					i
				};
				squares.push_back(sq);
			}
		}

		// for (auto &sq : squares) {
		// 	printf("%i, %i size %i\n", sq.mx, sq.my, sq.msize);
		// }

		shuffle();
		
		for (int i = 0; i < ROOT; ++i) {
			counter[ROOT] = 0;
		}

		// initialize unused
		unused.push_back(squares);
	}

	int current = 0;

	void shuffle() {
		random_shuffle(squares.begin(), squares.end());
	}

	int counter[ROOT];

	vector<vector<Square>> unused;

	void setGrid(const Square &sq, bool value = true) {
		for (int x = 0; x < sq.msize; ++x) {
			for (int y = 0; y < sq.msize; ++y) {
				grid(x + sq.mx, y + sq.my) = value;
			}
		}
	}

	void clearGrid(const Square &sq) {
		setGrid(sq, false);
	}
	
	// recursive descent approach
	void update() override {
		
		// TODO:
		// check for existence of 2x1 gaps and discard...

		for (int i = 0; i < 2000; ++i) {
			if (placed.size() == squares.size()) { return; } // DONE! 
			
			// pop top from unused.
			assert(unused.size() > 0);

			auto &top_row = unused.back();
			if (top_row.size() == 0) {
				clearGrid(placed.back());
				placed.pop_back();
				unused.pop_back();
			} 
			else {

				auto next = top_row.back();
				top_row.pop_back();

				int mx, my;
				scanEmpty2(grid, next.msize, mx, my);
				// scanEmpty(placed, next.msize, mx, my);

				if (mx < 0 || my < 0) {
					clearGrid(placed.back());
					placed.pop_back();
					unused.pop_back();
				}
				else {
					unused.push_back(top_row);
					next.mx = mx;
					next.my = my;
					placed.push_back(next);
					setGrid(next);
				}
			}
		}
	}

	bool isGridEmptyAt(const Map2D<bool> &grid, int size, int x, int y) {
		bool isEmpty = true;
		// scan top-left corner first
		if (grid.get(x, y)) {
			return false;
		}
		// if square of one, we're already done.
		if (size == 1) {
			return true;
		}
		// scan other three corners
		if (grid.get(x + size - 1, y + size - 1)) {
			return false;
		}
		if (grid.get(x, y + size - 1)) {
			return false;
		}
		if (grid.get(x + size - 1, y)) {
			return false;
		}
		// scan remaining points to be sure.
		for (int xx = 0; xx < size; ++xx) {
			for (int yy = 0; yy < size; ++yy) {
				if (grid.get(x + xx, y + yy)) {
					return false;
				}
			}
		}
		return true;
	}

	/* scan based on up-to-date grid, should be faster */
	void scanEmpty2(const Map2D<bool> &grid, int size, int &rx, int &ry) {
		for (int x = 0; x < (ROOT - size); ++x) {
			for (int y = 0; y < (ROOT - size); ++y) {
				bool isEmpty = isGridEmptyAt(grid, size, x, y);
				if (isEmpty) {
					rx = x;
					ry = y;
					return;
				}
			}
		}
		rx = -1;
		ry = -1;
	}

	/* scan based on linear scan of placed rectangles */
	void scanEmpty(const vector<Square> &placed, int size, int &rx, int &ry) {
		Square temp;
		temp.msize = size;
		for (int x = 0; x < (ROOT - temp.msize); ++x) {
			for (int y = 0; y < (ROOT - temp.msize); ++y) {
				temp.mx = x;
				temp.my = y;
				bool isEmpty = true;
				for (const auto &sq : placed) {
					if (temp.overlaps(sq)) {
						isEmpty = false;
						break;
					}
				}
				if (isEmpty) {
					rx = x;
					ry = y;
					return;
				}
			}
		}
		rx = -1;
		ry = -1;
	}

	void draw(const GraphicsContext &gc) override {
		al_clear_to_color(LIGHT_GREY);
		for (const auto &sq : placed) {
			al_draw_filled_rectangle(
				MARGINX + sq.mx * SCALE, MARGINY + sq.my * SCALE,
				MARGINX + (sq.mx + sq.msize) * SCALE - 1, MARGINY + (sq.my + sq.msize) * SCALE - 1,
				SQUARE_COLORS[sq.msize - 1]
			);
		}

		// draw grid...
		for (int x = 0; x < ROOT; ++x) {
			for (int y = 0; y < ROOT; ++y) {
				if (grid.get(x, y)) {
					al_draw_filled_rectangle(
						MARGINX + x * SCALE, MARGINY + y * SCALE,
						MARGINX + x * SCALE + 4, MARGINY + y * SCALE + 4,
						BLACK
					);
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
		.setTitle("Genuary26 Day 14")
		.setAppName("Genuary26.14")
		.setLogicIntervalMsec(20)
		.setPreferredDisplaySize(1280, 800);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
