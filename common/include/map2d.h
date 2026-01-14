#pragma once

#include <cassert>
#include <vector>
#include "point.h"

// -> size_t
#include <cstddef>

template <class T> class Map2D
{
protected:
	T *data;

	// dimensions of the map in map units
	int dim_mx;
	int dim_my;

	inline int indexFromXY(int mx, int my) const {
		return mx + dim_mx * my;
	}

public:
	//TODO: unit test
	//TODO: rename to matrix
	Map2D (std::initializer_list< std::initializer_list<T> > il)
	{
		// deduce dimensions from initializer list
		size_t h = il.size();
		size_t w = 0;
		for (auto row : il)
		{
			if (row.size() > w) w = row.size();
		}

		data = new T[w * h];
		dim_mx = w;
		dim_my = h;

		int my = 0;
		for (auto row : il)
		{
			int mx = 0;
			for (auto cell : row)
			{
				data[indexFromXY(mx, my)] = cell;
				mx++;
			}
			my++;
		}
	}

	Map2D (int w, int h, T defaultValue = T())
	{
		assert (w >= 0);
		assert (h >= 0);
		data = new T[w * h];
		std::fill(data, data + (w * h), defaultValue);
		dim_mx = w;
		dim_my = h;
	}

	Map2D ()
	{
		data = nullptr;
		dim_mx = 0;
		dim_my = 0;
	}

	// copy constructor
	Map2D(const Map2D &src) {
		dim_mx = src.dim_mx;
		dim_my = src.dim_my;
		data = new T[dim_mx * dim_my];
		std::copy(src.data, src.data + (dim_mx * dim_my), data);
#ifdef DEBUG
		std::cout << "WARNING: Map2D copy constructor invoked\n"; // emitting warning, because this large copy is usually not intended.
#endif
	}

	virtual ~Map2D()
	{
		if (data) delete[] data;
		data = nullptr;
	}

	T &get (int mx, int my)
	{
		assert (inBounds(mx, my));
		assert (data);
		return data[indexFromXY(mx, my)];
	}

	const T &get (int mx, int my) const
	{
		assert (inBounds(mx, my));
		assert (data);
		return data[indexFromXY(mx, my)];
	}

	T& operator[](const Point &p) {
		assert (inBounds(p));
		assert (data);
		return data[indexFromXY(p.x(), p.y())];
	}

	const T& operator[](const Point &p) const {
		assert (inBounds(p));
		assert (data);
		return data[indexFromXY(p.x(), p.y())];
	}

	/** use parentheses to simulate array notation */
	T& operator()(unsigned int mx, unsigned int my)
	{
		assert (inBounds(mx, my));
		assert (data);
		return data[indexFromXY(mx, my)];
	}

	/** use parentheses to simulate array notation */
	const T& operator()(unsigned int mx, unsigned int my) const
	{
		assert (inBounds(mx, my));
		assert (data);
		return data[indexFromXY(mx, my)];
	}

	std::vector<T> getRow(int row) const
	{
		assert (row >= 0 && row < dim_my);
		std::vector<T> result(dim_mx);
		int pos = indexFromXY(0, row);
		for (int x = 0; x < dim_mx; ++x)
		{
			result[x] = data[pos];
			pos++;
		}
		return result;
	}

	/**
	 * Change dimension of the map.
	 * Data is cleared. No attempt is made to
	 * preserve contents of map
	 */
	void resizeAndClear(int w, int h)
	{
		if (data)
			delete[] data;

		dim_mx = w;
		dim_my = h;
		data = new T[dim_mx * dim_my];
	}

	size_t getDimMX() const { return dim_mx; }
	size_t getDimMY() const { return dim_my; }

	inline bool inBounds(int mx, int my) const {
		return (mx >= 0) && (my >= 0) && (mx < dim_mx) && (my < dim_my);
	}

	inline bool inBounds(const Point &p) const {
		return (p.x() >= 0) && (p.y() >= 0) && (p.x() < dim_mx) && (p.y() < dim_my);
	}

	void repr(std::ostream &os, const char *sep = ", ", const char *rowSep = "\n") const {
		for (int y = 0; y < dim_my; ++y) {
			bool rowFirst = true;
			for (int x = 0; x < dim_mx; ++x) {
				if (rowFirst) { rowFirst = false; } else { os << sep; }
				os << get(x, y);
			}
			os << rowSep;
		}
	}
};

template <class T>
void matrixMul(std::vector<T> &a, Map2D<T> const &b, std::vector<T> &result)
{
	size_t n = b.getDimMX();
	size_t m = b.getDimMY();
	assert (a.size() == n);
	result.resize(m);

	for (size_t y = 0; y < b.getDimMY(); ++y)
	{
		T sum = 0;
		for (size_t x = 0; x < b.getDimMX(); ++x)
		{
			sum += (a[x] * b.get(x, y));
		}
		result[y] = sum;
	}
}

template <class T>
void matrixMulInv(std::vector<T> &a, Map2D<T> const &b, std::vector<T> &result)
{
	size_t n = b.getDimMX();
	size_t m = b.getDimMY();
	assert (a.size() == m);
	result.resize(n);

	for (size_t x = 0; x < b.getDimMX(); ++x)
	{
		T sum = 0;
		for (size_t y = 0; y < b.getDimMY(); ++y)
		{
			sum += (a[y] * b.get(x, y));
		}
		result[x] = sum;
	}
}
