#pragma once

#include <memory>
#include "graphicscontext.h"
#include <allegro5/allegro.h>
#include "icomponent.h"

/**
 * A buffered viewport,
 * that stretches & letterboxes a lowres view onto a higher resolution display.
 * useful for low-resolution pixel art games.
 */
class PixelView: public IComponent {
private:
	std::shared_ptr<IComponent> childComponent;
	int w;
	int h;
	ALLEGRO_BITMAP *buffer;
public:
	PixelView(std::shared_ptr<IComponent> cc, 
		int _w, 
		int _h,
		bool maintainAspectRatio = false,
		bool integralScaling = false,
		bool useBuffer = true
	);
	virtual ~PixelView();
	virtual void draw(const GraphicsContext &gc) override;

	template <typename V>
	void adjustMickey(V &x, V &y) {
		x = x * w / al_get_display_width(al_get_current_display());
		y = y * h / al_get_display_height(al_get_current_display());
	}

	virtual void handleEvent(ALLEGRO_EVENT &event) override;
	virtual void update() override;
	virtual bool isAlive() const override;
};
