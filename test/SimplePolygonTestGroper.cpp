/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "SimplePolygonTestGroper.h"

namespace parallelogram {

area_t SimplePolygonTestGroper::area_host() const {
	return tested_simple_polygon->area_host();
}

area_t SimplePolygonTestGroper::area_opencl(const cl::Device& device) const {
	return tested_simple_polygon->area_opencl(device);
}

bool SimplePolygonTestGroper::contains_host(const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	return tested_simple_polygon->contains_host(point, include_edge, fill_type);
}

bool SimplePolygonTestGroper::contains_opencl(const cl::Device& device, const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	return tested_simple_polygon->contains_opencl(device, point, include_edge, fill_type);
}

}