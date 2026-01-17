#include "mainloop.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "color.h"

using namespace std;

constexpr int SCREEN_W = 1280;
constexpr int SCREEN_H = 800;

constexpr int TILE_W = 256;
constexpr int TILE_H = 256;

struct TileOpt {
	float xofst;
	float yofst;
	int flags;
	float rotation;
};

struct WallpaperGroup {
	string name;
	float scale;
	vector<TileOpt> opt;
};

vector<WallpaperGroup> WALLPAPER_GROUPS = {
	{
		"p1",
		1, 
		{ 
			{ 0, 0, 0, 0 }
		}
	}, {
		"p2",
		1.5,
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL, 0 }
		}
	}, {
		"pg",
		1.5,
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, ALLEGRO_FLIP_HORIZONTAL, 0 }
		}
	}, {
		"cm",
		2,
		{
			{ 0, 0, 0, 0 },
			{ 0, 1, ALLEGRO_FLIP_HORIZONTAL, 0 },
			{ 1, 0, ALLEGRO_FLIP_HORIZONTAL, 0 },
			{ 1, 1, 0, 0 }
		}
	}, {
		"pmm",
		2,
		{
			{ 0, 0, 0, 0 },
			{ 1, 0, ALLEGRO_FLIP_HORIZONTAL, 0 },
			{ 0, 1, ALLEGRO_FLIP_VERTICAL, 0 },
			{ 1, 1, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL, 0 }
		}
	}, {
		"pmg",
		2,
		{
			{ 0, 0, 0, 0 },
			{ 1, 0, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL, 0 },
			{ 0, 1, ALLEGRO_FLIP_VERTICAL, 0 },
			{ 1, 1, ALLEGRO_FLIP_HORIZONTAL, 0 }
		}
	}, {
		"pgg", // TODO - not sure I did this one correctly
		2,
		{
			{ 0, -0.5, 0, 0 },
			{ 0, 0.5, ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL, 0 },
			{ 1, 0, ALLEGRO_FLIP_VERTICAL, 0 },
			{ 1, 1, ALLEGRO_FLIP_HORIZONTAL, 0 },
		}
	}, {
		"cmm", 
		2,
		{
			{ 0, -1, 0, 0 },
			{ 1, 0, ALLEGRO_FLIP_HORIZONTAL, 0.5 * M_PI },
			{ 0, 1, 0, M_PI },
			{ -1, 0, ALLEGRO_FLIP_HORIZONTAL, 1.5 * M_PI },
		}
	}, {
		"p4", 
		2,
		{
			{ 0, -1, 0, 0 },
			{ 1, 0, 0, 0.5 * M_PI },
			{ 0, 1, 0, M_PI },
			{ -1, 0, 0, 1.5 * M_PI },
		}
	},

};


class App : public IComponent {
public:

	int w, h;
	ALLEGRO_BITMAP *img;
	ALLEGRO_FONT *font;

	App() {
		w = SCREEN_W;
		h = SCREEN_H;

		img = al_load_bitmap("data/310893-small.png");
		assert(img);

		font = al_create_builtin_font();
	}

	void update() override {
		frame++;
		if (frame % 50 == 0) {
			currentGroup = (currentGroup + 1) % WALLPAPER_GROUPS.size();
		}
	}

	int currentGroup = WALLPAPER_GROUPS.size() - 1;
	int frame = 0;

	void draw(const GraphicsContext &gc) override {
		al_clear_to_color(WHITE);

		WallpaperGroup &current = WALLPAPER_GROUPS[currentGroup];

		al_draw_text(font, BLUE, 0, 0, 0, current.name.c_str());

		for (int y = 0; y < h; y += (TILE_H * current.scale)) {
			for (int x = 0; x < w; x += (TILE_W * current.scale)) {

				for (auto &i : current.opt) {
					if (i.rotation != 0) {
						al_draw_rotated_bitmap(img, 0, 0, x + (128 * current.scale) * i.xofst, y + (128 * current.scale) * i.yofst, i.rotation, i.flags);
					}
					else {
						al_draw_bitmap(img, x + (128 * current.scale) * i.xofst, y + (128 * current.scale) * i.yofst, i.flags);
					}
				}
			}
		}

	}

	virtual ~App() {
		if (img) {
			al_destroy_bitmap(img);
		}
		if (font) {
			al_destroy_font(font);
		}
	}
};

int main(int argc, const char *const *argv)
{
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 17")
		.setAppName("Genuary26.17")
		.setLogicIntervalMsec(200)
		.setPreferredDisplaySize(SCREEN_W, SCREEN_H);

	if (!mainloop.init(argc, argv)) {
		App app;
		mainloop.run(&app);
	}
	return 0;
}
