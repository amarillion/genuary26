#pragma once

#include <memory>
#include "graphicscontext.h"
#include <allegro5/allegro.h>

class IComponent {
public:
	virtual void draw(const GraphicsContext &gc) = 0;
	virtual void handleEvent(ALLEGRO_EVENT &evt) {}
};

// TODO: inherit from Component...
class PixelBuffer: public IComponent {
private:
	std::shared_ptr<IComponent> childComponent;
	int w;
	int h;
	ALLEGRO_BITMAP *buffer;
public:
	PixelBuffer(std::shared_ptr<IComponent> cc, 
		int _w, 
		int _h,
		bool maintainAspectRatio = false,
		bool integralScaling = false,
		bool useBuffer = true
	);
	virtual ~PixelBuffer();
	virtual void draw(const GraphicsContext &gc) override;

	template <typename V>
	void adjustMickey(V &x, V &y) {
		x = x * w / al_get_display_width(al_get_current_display());
		y = y * h / al_get_display_height(al_get_current_display());
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override;	
};
