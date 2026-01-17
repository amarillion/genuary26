#include "partridge.h"
#include <vector>
#include "map2d.h"
#include <random>
#include <algorithm>
#include <array>

using namespace std;

vector<int> makeStartPos(int randomStart) {
	vector<int> result;

	// initialize squares
	for (int i = BASE; i >= 1; --i) {
		for (int j = 0; j < i; ++j) {
			result.push_back(i);
		}
	}

	if (randomStart) {
		std::shuffle(result.begin(), result.end(), std::random_device());
	}
	return result;
}

class PartridgeSolver: public IPartridgeSolver {

private:
	vector<int> squares;
	vector<int> remain;
	vector<vector<int>> unused;
	vector<vector<int>>::iterator top;

	Map2D<bool> grid { ROOT, ROOT, false };
public:
	vector<Square> placed;

	PartridgeSolver(vector<int> startPos) {
		for(int i = 0; i < ROOT + 1; ++i) {
			vector<int> row;
			row.reserve(BASE);
			unused.push_back(row);
		}
		top = unused.begin();

		squares = startPos;

		remain = squares;
		nextRow();
	}

	bool isDone() override {
		return placed.size() == squares.size();
	}

	// recursive descent approach
	void step() {
		if (placed.size() == squares.size()) { return; } // DONE! 
		
		if (top->size() == 0) {
			// pop top from unused.
			assert(top > unused.begin());
			clearGrid(placed.back());
			remain.push_back(placed.back().msize);
			placed.pop_back();
			top--;
		} 
		else {
			auto next = top->back();
			top->pop_back();

			int mx, my;
			scanEmpty(grid, next, mx, my);

			if (mx >= 0 && my >= 0) {
				Square sq { mx, my, next };
				placed.push_back(sq);
				setGrid(sq);
				removeFirst(remain, sq.msize);
				top++;
				nextRow();

				if (placed.size() == squares.size()) { 
					// DONE! 
					printf("Found solution:\n");
					int i = 0;
					for (const auto &sq : placed) {
						i++;
						printf("- %02i square %i at (%i, %i)\n", i, sq.msize, sq.mx, sq.my);
					}
				}
			}
			else if (top->size() == 0) {
				// pop top from unused.
				assert(top > unused.begin());
				clearGrid(placed.back());
				remain.push_back(placed.back().msize);
				placed.pop_back();
				top--;
			}
		}
	}

private:
	void nextRow() {
		assert(top != unused.end());
		top->clear();
		// vector<int> new_row;
		// new_row.reserve(BASE);

		array<bool, BASE> seen;
		seen.fill(false);
		
		for(auto val : remain) {
			if (!seen[val-1]) {
				top->push_back(val);
				seen[val-1] = true;
			}
		}
		// because we pop from the back, to presevere order we reverse first.
		reverse(top->begin(), top->end());
		// unused.push_back(new_row);
	}

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
		if (grid.get(x, y)) {
			return false;
		}
		// scan top and left, since there are no placement gaps, that is enough
		for (int i = 1; i < size; ++i) {
			if (grid.get(x + i, y)) {
				return false;
			}
			if (grid.get(x, y + i)) {
				return false;
			}
		}
		return true;
	}

	void scanEmpty(const Map2D<bool> &grid, int size, int &rx, int &ry) {
		int x = placed.size() == 0 ? 0 : placed.back().mx;
		int y = placed.size() == 0 ? 0 : placed.back().my + placed.back().msize;
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
		// as an optimization, start scanning from rx, ry...
		int x = rx;
		int y = ry;
		for (; x < ROOT; ++x) {
			for (; y < ROOT; ++y) {
				if (!grid(x, y)) {
					rx = x;
					ry = y;
					return;
				}
			}
			y = 0;
		}
		rx = -1;
		ry = -1;
	}

	std::vector<Square> getPlaced() {
		return placed;
	}
};

std::unique_ptr<IPartridgeSolver> IPartridgeSolver::newInstance(bool randomStart) {
	return make_unique<PartridgeSolver>(makeStartPos(randomStart));
}

std::unique_ptr<IPartridgeSolver> IPartridgeSolver::newInstance(vector<int> start) {
	return make_unique<PartridgeSolver>(start);
}