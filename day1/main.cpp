#include "simpleloop.h"

using namespace std;
using namespace Simple;

class App : public IApp {

public:
	void init() {

	}

	void handleEvent(ALLEGRO_EVENT &evt) override {

	}

	bool update() override {
		return true;
	}

	void draw(const GraphicsContext &gc) override {

	}
};

int main(int argc, const char *const *argv)
{
	Simple::MainLoop mainloop = Simple::MainLoop();
	auto engine = make_shared<App>();

	mainloop
		.setFixedResolution(false)
		.setUsagiMode()
		.setTitle("Genuary26 Day 1")
		.setAppName("Genuary26.1")
		.setApp(engine);

	mainloop.setPreferredDisplayResolution(1024, 768);

	mainloop.init(argc, argv);
	engine->init();
	mainloop.run();
	return 0;
}
