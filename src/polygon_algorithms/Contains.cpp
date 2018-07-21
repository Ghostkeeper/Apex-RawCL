/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "SimplePolygon.h" //The class whose algorithms we're implementing.

namespace parallelogram {

bool SimplePolygon::contains_opencl(const cl::Device& device, const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	return false;
}

bool SimplePolygon::contains_host(const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	//TODO: If pre-calculation is allowed, obtain the AABB of the polygon and do that check first.

	if(size() == 2) {
		//This polygon is a line. Only report true if we consider edges inside and we're on the edge.
		return include_edge == EdgeInclusion::INSIDE && point.isLeftOfLineSegment(front(), back()) == 0;
	}
	if(size() == 1) {
		//This polygon is a point. Only report true if we consider edges inside and we're on the vertex.
		return include_edge == EdgeInclusion::INSIDE && point == front();
	}
	if(size() == 0) {
		return false;
	}

	//This is the winding number algorithm to determine if a point is inside a polygon.
	int winding_number = 0;
	Point2 previous = back();
	for(size_t index = 0; index < size(); index++) //Take two adjacent vertices of the polygon.
	{
		const Point2& next = (*this)[index];
		/* Cast a ray from the point towards the right and figure out whether
		the line segment between these two vertices crosses it and in which
		direction. */
		if(previous.y < next.y) { //Rising edge.
			//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
			if(point.y >= previous.y && point.y < next.y) { //Crosses height of point.
				const coord_t point_is_left = point.isLeftOfLineSegment(previous, next);
				if(point_is_left > 0 || (point_is_left == 0 && include_edge == EdgeInclusion::INSIDE)) { //Line is absolutely right of point. Point is relatively left of line.
					winding_number++;
				}
			}
		} else if(previous.y > next.y) { //Falling edge (next vertex is lower than previous vertex).
			//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
			if(point.y < previous.y && point.y >= next.y) { //Crosses height of point.
				const coord_t point_is_left = point.isLeftOfLineSegment(previous, next);
				if(point_is_left < 0 || (point_is_left == 0 && include_edge == EdgeInclusion::OUTSIDE)) { //Line is absolutely right of point. Point is relatively right of line.
					winding_number--;
				}
			}
		} else if(previous.y == point.y) { //Horizontal line at exactly the height of the point.
			if(previous.x < next.x && point.x >= previous.x && point.x <= next.x) { //Going to the left.
				if(include_edge == EdgeInclusion::OUTSIDE) {
					winding_number--;
				}
			} else if(previous.x >= next.x && point.x <= previous.x && point.x >= next.x) { //Going to the right.
				if(include_edge == EdgeInclusion::INSIDE) {
					winding_number++;
				}
			}
		}

		previous = next;
	}

	switch(fill_type) {
		default:
		case FillType::EVEN_ODD:
			return winding_number & 1;
		case FillType::NONZERO:
			return winding_number != 0;
	}
}

}