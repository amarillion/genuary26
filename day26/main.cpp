#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "pixelview.h"
#include "icomponent.h"
#include <set>
#include "map2d.h"
#include "color.h"

using namespace std;

int randomInt(int val) {
	return rand() % val;
}

enum Edge {
	LEFT, RIGHT, VERTICAL
};

struct Triangle {
	Point pos;
	bool filled;
	bool visited;
	set<Edge> links;
};

struct TriangularMap {
public:
	int base;
	Map2D<Triangle> map;
	
	TriangularMap(int base) : base(base), map(base * 2, base) {
	}

	bool isValid(const Point &pos) {
		return 
			pos.y() >= 0 &&
			pos.x() >= 0 &&
			pos.y() < base &&
			pos.x() < rowLen(pos.y());
	}

	static int rowLen(int row) {
		return 1 + row * 2;
	}

	static bool isPointingUp(const Point &pos) {
		return pos.x() % 2 == 0;
	}

	vector<tuple<Edge, Point>> getAdjacent(const Point &src) {
		vector<tuple<Edge, Point>> result;
		for (Edge e : { Edge::LEFT, Edge::RIGHT, Edge::VERTICAL }) {
			Point dest = getNeighbor(src, e);
			if (isValid(dest)) {
				if (!map[dest].filled) {
					result.push_back(tuple<Edge, Point>(e, dest));
				}
			}
		}
		return result;
	}

	Point getNeighbor(const Point &src, Edge e) {
		switch(e) {
			case LEFT: return Point(src.x() - 1, src.y());
			case VERTICAL: 
				if (isPointingUp(src)) {
					return Point(src.x() + 1, src.y() + 1);
				}
				else {
					return Point(src.x() - 1, src.y() - 1);
				}
			case RIGHT: return Point(src.x() + 1, src.y());
		}
		assert(false);
	}

	void link(const Point &src, Edge e, const Point &dest) {
		assert(getNeighbor(src, e) == dest);

		auto &tSrc = map[src];
		auto &tDest = map[dest];

		Edge reverse;
		switch(e) {
			case LEFT: reverse = RIGHT; break;
			case RIGHT: reverse = LEFT; break;
			case VERTICAL: reverse = VERTICAL; break; 
		}

		tSrc.visited = true;
		tDest.visited = true;

		tSrc.links.insert(e);
		tDest.links.insert(reverse);
	}

	Point randomCell() {
		//TODO: NOT uniformly distributed
		Point result;
		do {
			int y = randomInt(base);
			int x = randomInt(rowLen(y));
			result = Point(x, y);
		} while (map[result].filled);
		return result;
	}
};

struct TriangularGrid {

	Vec2f origin;
	int base;
	float pxHeight;
	float pxBase;
	float pxBase_2;

	TriangularGrid(Vec2f origin, int base, float pxBase): origin(origin), base(base), pxBase(pxBase) {
		pxHeight = sqrt((double)(pxBase * pxBase) * 0.75);
		pxBase_2 = pxBase / 2.0;
	}

	Vec2f toPixelCoord(const Point &p) const {
		// return TOP of triangle
		int rowLen = TriangularMap::rowLen(p.y());
		auto result = Vec2f(
			(float)((p.x() * 2.0) - rowLen + 1.0) * pxBase / 4.0f,
			(float)(p.y()) * pxHeight
		);
		result += origin;
		return result;
	}
	array<Vec2f, 3> triangleOutline(Point p) const {
		array<Vec2f, 3> result;
		Vec2f top = toPixelCoord(p);
		if (TriangularMap::isPointingUp(p)) {
			result = {
				top, //TOP
				top + Vec2f(pxBase_2, pxHeight), // BOTTOM-RIGHT
				top + Vec2f(-pxBase_2, pxHeight) // BOTTOM-LEFT
			};
			// order: right, vertical, left
		}
		else {
			result = {
				top + Vec2f(-pxBase_2, 0), //TOP-LEFT
				top + Vec2f(pxBase_2, 0), // TOP-RIGHT
				top + Vec2f(0, pxHeight) // BOTTOM
			};
			// order: vertical, right, left
		}
		return result;
	}
};

