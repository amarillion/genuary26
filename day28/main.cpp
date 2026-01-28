#include "mainloop.h"
#include <math.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include <list>
#include <cmath>
#include "pixelview.h"
#include "icomponent.h"
#include <sstream>
#include "xml.h"

using namespace std;

const string TEXT { R"(<h1>Genuary 2026, Day 28 - HTML Only</h1>
<p>Q: What is the <i>most complicated</i> way to show a link on a webpage?</p>
<p>I don't know for sure, but this page is certainly not simple.</p>
<p>This is not actually HTML. Instead, this snippet of text is laid out using a program written in 
<b>C++</b>, rendered using <a href="https://liballeg.org">Allegro</a>, 
 and compiled to WASM using <a href="https://emscripten.org">Emscripten</a>. 
Text is drawn using allegro font routines. Click detection is emulated inside the C++ program.</p>)" 
};

	void parseXml(std::istream&& stream) {
	try {
		XmlParser parser(move(stream));
		
		while (parser.next() != XmlParser::END_DOCUMENT) {
			switch (parser.getEventType()) {
				case XmlParser::START_TAG: {
					std::cout << "Start Tag: " << parser.getName() << std::endl;
					auto attrs = parser.getAttributes();
					for (const auto& attr : attrs) {
						std::cout << "  Attribute: " << attr.first 
								<< " = \"" << attr.second << "\"" << std::endl;
					}
					break;
				}
				case XmlParser::END_TAG:
					std::cout << "End Tag: " << parser.getName() << std::endl;
					break;
				case XmlParser::TEXT: {
					std::string text = parser.getText();
					if (!text.empty()) {
						std::cout << "Text: \"" << text << "\"" << std::endl;
					}
					break;
				}
				case XmlParser::START_DOCUMENT:
					std::cout << "Start Document" << std::endl;
					break;
				case XmlParser::ERROR:
					std::cout << "Error at line " << parser.getLine() 
							<< ", column " << parser.getColumn() << std::endl;
					break;
			}
		}
		std::cout << "End Document" << std::endl;
		
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

class Day28 : public IComponent {
public:
	void draw(const GraphicsContext &gc) override {
		
	}

	virtual ~Day28() {}

	void update() override {}
};

int main(int argc, const char *const *argv) {
	
	// Parse the sample XML
	parseXml(std::stringstream(TEXT));
	
	return 0;
/*
	MainLoop mainloop;

	mainloop
		.setTitle("Genuary26 Day 28")
		.setAppName("Genuary26.28")
		.setLogicIntervalMsec(17) // ~60fpx
		.setPreferredDisplaySize(1024, 768);

	if (!mainloop.init(argc, argv)) {
		auto app = make_shared<Day28>();
		mainloop.run(app.get());
	}
	return 0;
*/
}
