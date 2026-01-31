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

class Day26 : public IComponent {
public:
	const int BASE = 4;
	const float SCALE = 1024.0f;
	Vec2f origin = { (float)al_get_display_width(al_get_current_display()) / 2.0f, 40.0f };
	
	TriangularGrid grid[4] {
		{ origin, BASE, SCALE / 4.0f },
		{ origin, BASE * 4, SCALE / 16.0f },
		{ origin, BASE * 16, SCALE / 64.0f },
		{ origin, BASE * 64, SCALE / 256.0f },
	};
	
	TriangularMap map[4] {
		{ BASE },
		{ BASE * 4 },
		{ BASE * 16 },
		{ BASE * 64 }
	};

	int currentMap = 0;

	void drawGrid(const TriangularGrid &grid, const TriangularMap & map, ALLEGRO_COLOR color) {
		for (int y = 0; y < map.base; ++y) {
			for (int x = 0; x < TriangularMap::rowLen(y); ++x) {
				bool filled = map.map.get(x, y).filled;
				if (filled) {
					auto triangle = grid.triangleOutline(Point(x, y));
					al_draw_filled_triangle(
						triangle[0].x(), triangle[0].y(),
						triangle[1].x(), triangle[1].y(),
						triangle[2].x(), triangle[2].y(),
						color
					);
				}
			}
		}
	}

	void draw(const GraphicsContext &gc) override {
		al_clear_to_color(BLACK);
		drawGrid(grid[currentMap], map[currentMap], WHITE);
	}

	virtual ~Day26() {}

	int frame = 0;
	void update() override {
		
		if (frame % ((4-currentMap) * 5) == 1) {
			int y = randomInt(map[currentMap].base);
			int x = randomInt(TriangularMap::rowLen(y));
		
			Triangle &t = map[currentMap].map.get(x, y);
			t.filled = !t.filled;
		}

		// fill / remove random triangles...
		frame++;
		// periodically, copy to next grid and increase level
		if (frame % ((currentMap + 1) * 500) == 0) {
			if (currentMap < 3) {
				frame = 0;
				copyAndScale(map[currentMap], map[currentMap+1]);
				currentMap++;
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
