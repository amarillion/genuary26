#include <memory>
#include <vector>

constexpr int ROOT = 45;
constexpr int BASE = 9;

struct Square {
	int msize;
	int coord;
	int getx() const {
		return coord % ROOT;
	}
	int gety() const {
		return coord / ROOT;
	}
};

class IPartridgeSolver {
public:
	virtual void step() = 0;
	virtual bool isDone() = 0;
	virtual std::vector<Square> getPlaced() = 0;
	static std::unique_ptr<IPartridgeSolver> newInstance(bool randomStart);
	static std::unique_ptr<IPartridgeSolver> newInstance(std::vector<int> startPos);
};
