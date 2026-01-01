#pragma once

#include <iostream>
#include "mathutil.h"

template<typename T>
class Vec2
{
protected:
	/** The underlaying x coordinate. */
	T posx;

	/** The underlaying y coordinate. */
	T posy;
public:
	/** Default constructor. */
	Vec2(T x = 0, T y = 0) : posx(x), posy(y) {}
	
	void operator+=(const Vec2<T> &p) {
		posx += p.x();
		posy += p.y();
	}
	
	void operator-=(const Vec2<T> &p) {
		posx -= p.x();
		posy -= p.y();
	}
	

	/**
         Operators for doing simple vector arithmetic with two points.
         Examples:
<pre>
      Point a(10, 20);
      Point b(30, 40);

      Point c = a + b;   // c = (40, 60)
      Point d = b - a;   // d = (20, 20)
      c += a;            // c = (50, 80)
      c -= b;            // c = (20, 40)
</pre>
      */
	Vec2<T> operator-(const Vec2<T> &p) const {
		Vec2<T> res;
		res.x(posx - p.x());
		res.y(posy - p.y());
		return res;
	}

	Vec2<T> operator+(const Vec2<T> &p) const {
		Vec2<T> res;
		res.x(p.x() + posx);
		res.y(p.y() + posy);
		return res;
	}

	/**
	 * Multiply by scalar of any type (int, double, etc)
	 */
	template<typename U>
	Vec2<T> operator*(U scalar) const {
		Vec2<T> res;
		res.x(posx * scalar);
		res.y(posy * scalar);
		return res;
	}

	bool operator==(const Vec2<T> &p) const {
		return (x() == p.x()) && (y() == p.y());
	}

	bool operator!=(const Vec2<T> &p) const {
		return (x() != p.x()) || (y() != p.y());
	}

	// needed for e.g. inserting in a std::set.
	bool operator<(const Vec2<T> &p) const {
		return y() == p.y()
			? x() < p.x()
			: y() < p.y();
	}

	/* Get and set functions */
	T x() const { return posx; }
	T y() const { return posy; }
	void x(T v) { posx = v; }
	void y(T v) { posy = v; }

	/**
	 * Reduces both x and y to -1, 0 or 1
	 * For example Point(-9, 0) becomes Point(-1, 0)
	 */
	Vec2<T> sign() {
		return Vec2<T>(sgn(posx), sgn(posy));
	}
};

template<typename T>
std::ostream &operator<<(std::ostream &os, const Vec2<T> &p) {
	os << "[" << p.x() << "," << p.y() << "]";
	return os;
}

typedef Vec2<int> Point;
typedef Vec2<float> Vec2f;
