#pragma once

struct ALLEGRO_BITMAP;

struct GraphicsContext {
	ALLEGRO_BITMAP *buffer;
	int xofst;
	int yofst;
	int viewWidth;
	int viewHeight;

	int displayWidth;
	int displayHeight;

	/**
	 * converts display coordinates to view coordinates.
	 */
	template<typename V>
	void adjustMickey(V &x, V &y) const {
		x = (x - xofst) * viewWidth / displayWidth;
		y = (y - xofst) * viewHeight / displayHeight;
	}
};
