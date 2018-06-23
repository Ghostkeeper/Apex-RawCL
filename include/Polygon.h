/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef POLYGON_H
#define POLYGON_H

#include <vector> //To list the simple polygons.
#include "SimplePolygon.h" //The simple polygons that this complex polygon consists of.

namespace parallelogram {

/*
 * A complex polygon.
 *
 * Complex polygons represent shapes that can be represented using vertices and
 * straight line segments between them.
 */
class Polygon : private std::vector<SimplePolygon> {
public:
	/*
	 * Initialise an empty complex polygon.
	 *
	 * The polygon will have no area at all.
	 */
	Polygon();

	//Operations inheriting from std::vector.
	using std::vector<SimplePolygon>::operator=;
	using std::vector<SimplePolygon>::operator[];
	using std::vector<SimplePolygon>::assign;
	using std::vector<SimplePolygon>::at;
	using std::vector<SimplePolygon>::back;
	using std::vector<SimplePolygon>::begin;
	using std::vector<SimplePolygon>::capacity;
	using std::vector<SimplePolygon>::cbegin;
	using std::vector<SimplePolygon>::cend;
	using std::vector<SimplePolygon>::crbegin;
	using std::vector<SimplePolygon>::crend;
	using std::vector<SimplePolygon>::clear;
	using std::vector<SimplePolygon>::data;
	using std::vector<SimplePolygon>::emplace;
	using std::vector<SimplePolygon>::emplace_back;
	using std::vector<SimplePolygon>::empty;
	using std::vector<SimplePolygon>::end;
	using std::vector<SimplePolygon>::front;
	using std::vector<SimplePolygon>::get_allocator;
	using std::vector<SimplePolygon>::insert;
	using std::vector<SimplePolygon>::max_size;
	using std::vector<SimplePolygon>::pop_back;
	using std::vector<SimplePolygon>::push_back;
	using std::vector<SimplePolygon>::rbegin;
	using std::vector<SimplePolygon>::rend;
	using std::vector<SimplePolygon>::reserve;
	using std::vector<SimplePolygon>::shrink_to_fit;
	using std::vector<SimplePolygon>::size;
	using std::vector<SimplePolygon>::swap;

	/*
	 * Compute the total surface area of the polygon.
	 * \return The area of the polygon.
	 */
	area_t area() const;

	/*
	 * Move the polygon by a certain offset in each dimension.
	 * \param x The offset to move in the X direction.
	 * \param y The offset to move in the Y direction.
	 */
	void translate(const coord_t x, const coord_t y);
};

}

#endif //POLYGON_H
