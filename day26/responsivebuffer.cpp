#include "responsivebuffer.h"

#include "color.h"
#include "point.h"

ResponsiveBuffer::ResponsiveBuffer(std::shared_ptr<IComponent> cc) : childComponent(cc) {
	assert(childComponent);
	
	int dw = al_get_display_width(al_get_current_display());
	int dh = al_get_display_height(al_get_current_display());
	setSize(Point(dw, dh));

	ALLEGRO_BITMAP *prev = al_get_target_bitmap();
	al_set_target_bitmap(buffer);
	al_clear_to_color(MAGIC_PINK);
	al_set_target_bitmap(prev);
}

ResponsiveBuffer::~ResponsiveBuffer() {
	al_destroy_bitmap(buffer);
}

void ResponsiveBuffer::draw(const GraphicsContext &gc) {
	GraphicsContext childContext = gc;
	childContext.buffer = buffer;
	
	al_set_target_bitmap(buffer);
	childComponent->draw(childContext);
	al_set_target_bitmap(gc.buffer);

	al_draw_bitmap(buffer, 0, 0, 0);
}

void ResponsiveBuffer::handleEvent(ALLEGRO_EVENT &event) {
	childComponent->handleEvent(event);
}

void ResponsiveBuffer::update() {
	childComponent->update();
}

bool ResponsiveBuffer::isAlive() const {
	return childComponent->isAlive();
}

void ResponsiveBuffer::setSize(const Point &dim) {
	if (buffer) {
		al_destroy_bitmap(buffer);
	}
	buffer = al_create_bitmap(dim.x(), dim.y());
	
	w = dim.x();
	h = dim.y();

	childComponent->setSize(dim);
}
