#include "pixelbuffer.h"
#include "color.h"

PixelBuffer::PixelBuffer(std::shared_ptr<IComponent> cc, 
	int _w, 
	int _h,
	bool maintainAspectRatio,
	bool integralScaling,
	bool useBuffer
): childComponent(cc), w(_w), h(_h) {
	buffer = al_create_bitmap(_w, _h);
	
	ALLEGRO_BITMAP *prev = al_get_target_bitmap();
	al_set_target_bitmap(buffer);
	al_clear_to_color(MAGIC_PINK);
	al_set_target_bitmap(prev);
}

PixelBuffer::~PixelBuffer() {
	al_destroy_bitmap(buffer);
}

void PixelBuffer::draw(const GraphicsContext &gc) {
	GraphicsContext childContext;
	childContext.xofst = gc.xofst;
	childContext.yofst = gc.yofst;
	childContext.buffer = buffer;
	
	al_set_target_bitmap(buffer);
	childComponent->draw(childContext);
	al_set_target_bitmap(gc.buffer);

	int dw = al_get_bitmap_width(gc.buffer);
	int dh = al_get_bitmap_height(gc.buffer);
	al_draw_scaled_bitmap(buffer, 0, 0, w, h, 0, 0, dw, dh, 0);
}

void PixelBuffer::handleEvent(ALLEGRO_EVENT &event) {
	switch(event.type) {
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		case ALLEGRO_EVENT_MOUSE_AXES: {
			adjustMickey(event.mouse.x, event.mouse.y);
			childComponent->handleEvent(event);
			break;
		}
		case ALLEGRO_EVENT_TOUCH_BEGIN:
		case ALLEGRO_EVENT_TOUCH_END:
		case ALLEGRO_EVENT_TOUCH_MOVE:
		case ALLEGRO_EVENT_TOUCH_CANCEL: {
			adjustMickey(event.touch.x, event.touch.y);
			childComponent->handleEvent(event);
			break;
		}
		default:
			childComponent->handleEvent(event);
			break;
	}
}
