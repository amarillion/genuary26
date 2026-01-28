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
	int destw;
	int desth;
	int xofst;
	int yofst;
	bool maintainAspectRatio;
	bool borderDirty = true;
	ALLEGRO_BITMAP *buffer;
public:
	/**
	 * Constructor
	 * Initializes a low-res buffer with fixed `w` (width) and `h` (height).
	 * Draw, update and events are passed on to the child component `cc`
	 * If `maintainAspectRatio` is false, the canvas is always stretched to the corners of the screen. 
	 * If `maintainAspectRatio` is true, the canvas is surrounded by black borders as necessary to maintain the aspect ratio.
	 */
	PixelView(std::shared_ptr<IComponent> cc, 
		int w, 
		int h,
		bool maintainAspectRatio = false
	);
	virtual ~PixelView();

	/**
	 * Asks child component to draw on internal buffer, then displays the buffer.
	 */
	virtual void draw(const GraphicsContext &gc) override;

	/**
	 * This helper function converts mouse cursor coordinates to view coordinates.
	 */
	template <typename V>
	void adjustMickey(V &x, V &y) {
		x = (x - xofst) * w / destw;
		y = (y - yofst) * h / desth;
	}

	/**
	 * Forwards to child component, adjusting mouse & touch coordinates as needed.
	 */
	virtual void handleEvent(ALLEGRO_EVENT &event) override;

	/**
	 * Forwards to child component.
	 */
	virtual void update() override;

	/**
	 * Forwards to child component.
	 */
	virtual bool isAlive() const override;

	virtual void setSize(const Point &dim) override;
};