/**
 * Step-wise maze generator using recursive backtracking
 */
template<typename N, typename E>
class RecursiveBacktracker {
	typedef function<vector<tuple<E, N>>(N)> AdjacencyFunc;
	typedef function<void(N, E, N)> LinkFunc;

	AdjacencyFunc getAdjacent;
	LinkFunc link;
	vector<N> open;
	set<N> visited;

public:
	RecursiveBacktracker(N start, AdjacencyFunc getAdjacent, LinkFunc link): getAdjacent(getAdjacent), link(link) {
		open.push_back(start);
	}

	void step() {
		while(true) {
			if (open.empty()) return;

			N current = open.back();
			
			vector<tuple<E, N>> unvisitedAdjacents;
			for (const auto &i : getAdjacent(current)) {
				if (visited.contains(get<1>(i))) continue;
				unvisitedAdjacents.push_back(i);
			}

			if (unvisitedAdjacents.empty()) {
				open.pop_back();
			}
			else {
				int idx = randomInt(unvisitedAdjacents.size());
				auto &chosen = unvisitedAdjacents[idx]; 
				N &dest = get<1>(chosen);
				E &dir = get<0>(chosen); 
				open.push_back(dest);
				visited.insert(dest);
				link(current, dir, dest);
				return;
			}
		}
	}

	bool isDone() {
		return open.empty();
	}
};

void copyAndScale(const TriangularMap &src, TriangularMap &dest) {
	assert(dest.base % src.base == 0);
	int scale = dest.base / src.base;
	int scale2 = scale * 2;
	for (int dy = 0; dy < dest.base; ++dy) {
		for (int dx = 0; dx < TriangularMap::rowLen(dy); ++dx) {
			int sy = dy / scale;
			int sx = dx / scale; sx -= (sx % 2); // round down to nearest even number
			bool diagonal = (dx % scale2) > ((dy % scale) * 2);
			if (diagonal) sx++;
			const Triangle &st = src.map.get(sx, sy);
			Triangle &dt = dest.map.get(dx, dy);
			dt.filled = st.filled;
		}
	}
}

void drawCell(const Point &src, TriangularMap &srcMap, TriangularMap &destMap) {
	assert(destMap.base % srcMap.base == 0);
	int scale = destMap.base / srcMap.base;
	const Triangle &tSrc = srcMap.map[src];

	if (TriangularMap::isPointingUp(src)) {
		Point top = Point(src.x() * scale, src.y() * scale);
		for (int y = 0; y < scale; ++y) {
			for (int x = 0; x < TriangularMap::rowLen(y); ++x) {
				bool filled = false;
				if (x < 2 && !tSrc.links.contains(Edge::LEFT)) {
					filled = true;
				}
				if (y == scale-1 && !tSrc.links.contains(Edge::VERTICAL)) { filled = true; }
				if ((TriangularMap::rowLen(y) - x) <= 2 && !tSrc.links.contains(Edge::RIGHT)) {
					filled = true;
				}
				// filled = true;
				// corners always filled
				if (x == 0 && y == 0) { filled = true; }
				if (x == TriangularMap::rowLen(y)-1 && y == scale-1) { filled = true; }
				if (x == 0 && y == scale-1) { filled = true; }
				Point dest = top + Point(x, y);
				destMap.map[dest].filled = filled;
			}
		}
	}
	else {
		Point top = Point((src.x() + 1) * scale - 1, (src.y() + 1) * scale - 1);
		for (int y = 0; y < scale; ++y) {
			for (int x = 0; x < TriangularMap::rowLen(y); ++x) {
				bool filled = false;
				if (x < 2 && !tSrc.links.contains(Edge::RIGHT)) {
					filled = true;
				}
				if (y == scale-1 && !tSrc.links.contains(Edge::VERTICAL)) { filled = true; }
				if ((TriangularMap::rowLen(y) - x) <= 2 && !tSrc.links.contains(Edge::LEFT)) {
					filled = true;
				}
				// corners always filled
				if (x == 0 && y == 0) { filled = true; }
				if (x == TriangularMap::rowLen(y)-1 && y == scale-1) { filled = true; }
				if (x == 0 && y == scale-1) { filled = true; }
				
				Point dest = top + Point(-x, -y);
				destMap.map[dest].filled = filled;
			}
		}

	}
}

