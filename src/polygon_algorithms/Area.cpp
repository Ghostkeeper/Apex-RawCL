/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "SimplePolygon.h"

namespace parallelogram {

coord_t SimplePolygon::area_gpu() const {
	//TODO: Implement this.
	return area_host();
}

coord_t SimplePolygon::area_host() const {
	//Apothem method to compute the area.
	coord_t area = 0;
	size_t previous = size() - 1;
	for(size_t vertex = 0, previous = size() - 1; vertex < size(); vertex++) {
		area += (*this)[previous].x * (*this)[vertex].y - (*this)[previous].y * (*this)[vertex].x;
		previous = vertex;
	}
	return area >> 1;
}

}