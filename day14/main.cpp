#include "mainloop.h"

#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"

#include <list>

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
	
	// Map2D<bool> grid { 45, 45, false };

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

	void setGrid(const Square &sq, bool value) {

	}
	
	// recursive descent approach
	void update() override {
		
		// TODO:
		// check for existence of 2x1 gaps and discard...

		for (int i = 0; i < 250; ++i) {
			if (placed.size() == squares.size()) { return; } // DONE! 
			
			// pop top from unused.
			assert(unused.size() > 0);

			auto &top_row = unused.back();
			if (top_row.size() == 0) {
				placed.pop_back();
				unused.pop_back();
			} 
			else {

				auto next = top_row.back();
				top_row.pop_back();

				int mx, my;
				scanEmpty(placed, next.msize, mx, my);

				if (mx < 0 || my < 0) {
					placed.pop_back();
					unused.pop_back();
				}
				else {
					unused.push_back(top_row);
					next.mx = mx;
					next.my = my;
					placed.push_back(next);
				}
			}
		}
	}

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

	void update2() {
		// check if current is overlapping
		
		// find an overlapping pair
		bool found = false;
		for (int i = 0; i < squares.size(); ++i) {
			
			for (int j = 0; j < squares.size(); ++j) {
				if (j == current) continue;
				if (squares[j].overlaps(squares[current])) {
					found = true;
					break;
				}
			}

			if (found) {
				break;
			}
			current = (current + 1) % squares.size();
		}

		if (!found) {
			// solved!
			return;
		}

		found = false;
		int rx = squares[current].mx;
		int ry = squares[current].my;
		
		int bestx = 0;
		int besty = 0;
		int minOverlap = 2000000000;
		// find an empty space for i one of the two
		for (int x = 0; x < (ROOT - squares[current].msize); ++x) {
			for (int y = 0; y < (ROOT - squares[current].msize); ++y) {
				squares[current].mx = x;
				squares[current].my = y;
				int totalOverlap = 0;
				for (int j = 0; j < squares.size(); ++j) {
					if (current == j) continue;
					totalOverlap += squares[current].overlapSize(squares[j]);
				}
				if (totalOverlap < minOverlap) {
					minOverlap = totalOverlap;
					bestx = x;
					besty = y;
				}
				// if (totalOverlap = 0) {
				// 	found = true;
				// 	break;
				// }
			}
			// if (found) { break; }
		}

		squares[current].mx = bestx;
		squares[current].my = besty;

		if (bestx == rx && besty == ry) {
			// if space can't be found, make space by relocating other squares to random positions.
			for (int j = 0; j < squares.size(); ++j) {
				if (j == current) continue;
				if (squares[j].overlaps(squares[current])) {
					squares[j].mx = randomInt(ROOT - squares[j].msize);
					squares[j].my = randomInt(ROOT - squares[j].msize);
					break;
				}
			}
		}
		
		// advance
		current = (current + 1) % squares.size();
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
	}

	virtual ~App() {}
};

int main(int argc, const char *const *argv)
{
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 14")
		.setAppName("Genuary26.14")
		.setLogicIntervalMsec(50)
		.setPreferredDisplaySize(1280, 800);

	if (!mainloop.init(argc, argv)) {
		App app;
		app.init();
		mainloop.run(&app);
	}
	return 0;
}
