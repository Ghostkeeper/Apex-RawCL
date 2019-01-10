/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef APEX_POLYGON_H
#define APEX_POLYGON_H

#include <vector> //To list the simple polygons.
#include "SimplePolygon.h" //The simple polygons that this complex polygon consists of.

namespace apex {

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
	using std::vector<SimplePolygon>::const_iterator;
	using std::vector<SimplePolygon>::const_reverse_iterator;
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
	using std::vector<SimplePolygon>::iterator;
	using std::vector<SimplePolygon>::max_size;
	using std::vector<SimplePolygon>::pop_back;
	using std::vector<SimplePolygon>::push_back;
	using std::vector<SimplePolygon>::rbegin;
	using std::vector<SimplePolygon>::rend;
	using std::vector<SimplePolygon>::reserve;
	using std::vector<SimplePolygon>::reverse_iterator;
	using std::vector<SimplePolygon>::shrink_to_fit;
	using std::vector<SimplePolygon>::size;
	using std::vector<SimplePolygon>::swap;

	/*
	 * Compute the total surface area of the polygon.
	 * \return The area of the polygon.
	 */
	area_t area() const;

	/*
	 * Test whether the specified point is inside this polygon.
	 *
	 * You can test this with different fill rules, and choose whether to
	 * include edges or not.
	 *
	 * Negative polygons (with clockwise winding order) are computed in the same
	 * way as positive polygons. For the non-zero and even-odd fill rules, the
	 * answer of this algorithm will be the same regardless of the winding order
	 * of the polygon. However if you check for a point on the edge of a
	 * polygon, the answer will be inversed for negative polygons. This keeps
	 * the answer consistent with complex polygons, where a negative polygon
	 * delimits a hole in the shape.
	 *
	 * Points where the polygon's edges intersect each other are indeterminate.
	 * This is because the point must be inside if it's on the edge of a
	 * positive polygon and edges are included, or outside if it's on the edge
	 * of a negative polygon and edges are included (and vice-versa if edges are
	 * not included). On a point of self-intersection, the winding order of the
	 * shape locally around the point is indeterminate. It could be either
	 * positive (counter-clockwise) or negative (clockwise). The answer given
	 * will depend on the frills of the algorithm but should not be considered
	 * reliable.
	 * \param point The point to test.
	 * \param include_edge Whether the edge of the polygon should be counted as
	 * being inside the polygon.
	 * \param fill_type What areas to count as being inside this simple polygon
	 * if the polygon is self-intersecting.
	 * \return ``True`` if the specified point is inside this polygon, or
	 * ``False`` if it is outside.
	 */
	bool contains(const Point2& point, const EdgeInclusion& include_edge = EdgeInclusion::INSIDE, const FillType& fill_type = FillType::NONZERO) const;

	/*
	 * Move the polygon by a certain offset in each dimension.
	 * \param x The offset to move in the X direction.
	 * \param y The offset to move in the Y direction.
	 */
	void translate(const coord_t x, const coord_t y);
};

}

#endif //APEX_POLYGON_H