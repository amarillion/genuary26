#pragma once

union ALLEGRO_EVENT;
struct GraphicsContext;

class IComponent {
public:
	virtual void update() = 0;
	virtual void draw(const GraphicsContext &gc) = 0;
	virtual void handleEvent(ALLEGRO_EVENT &evt) {};
	virtual bool isAlive() const {
		return true;
	}
};
