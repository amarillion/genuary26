#include "mainloop.h"

#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"

#include <list>
#include "map2d.h"
#include <set>

using namespace std;

struct Square {
	int mx;
	int my;	
	int msize;
};

constexpr int ROOT = 45;
constexpr int BASE = 9;

constexpr int SCREEN_W = 1280;
constexpr int SCREEN_H = 800;
constexpr int SCALE = SCREEN_H / ROOT;
constexpr int MARGINX = (SCREEN_W - (SCALE * ROOT)) / 2;
constexpr int MARGINY = (SCREEN_H - (SCALE * ROOT)) / 2;

// ordered first from top to bottom, then from left to right:
vector<int> solution {
	8, 5, 7, 7, 9, 9,
	3, 2,
	1, 7, 8,
	6, 6, 9, 9,
	5, 5, 9,
	8, 9, 9,
	6, 4,
	7, 6, 4, 4,
	6,
	2, 7, 8, 9,
	8, 
	7,
	6, 
	3, 4,
	3, 5,
	9,
	8, 8, 8,
	7, 
	5,
};

// starting from a partial solution to give the simulation a chance to reach the end
vector<int> solution_almost {
	8, 5, 7, 7, 9, 9,
	3, 2,
	1, 7, 8,
	6, 6, 9, 9,
	5, 5, 9,
	8, 9, 9,
	6, 4,
	7, 6, 4, 4,
	6,
	2, 7, 8, 9,
	8,
	7,
	3, 3, 4, 5, 5, 6, 7, 8, 8, 8, 9
};

vector<int> squares;
vector<int> remain;
vector<vector<int>> unused;
vector<Square> placed;
Map2D<bool> grid { 45, 45, false };

class PartridgeSolver {
private:
public:
	PartridgeSolver() {
		// initialize squares
		for (int i = 1; i <= BASE; ++i) {
			for (int j = 0; j < i; ++j) {
				squares.push_back(i);
			}
		}

		// shuffle();
		squares = solution_almost;

		remain = squares;
		nextRow();
	}

	// recursive descent approach
	void step() {
		if (placed.size() == squares.size()) { return; } // DONE! 
		
		// pop top from unused.
		assert(unused.size() > 0);

		auto &top_row = unused.back();
		if (top_row.size() == 0) {
			clearGrid(placed.back());
			remain.push_back(placed.back().msize);
			placed.pop_back();
			unused.pop_back();
		} 
		else {
			auto next = top_row.back();

			top_row.pop_back();

			int mx, my;
			scanEmpty(grid, next, mx, my);

			if (mx >= 0 && my >= 0) {
				Square sq { mx, my, next };
				placed.push_back(sq);
				setGrid(sq);
				removeFirst(remain, sq.msize);
				nextRow();
			}
			else {
				if (top_row.size() == 0) {
					clearGrid(placed.back());
					remain.push_back(placed.back().msize);
					placed.pop_back();
					unused.pop_back();
				}
			}
		}
	}

private:
	void nextRow() {
		// initialize a new row on unused.
		// in order of original squares array as the random seed...
		vector<int> new_row = remain;
		// remove duplicates, there are many squares interchangeable
		removeDuplicates(new_row);

		// because we pop from the back, to presevere order we reverse first.
		reverse(new_row.begin(), new_row.end());
		unused.push_back(new_row);
	}

	// void shuffle() {
	//  // NOTE: random_shuffle not available in emscripten toolchain
	// 	random_shuffle(squares.begin(), squares.end());
	// }

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
	
	void removeFirst(vector<int> &vec, int needle) {
		auto it = find(vec.begin(), vec.end(), needle);
		assert(it != vec.end());
		vec.erase(it);
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

	void scanEmpty(const Map2D<bool> &grid, int size, int &rx, int &ry) {
		int x = -1;
		int y = -1;
		findEmptyCell(grid, x, y);
		if (x >= 0 && y >= 0) {
			if (x + size <= ROOT && y + size <= ROOT) {
				if (isGridEmptyAt(grid, size, x, y)) {
					rx = x;
					ry = y;
					return;
				}
			}
		}
		rx = -1;
		ry = -1;
	}

	void findEmptyCell(const Map2D<bool> &grid, int &rx, int &ry) {
		for (int x = 0; x < ROOT; ++x) {
			for (int y = 0; y < ROOT; ++y) {
				if (!grid(x, y)) {
					rx = x;
					ry = y;
					return;
				}
			}
		}
		rx = -1;
		ry = -1;
	}

	void removeDuplicates(vector<int>& myVector) {
		set<int> seen;

		// Using remove_if to eliminate duplicates and get the
		// new end iterator
		auto newEnd = remove_if(
			myVector.begin(), myVector.end(),
			[&seen](int& value) {
				// Checking if value has been seen; if not, add
				// to seen and keep in vector
				if (seen.find(value) == seen.end()) {
					seen.insert(value);
					return false; // Don't remove the item
				}
				return true; // Remove the item
			});

		// Erase the non-unique elements
		myVector.erase(newEnd, myVector.end());
	}

};

class App : public IApp {
private:
	ALLEGRO_COLOR SQUARE_COLORS[BASE];
	PartridgeSolver solver;
public:
	void init() {
		srand(time(0));

		// init palette
		int hue_start = 270;
		int hue_delta = 360 / BASE;
		float s = 0.6;
		float v = 0.9;
		for (int i = 0; i <= BASE; ++i) {
			ALLEGRO_COLOR col = al_color_hsv(
				((i * hue_delta) + hue_start) % 360, s, v 
			);
			SQUARE_COLORS[i] = col;
		}
	}

		// recursive descent approach
	void update() override {
		solver.step();
	}

	void draw(const GraphicsContext &gc) override {
		al_clear_to_color(LIGHT_GREY);
		al_draw_rectangle(MARGINX, MARGINY, MARGINX + ROOT * SCALE, MARGINY + ROOT * SCALE, DARK_GREY, 1.0);
		for (const auto &sq : placed) {
			al_draw_filled_rectangle(
				MARGINX + sq.mx * SCALE, MARGINY + sq.my * SCALE,
				MARGINX + (sq.mx + sq.msize) * SCALE - 1, MARGINY + (sq.my + sq.msize) * SCALE - 1,
				SQUARE_COLORS[sq.msize - 1]
			);
			al_draw_rectangle(
				MARGINX + sq.mx * SCALE, MARGINY + sq.my * SCALE,
				MARGINX + (sq.mx + sq.msize) * SCALE - 1, MARGINY + (sq.my + sq.msize) * SCALE - 1,
				BLACK, 1.0
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
