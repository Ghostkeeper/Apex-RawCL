/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef POINT2_H
#define POINT2_H

#include "Coordinate.h"

namespace parallelogram {

/*
 * Represents a point in a 2-dimensional coordinate system.
 *
 * We call the two dimensions X and Y.
 *
 * The coordinates are stored as coord_t, which is an integer type. This allows
 * for fixed-point accuracy where the loss of accuracy due to rounding is
 * predictable.
 */
class Point2 {
public:
	/*
	 * The projection of this point on the X axis.
	 */
	coord_t x;

	/*
	 * The projection of this point on the Y axis.
	 */
	coord_t y;

	/*
	 * Create a new point.
	 */
	Point2(const coord_t x, const coord_t y);

	/*
	 * Adds two points together.
	 *
	 * This is equivalent to translating this point by the coordinates of the
	 * other (or vice versa).
	 * \param other The point to add to this point.
	 */
	Point2 operator +(const Point2& other) const;

	/*
	 * Adds another point to this point in-place.
	 *
	 * This is equivalent to translating this point by the coordinates of the
	 * other.
	 * \param other The point to add to this point.
	 */
	Point2& operator +=(const Point2& other);

	/*
	 * Subtracts another point from this point.
	 *
	 * This results in the difference vector between the two points.
	 * \param other The point to subtract from this point.
	 */
	Point2 operator -(const Point2& other) const;

	/*
	 * Subtracts another point from this point in-place.
	 *
	 * This results in the difference vector between the two points.
	 * \param other The point to subtract from this point.
	 */
	Point2& operator -=(const Point2& other);

	/*
	 * Determines whether the point is left of a line segment.
	 *
	 * Or more accurately, this tests whether the point is port-side of the line
	 * going through the provided two points, when looking from the starting
	 * position in the direction of the end position.
	 * \param start The start of the line segment.
	 * \param end The end of the line segment.
	 * \return A positive number if this point is to the left of the line
	 * segment, a negative number if this point is to the right of the line
	 * segment, or 0 if it lies exactly on the line.
	 */
	coord_t isLeftOfLineSegment(const Point2& start, const Point2& end) const;
};

}

#endif //POINT2_H