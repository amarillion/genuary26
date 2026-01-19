#include "partridge.h"
#include <vector>
#include <random>
#include <algorithm>
#include <array>
#include <cassert>

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
	array<bool, ROOT * ROOT> grid;
public:
	vector<Square> placed;

	PartridgeSolver(vector<int> startPos) {
		fill(grid.begin(), grid.end(), false);

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
		
		if (top->size() != 0) {
			auto next = top->back();
			top->pop_back();

			int coord = scanEmpty(next);

			if (coord >= 0) {
				// Square sq { next, coord };
				const auto &nextSq = placed.emplace_back(next, coord);
				setGrid(nextSq);
				removeFirst(remain, next);
				top++;
				nextRow();

				if (placed.size() == squares.size()) { 
					// DONE! 
					printf("Found solution:\n");
					int i = 0;
					for (const auto &sq : placed) {
						i++;
						printf("- %02i square %i at (%i, %i)\n", i, sq.msize, sq.getx(), sq.gety());
					}
				}
				return;
			}
		}
		if (top->size() == 0) {
			// pop top from unused.
			assert(top > unused.begin());
			clearGrid(placed.back());
			remain.push_back(placed.back().msize);
			placed.pop_back();
			top--;
		}
	}

private:
	void nextRow() {
		assert(top != unused.end());
		top->clear();

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
	}

	void setGrid(const Square &sq, bool value = true) {
		int coord = sq.coord;
		for (int y = 0; y < sq.msize; ++y, coord += ROOT) {
			// std::fill will use memset if possible...
			std::fill(grid.begin() + coord, grid.begin() + coord + sq.msize, value);
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

	/**
	 * Check if a square of `size` can be placed at `coord`.
	 */
	bool isGridEmptyAt(int size, int coord) {
		if (grid[coord]) {
			return false;
		}
		// scan top and left, since there are no placement gaps, that is enough
		int hcoord = coord;
		int vcoord = coord;
		for (int i = 1; i < size; ++i) {
			hcoord += 1;
			if (grid[hcoord]) { return false; }
			vcoord += ROOT;
			if (grid[vcoord]) { return false; }
		}
		return true;
	}

	/** 
	 * Find the next empty cell in the grid, and check if a square of `size` fits there.
	 * If yes, return the coordinate as (x + y * ROOT).
	 * If not, return -1.
	 */
	int scanEmpty(int size) {
		int coord = placed.size() == 0 ? 0 : placed.back().coord + placed.back().msize;
		findEmptyCell(coord);
		if (coord >= 0) {
			int x = coord % ROOT;
			int y = coord / ROOT;
			if (x + size <= ROOT && y + size <= ROOT) {
				if (isGridEmptyAt(size, coord)) {
					return coord;
				}
			}
		}
		return -1;
	}

	/**
	 * Scan, starting at `coord`, for an empty cell.
	 * Scan rows first, columns second.
	 * Return -1 if no empty cell is found.
	 */
	void findEmptyCell(int &coord) {
		for (; coord < ROOT * ROOT; ++coord) {
			if (!grid[coord]) {
				return;
			}
		}
		coord = -1;
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