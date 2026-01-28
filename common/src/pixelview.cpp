#include "pixelview.h"
#include "color.h"
#include "point.h"

PixelView::PixelView(std::shared_ptr<IComponent> cc, 
	int _w, 
	int _h,
	bool _maintainAspectRatio
) : 
	childComponent(cc), 
	w(_w), h(_h), 
	maintainAspectRatio(_maintainAspectRatio)
{
	assert(childComponent);
	buffer = al_create_bitmap(_w, _h);
	
	int dw = al_get_display_width(al_get_current_display());
	int dh = al_get_display_height(al_get_current_display());
	setSize(Point(dw, dh));

	ALLEGRO_BITMAP *prev = al_get_target_bitmap();
	al_set_target_bitmap(buffer);
	al_clear_to_color(MAGIC_PINK);
	al_set_target_bitmap(prev);
}

PixelView::~PixelView() {
	al_destroy_bitmap(buffer);
}

void PixelView::draw(const GraphicsContext &gc) {
	GraphicsContext childContext = gc;
	// reset xofst / yofst from parent gc
	childContext.xofst = 0; 
	childContext.yofst = 0;
	childContext.viewWidth = al_get_bitmap_width(buffer);
	childContext.viewHeight = al_get_bitmap_height(buffer);
	childContext.buffer = buffer;
	
	al_set_target_bitmap(buffer);
	childComponent->draw(childContext);
	al_set_target_bitmap(gc.buffer);

	if (borderDirty) {
		al_clear_to_color(BLACK);
	}
	al_draw_scaled_bitmap(buffer, 0, 0, w, h, xofst, yofst, destw, desth, 0);
}

void PixelView::handleEvent(ALLEGRO_EVENT &event) {
	switch(event.type) {
		case ALLEGRO_EVENT_MOUSE_AXES:
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
		case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
		case ALLEGRO_EVENT_MOUSE_WARPED: {
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

void PixelView::update() {
	childComponent->update();
}

bool PixelView::isAlive() const {
	return childComponent->isAlive();
}

void PixelView::setSize(const Point &dim) {
	int dw = dim.x();
	int dh = dim.y();

	borderDirty = true;
	if (maintainAspectRatio) {
		float aspectRatio = (float)w / (float)h;
		float displayAspectRatio = (float)dw / (float)dh;

		// calculate letterbox...
		if (displayAspectRatio > aspectRatio) {
			int margin = dw - (dh * aspectRatio);
			// display is wider, black bars on the sides
			destw = dw - margin;
			desth = dh;
			xofst = margin / 2;
			yofst = 0;
		}
		else {
			// black bars above / below
			int margin = dh - (dw / aspectRatio);
			destw = dw;
			desth = dh - margin;
			xofst = 0;
			yofst = margin / 2;
		}
	}
	else {
		// don't maintain aspect ratio
		xofst = 0;
		yofst = 0;
		destw = dw;
		desth = dh;
	}

	childComponent->setSize(Point(w, h));
}
