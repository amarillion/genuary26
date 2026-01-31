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

enum Edge {
	LEFT, RIGHT, VERTICAL
};

struct Triangle {
	Point pos;
	bool filled;
	set<Edge> links;
};

struct TriangularMap {
public:
	Map2D<Triangle> map;
	int base;

	TriangularMap(int base) : base(base) {
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

	vector<Point> getAdjacent(const Point &pos) {
		vector<Point> result;
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
		// printf("Point: %i, %i; rowLen: %i; result: %f, %f\n", p.x(), p.y(), rowLen, result.x(), result.y());
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
		}
		else {
			result = {
				top + Vec2f(-pxBase_2, 0), //TOP-LEFT
				top + Vec2f(pxBase_2, 0), // TOP-RIGHT
				top + Vec2f(0, pxHeight) // BOTTOM
			};
		}
		return result;
	}
};

class Day26 : public IComponent {
public:
	const int BASE = 4;
	const float SCALE = 768.0f;

	Vec2f origin = { (float)al_get_display_width(al_get_current_display()) / 2.0f, 40.0f };
	TriangularGrid grid { origin, BASE, SCALE / 4.0f };
	TriangularMap map { BASE };

	TriangularGrid grid2 { origin, BASE * 4, SCALE / 16.0f };
	TriangularMap map2 { BASE * 4 };

	TriangularGrid grid3 { origin, BASE * 16, SCALE / 64.0f };
	TriangularMap map3 { BASE * 16 };

	TriangularGrid grid4 { origin, BASE * 64, SCALE / 256.0f };
	TriangularMap map4 { BASE * 64 };

	void drawGrid(const TriangularGrid &grid, const TriangularMap & map, ALLEGRO_COLOR color) {
		for (int y = 0; y < map.base; ++y) {
			for (int x = 0; x < TriangularMap::rowLen(y); ++x) {
				auto triangle = grid.triangleOutline(Point(x, y));
				al_draw_triangle(
					triangle[0].x(), triangle[0].y(),
					triangle[1].x(), triangle[1].y(),
					triangle[2].x(), triangle[2].y(),
					color, 1.0
				);
			}
		}
	}

	void draw(const GraphicsContext &gc) override {
		// drawGrid(grid4, map4, GREY);
		// drawGrid(grid3, map3, CYAN);
		// drawGrid(grid2, map2, YELLOW);
		drawGrid(grid, map, RED);
	}

	virtual ~Day26() {}

	void update() override {
		// fill / remove random triangles...

		// periodically, copy to next grid and increase level
	}
};

int main(int argc, const char *const *argv) {
	
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 26")
		.setAppName("Genuary26.26")
		.setLogicIntervalMsec(17) // ~60fpx
		.setPreferredDisplaySize(1280, 800);

	if (!mainloop.init(argc, argv)) {
		auto app = make_shared<Day26>();
		mainloop.run(app.get());
	}
	return 0;
}
