/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <stdlib.h> //For abs.
#include "SimplePolygon.h"

namespace parallelogram {

SimplePolygon::SimplePolygon() {
	//Only construct the vector of vertices.
}

coord_t SimplePolygon::area() const {
	//Apothem method to compute the area.
	coord_t area = 0;
	const Point2& previous_vertex = back();
	for(const Point2 vertex : *this) {
		area += previous_vertex.x * vertex.y - previous_vertex.y * vertex.x;
	}
	area /= 2;
	return abs(area);
}

}