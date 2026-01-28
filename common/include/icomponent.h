#pragma once

#include "point.h"

union ALLEGRO_EVENT;
struct GraphicsContext;

class IComponent {
public:
	virtual void update() = 0;
	virtual void draw(const GraphicsContext &gc) = 0;
	virtual void handleEvent(ALLEGRO_EVENT &evt) {};
	virtual void setSize(const Point &dim) {};
	virtual bool isAlive() const {
		return true;
	}
};
