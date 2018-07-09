/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <iostream> //DEBUG!
#include "SimplePolygon.h" //The class whose algorithms we're implementing.

namespace parallelogram {

bool SimplePolygon::contains_host(const Point2& point, const FillType& fill_type) const {
	if(size() < 3) {
		return false; //TODO: Check edge cases for lines and points if edges are to be included. Currently they're not included.
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
			if(point.y > previous.y && point.y < next.y) { //Crosses height of point.
				if(point.isLeftOfLineSegment(previous, next) > 0) { //Line is right of point. Point is left of line.
					winding_number++;
				}
			}
		} else if(previous.y > next.y) { //Falling edge (next vertex is lower than previous vertex).
			if(point.y < previous.y && point.y > next.y) { //Crosses height of point.
				if(point.isLeftOfLineSegment(previous, next) < 0) { //Line is right of point. Point is right of line.
					winding_number--;
				}
			}
		}

		previous = next;
	}

	switch(fill_type) {
		default:
		case FillType::EVEN_ODD:
			return winding_number % 2 == 1;
		case FillType::NONZERO:
			return winding_number != 0;
	}
}

}