class Day26 : public IComponent {
public:
	const int BASE = 10;
	const float SCALE = 1024.0f;
	Vec2f origin = { (float)al_get_display_width(al_get_current_display()) / 2.0f, 40.0f };
	
	TriangularGrid grid[4] {
		{ origin, 1, SCALE },
		{ origin, BASE, SCALE / 10.0f },
		{ origin, BASE * 8, SCALE / 80.0f },
		{ origin, BASE * 32, SCALE / 320.0f },
		// { origin, BASE * 64, SCALE / 512.0f },
	};
	
	TriangularMap map[4] {
		{ 1 },
		{ BASE },
		{ BASE * 8 },
		{ BASE * 32 },
		// { BASE * 64 }
	};

	int currentMap = 1;

	void drawGrid(const TriangularGrid &grid, const TriangularMap & map, ALLEGRO_COLOR color) {
		for (int y = 0; y < map.base; ++y) {
			for (int x = 0; x < TriangularMap::rowLen(y); ++x) {
				const auto &t = map.map.get(x, y);
				Point pos { x, y };
				auto tCoords = grid.triangleOutline(pos);
				if (t.filled) {
					al_draw_filled_triangle(
						tCoords[0].x(), tCoords[0].y(),
						tCoords[1].x(), tCoords[1].y(),
						tCoords[2].x(), tCoords[2].y(),
						color
					);
				}
				else if (t.visited) {
					array<Edge, 3> POINTING_UP_ORDER { Edge::RIGHT, Edge::VERTICAL, Edge::LEFT };
					array<Edge, 3> POINTING_DOWN_ORDER { Edge::VERTICAL, Edge::RIGHT, Edge::LEFT };
					
					for (int i = 0; i < 3; ++i) {
						Edge currentEdge = TriangularMap::isPointingUp(pos) ? POINTING_UP_ORDER[i] : POINTING_DOWN_ORDER[i];
						if (!t.links.contains(currentEdge)) {
							al_draw_line(
								tCoords[i].x(), tCoords[i].y(),
								tCoords[(i + 1) % 3].x(), tCoords[(i + 1) % 3].y(),
								color, 1.0
							);
						}
					}
				}
			}
		}
	}

	void draw(const GraphicsContext &gc) override {
		al_clear_to_color(BLACK);
		drawGrid(grid[currentMap], map[currentMap], WHITE);
	}

	virtual ~Day26() {}

	unique_ptr<RecursiveBacktracker<Point, Edge>> generator = nullptr;

	int frame = 0;
	void update() override {
		if (++frame < 200) {
			return;
		}

		if (!generator) {
			auto getAdjacent = [=, this](Point node){
				return map[currentMap-1].getAdjacent(node);
			};
			auto link = [=, this](Point src, Edge e, Point dest){
				map[currentMap-1].link(src, e, dest);
				drawCell(src, map[currentMap-1], map[currentMap]);
				drawCell(dest, map[currentMap-1], map[currentMap]);
			};
			Point start = map[currentMap-1].randomCell();
			generator = make_unique<RecursiveBacktracker<Point, Edge>>( start, getAdjacent, link );
		}
		
		if (generator) {
			if (currentMap > 1) {
				generator->step(); 
				generator->step(); 
			}
			generator->step(); 

		}

		// periodically, copy to next grid and increase level
		
		if (generator->isDone()) {
			if (currentMap < 3) {
				copyAndScale(map[currentMap], map[currentMap+1]);
				currentMap++;
				generator = nullptr; // triggers re-initialisation
			}
		}

	}
};

int main(int argc, const char *const *argv) {
	
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 26")
		.setAppName("Genuary26.26")
		.setLogicIntervalMsec(17) // ~60fpx
		.setPreferredDisplaySize(1280, 1080);

	if (!mainloop.init(argc, argv)) {
		auto app = make_shared<Day26>();
		mainloop.run(app.get());
	}
	return 0;
}
