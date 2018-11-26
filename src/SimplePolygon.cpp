/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "Benchmarks.h" //To choose the preferred algorithm and device.
#include "SimplePolygon.h" //The class we're implementing.

namespace apex {

SimplePolygon::SimplePolygon() {
	//Only construct the vector of vertices.
}

area_t SimplePolygon::area() const {
	const std::vector<std::string> options = {"area_opencl", "area_host"};
	const std::vector<size_t> problem_size = {size()};
	const std::pair<std::string, const cl::Device*> best_choice = benchmarks::choose(options, problem_size);
	if(best_choice.first == "area_host" || !best_choice.second) {
		return area_host();
	} else {
		return area_opencl(*best_choice.second);
	}
}

bool SimplePolygon::contains(const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	return contains_host(point, include_edge, fill_type);
}

void SimplePolygon::translate(const coord_t x, const coord_t y) {
	translate(Point2(x, y));
}

void SimplePolygon::translate(const Point2 translation_vector) {
	for(size_t vertex = 0; vertex < size(); vertex++) {
		at(vertex) += translation_vector; //Add the coordinates to every vertex of the polygon.
	}
}

}