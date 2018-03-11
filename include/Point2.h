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
};

}

#endif //POINT2_H