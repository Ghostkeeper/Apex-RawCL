/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "Polygon.h"

namespace parallelogram {

Polygon::Polygon() {
	//Only initialise the simple_polygons vector.
}

area_t Polygon::area() const {
	coord_t area = 0;
	for(const SimplePolygon& simple_polygon : *this) {
		area += simple_polygon.area();
	}
	return area;
}

void Polygon::translate(const coord_t x, const coord_t y) {
	for(SimplePolygon& simple_polygon : *this) {
		simple_polygon.translate(x, y);
	}
}

}