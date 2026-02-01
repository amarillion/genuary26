#pragma once

#include <memory>
#include "graphicscontext.h"
#include <allegro5/allegro.h>
#include "icomponent.h"

/**
 * A buffered viewport,
 * always matches parent size pixel for pixel, it doesn't stretch.
 * useful to speed up detailed rendering.
 */
class ResponsiveBuffer: public IComponent {
private:
	std::shared_ptr<IComponent> childComponent;
	int w;
	int h;
	ALLEGRO_BITMAP *buffer;
public:
	ResponsiveBuffer(std::shared_ptr<IComponent> cc);
	virtual ~ResponsiveBuffer();

	/**
	 * Asks child component to draw on internal buffer, then displays the buffer.
	 */
	virtual void draw(const GraphicsContext &gc) override;

	/**
	 * Forwards to child component.
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

	/**
	 * Recreates buffer, then resizes child component.
	 */
	virtual void setSize(const Point &dim) override;
};
