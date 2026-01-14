#include "mainloop.h"

#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include "color.h"

#include <list>
#include "map2d.h"
#include <set>

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

ALLEGRO_COLOR SQUARE_COLORS[BASE];

class App : public IApp {
public:
	int frame;
	
	vector<int> squares;
	vector<int> remain;
	vector<Square> placed;
	
	Map2D<bool> grid { 45, 45, false };

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

	void nextRow() {
		// initialize a new row on unused.
		// in order of original squares array as the random seed...
		vector<int> new_row = remain;
		// printf("Minus placed: ");
		// for (int i : new_row) {
		// 	printf("%i,", i);
		// }
		// printf("\n");
		// remove duplicates to speed up the process
		removeDuplicates(new_row);
		// ///////
		// printf("New row: ");
		// for (int i : new_row) {
		// 	printf("%i,", i);
		// }
		// printf("\n");

		// because we pop from the back, to presever order we need to reverse here.
		reverse(new_row.begin(), new_row.end());
		unused.push_back(new_row);
	}

	// void shuffle() {
	//  // NOTE: random_shuffle not available in emscripten toolchain
	// 	random_shuffle(squares.begin(), squares.end());
	// }

	vector<vector<int>> unused;

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

	// recursive descent approach
	void update() override {
		
		// for (int i = 0; i < 100; ++i) {
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
				scanEmpty3(grid, next, mx, my);
				// scanEmpty2(grid, next, mx, my);
				// scanEmpty(placed, next.msize, mx, my);

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
		// }
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
		for (int x = 0; x < (ROOT - size + 1); ++x) {
			for (int y = 0; y < (ROOT - size + 1); ++y) {
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

	void scanEmpty3(const Map2D<bool> &grid, int size, int &rx, int &ry) {
		int x = -1;
		int y = -1;
		findGap0(grid, x, y);
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

	// Regular scan
	void findGap0(const Map2D<bool> &grid, int &rx, int &ry) {
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

	// Scan inL-shape
	void findGap(const Map2D<bool> &grid, int &rx, int &ry) {
		for (int i = 0; i < ROOT; ++i) {
			int x, y;
			y = i;
			for (x = 0; x <= i; ++x) {
				if (!grid(x, y)) {
					rx = x;
					ry = y;
					return;
				}
			}
			x = i;
			for (y = 0; y <= i; ++y) {
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

	/* scan based on linear scan of placed rectangles */
	void scanEmpty(const vector<Square> &placed, int size, int &rx, int &ry) {
		Square temp;
		temp.msize = size;
		for (int x = 0; x < (ROOT - temp.msize + 1); ++x) {
			for (int y = 0; y < (ROOT - temp.msize + 1); ++y) {
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
