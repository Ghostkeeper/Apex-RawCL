/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGON_H
#define SIMPLEPOLYGON_H

#include <vector> //To store the vertices.
#include "Benchmarker.h" //To give the benchmarker access to private members to benchmark them.
#include "Point2.h" //The type to represent a vertex.

namespace parallelogram {

/*
 * Represents a simple polygon.
 *
 * Simple polygons consist of a single boundary. This is not necessarily a
 * simple polygon in the mathematical sense since there is no checking whether
 * the boundary intersects itself.
 */
class SimplePolygon : private std::vector<Point2> {
	friend class Benchmarker;
public:
	/*
	 * Constructs an empty simple polygon.
	 *
	 * The polygon will have no vertices or edges and no area.
	 */
	SimplePolygon();

	//Operations inheriting from std::vector.
	using std::vector<Point2>::operator=;
	using std::vector<Point2>::operator[];
	using std::vector<Point2>::assign;
	using std::vector<Point2>::at;
	using std::vector<Point2>::back;
	using std::vector<Point2>::begin;
	using std::vector<Point2>::capacity;
	using std::vector<Point2>::cbegin;
	using std::vector<Point2>::cend;
	using std::vector<Point2>::crbegin;
	using std::vector<Point2>::crend;
	using std::vector<Point2>::clear;
	using std::vector<Point2>::data;
	using std::vector<Point2>::emplace;
	using std::vector<Point2>::emplace_back;
	using std::vector<Point2>::empty;
	using std::vector<Point2>::end;
	using std::vector<Point2>::front;
	using std::vector<Point2>::get_allocator;
	using std::vector<Point2>::insert;
	using std::vector<Point2>::max_size;
	using std::vector<Point2>::pop_back;
	using std::vector<Point2>::push_back;
	using std::vector<Point2>::rbegin;
	using std::vector<Point2>::rend;
	using std::vector<Point2>::reserve;
	using std::vector<Point2>::shrink_to_fit;
	using std::vector<Point2>::size;
	using std::vector<Point2>::swap;

	/*
	 * Compute the total surface area of this simple polygon.
	 * \return The area of the simple polygon.
	 */
	area_t area() const;

	/*
	 * Move the polygon by a certain offset in each dimension.
	 * \param x The offset to move in the X direction.
	 * \param y The offset to move in the Y direction.
	 */
	void translate(const coord_t x, const coord_t y);

private:
	/*
	 * Implements the ``area`` function via OpenCL tuned for use on a GPU.
	 *
	 * This may perform better on large amounts of data.
	 */
	area_t area_gpu() const;

	/*
	 * Implements the ``area`` function on the host hardware.
	 *
	 * This does not go via OpenCL. It may perform better for small amounts of
	 * data.
	 */
	area_t area_host() const;
};

}

#endif //SIMPLEPOLYGON_H

