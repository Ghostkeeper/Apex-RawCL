/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <cmath> //To construct a regular n-gon.
#include "Coordinate.h" //Creating vertices for polygons.
#include "SimplePolygonBenchmark.h"

#define PI 3.14159265358979

namespace parallelogram {

namespace benchmark {

SimplePolygon SimplePolygonBenchmark::regularNGon(const size_t size) {
	SimplePolygon polygon;
	for(size_t vertex = 0; vertex < size; vertex++) {
		const coord_t x = std::lround(std::cos(PI * 2 / size * vertex) * size);
		const coord_t y = std::lround(std::sin(PI * 2 / size * vertex) * size);
		polygon.emplace_back(x, y);
	}
}

SimplePolygonBenchmark::SimplePolygonBenchmark(const std::string name, const void(*run)(SimplePolygon), const std::vector<size_t> input_sizes, const SimplePolygon(*construct_polygon)(const size_t))
: name(name)
, run(run)
, input_sizes(input_sizes)
, construct_polygon(construct_polygon) {
	//Simply store all input parameters in the fields.
}

}

}