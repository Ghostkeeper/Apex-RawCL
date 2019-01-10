/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef APEX_SIMPLEPOLYGON_H
#define APEX_SIMPLEPOLYGON_H

#include <vector> //To store the vertices.
#include "EdgeInclusion.h" //To specify whether the edge of a polygon is considered inside the polygon.
#include "FillType.h" //To specify the default fill type for contains().
#include "Point2.h" //The type to represent a vertex.

namespace cl {
class Device; //Forward declaration of Device so we don't have to include all of OpenCL.
}

namespace apex {

namespace benchmarks {
class Benchmarker; //Forward declaration of Benchmarker so that the benchmarker can use SimplePolygons as well.
}
class SimplePolygonTestGroper; //Forward declaration of a class that can grope the privates of this class to test them.

/*
 * Represents a simple polygon.
 *
 * Simple polygons consist of a single boundary. This is not necessarily a
 * simple polygon in the mathematical sense since there is no checking whether
 * the boundary intersects itself.
 */
class SimplePolygon : private std::vector<Point2> {
	friend class benchmarks::Benchmarker;
	friend class SimplePolygonTestGroper;
public:
	/*
	 * Constructs an empty simple polygon.
	 *
	 * The polygon will have no vertices or edges and no area.
	 */
	SimplePolygon();

	/*
	 * Copies a simple polygon.
	 * \param original The polygon to create a copy of.
	 */
	SimplePolygon(const SimplePolygon& original) : std::vector<Point2>(original) {
		//Copy constructor of std::vector.
	}

	/*
	 * Moves a simple polygon.
	 * \param original The polygon to move to a different instance.
	 */
	SimplePolygon(SimplePolygon&& original) : std::vector<Point2>(original) {
		//Move constructor of std::vector.
	}

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
	 * Test whether the specified point is inside this simple polygon.
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

	/*
	 * Move the polygon by a certain offset in each dimension.
	 * \param translation_vector The vector by which to move the polygon.
	 */
	void translate(const Point2 translation_vector);

private:
	/*
	 * Implements the ``area`` function via OpenCL.
	 *
	 * This may perform better on large amounts of data.
	 * \param device The OpenCL device to compute the area with.
	 * \return The area of the current polygon.
	 */
	area_t area_opencl(const cl::Device& device) const;

	/*
	 * Implements the ``area`` function on the host hardware.
	 *
	 * This does not go via OpenCL. It may perform better for small polygons.
	 * \return The area of the current polygon.
	 */
	area_t area_host() const;

	/*
	 * Implements the ``contains`` function via OpenCL.
	 *
	 * This may perform better on large amounts of data.
	 * \param device The OpenCL device to test for containment with.
	 * \param point The point to test.
	 * \param include_edge Whether the edge of the polygon should be counted as
	 * being inside the polygon.
	 * \param fill_type What areas to count as being inside this simple polygon
	 * if the polygon is self-intersecting.
	 * \return ``True`` if the specified point is inside this polygon, or
	 * ``False`` if it is outside.
	 */
	bool contains_opencl(const cl::Device& device, const Point2& point, const EdgeInclusion& include_edge = EdgeInclusion::INSIDE, const FillType& fill_type = FillType::NONZERO) const;

	/*
	 * Implements the ``contains`` function on the host hardware.
	 *
	 * This does not go via OpenCL. It may perform better for small polygons.
	 * \param point The point to test.
	 * \param include_edge Whether the edge of the polygon should be counted as
	 * being inside the polygon.
	 * \param fill_type What areas to count as being inside this simple polygon
	 * if the polygon is self-intersecting.
	 * \return ``True`` if the specified point is inside this polygon, or
	 * ``False`` if it is outside.
	 */
	bool contains_host(const Point2& point, const EdgeInclusion& include_edge = EdgeInclusion::INSIDE, const FillType& fill_type = FillType::NONZERO) const;
};

}

#endif //APEX_SIMPLEPOLYGON_H

