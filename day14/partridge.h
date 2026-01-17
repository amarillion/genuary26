#include <memory>
#include <vector>

struct Square {
	int mx;
	int my;	
	int msize;
};

constexpr int ROOT = 45;
constexpr int BASE = 9;

class IPartridgeSolver {
public:
	virtual void step() = 0;
	virtual bool isDone() = 0;
	virtual std::vector<Square> getPlaced() = 0;
	static std::unique_ptr<IPartridgeSolver> newInstance(bool randomStart);
	static std::unique_ptr<IPartridgeSolver> newInstance(std::vector<int> startPos);
};
