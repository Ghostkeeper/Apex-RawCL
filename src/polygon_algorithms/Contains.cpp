/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "SimplePolygon.h" //The class whose algorithms we're implementing.

namespace parallelogram {

bool SimplePolygon::contains_host(const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	//TODO: If pre-calculation is allowed, obtain the AABB of the polygon and do that check first.
	if(size() < 3) {
		return false; //TODO: Check edge cases for lines and points if edges are to be included. Currently they're not included.
	}
	//This is the winding number algorithm to determine if a point is inside a polygon.
	int winding_number = 0;
	int edges_hit = 0;
	Point2 previous = back();
	for(size_t index = 0; index < size(); index++) //Take two adjacent vertices of the polygon.
	{
		const Point2& next = (*this)[index];
		/* Cast a ray from the point towards the right and figure out whether
		the line segment between these two vertices crosses it and in which
		direction. */
		if(previous.y < next.y) { //Rising edge.
			//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
			//Do NOT count horizontal lines at all.
			if(point.y >= previous.y && point.y < next.y) { //Crosses height of point.
				const coord_t point_is_left = point.isLeftOfLineSegment(previous, next);
				if(point_is_left > 0) { //Line is absolutely right of point. Point is relatively left of line.
					winding_number++;
				} else if(point_is_left == 0) { //Point is on top of line! This is the edge case.
					if(include_edge == EdgeInclusion::OUTSIDE) {
						edges_hit++;
					} else {
						winding_number++;
					}
				}
			}
		} else if(previous.y > next.y) { //Falling edge (next vertex is lower than previous vertex).
			//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
			if(point.y < previous.y && point.y >= next.y) { //Crosses height of point.
				const coord_t point_is_left = point.isLeftOfLineSegment(previous, next);
				if(point.isLeftOfLineSegment(previous, next) < 0) { //Line is absolutely right of point. Point is relatively right of line.
					winding_number--;
				} else if(point_is_left == 0) { //Point is on top of line! This is the edge case.
					if(include_edge == EdgeInclusion::INSIDE) {
						edges_hit--;
					} else {
						winding_number--;
					}
				}
			}
		} else if(previous.y == point.y) { //Horizontal line at exactly the height of the point.
			if(previous.x < next.x && point.x >= previous.x && point.x <= next.x) {
				if(include_edge == EdgeInclusion::INSIDE) {
					edges_hit++;
				} else {
					winding_number--;
				}
			} else if(previous.x >= next.x && point.x <= previous.x && point.x >= next.x) {
				if(include_edge == EdgeInclusion::OUTSIDE) {
					edges_hit--;
				} else {
					winding_number++;
				}
			}
		}

		previous = next;
	}

	switch(fill_type) {
		default:
		case FillType::EVEN_ODD:
			return winding_number & 1 || (include_edge == EdgeInclusion::INSIDE && (edges_hit & 1));
		case FillType::NONZERO:
			return winding_number != 0 || (include_edge == EdgeInclusion::INSIDE && (edges_hit != 0));
	}
}

}