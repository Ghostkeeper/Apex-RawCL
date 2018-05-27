/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <cmath> //To construct a regular n-gon.
#include <iostream> //To output the benchmark results to cout and progress data to cerr.
#include "Coordinate.h" //Creating vertices for polygons.
#include "SimplePolygonBenchmark.h"

#define PI 3.14159265358979

namespace parallelogram {

namespace benchmarks {

SimplePolygon SimplePolygonBenchmark::regularNGon(const size_t size) {
	SimplePolygon polygon;
	for(size_t vertex = 0; vertex < size; vertex++) {
		const coord_t x = std::lround(std::cos(PI * 2 / size * vertex) * size);
		const coord_t y = std::lround(std::sin(PI * 2 / size * vertex) * size);
		polygon.emplace_back(x, y);
	}
	return polygon;
}

SimplePolygonBenchmark::SimplePolygonBenchmark(const std::string name, const void(*run)(SimplePolygon)): name(name), run(run) {
	//Simply store all input parameters in the fields.
}

void SimplePolygonBenchmark::benchmark(const std::string device_identifier) const {
	//Debug output for progress reporting goes through std::cerr.
	std::cerr << name << ":   0%";

	size_t total_size = 0; //Compute the total input size for more gradual progress reporting.
	for(const size_t& size : input_sizes) {
		total_size += size;
	}
	std::vector<double> times(input_sizes.size(), 0.0); //The results of the benchmarks.
	size_t size_progress = 0;
	for(size_t size_index = 0; size_index < input_sizes.size(); size_index++) {
		//Generate a polygon of the appropriate size to test on.
		SimplePolygon polygon = construct_polygon(input_sizes[size_index]);

		unsigned long total_time = 0;
		unsigned long start_time = 0;
		unsigned long end_time = 0;
		for(unsigned int repeat = 0; repeat < repeats; repeat++) {
			start_time = clock();
			run(polygon); //Perform the benchmark!
			end_time = clock();
			total_time += end_time - start_time;

			const unsigned short progress = (size_progress * repeats + input_sizes[size_index] * repeat) * 100 / (total_size * repeats);
			std::cerr << "\b\b\b"; //Erase percentage sign and two digits (or one digit and a space).
			if(progress < 10) {
				std::cerr << " ";
			}
			std::cerr << progress << "%";
		}
		size_progress += input_sizes[size_index];
		times[size_index] = static_cast<double>(total_time) / CLOCKS_PER_SEC / repeats;
	}

	//Output the results to cout.
	for(size_t size_index = 0; size_index < input_sizes.size(); size_index++) {
		std::cout << name << "[std::make_pair(\"" << device_identifier << "\", " << input_sizes[size_index] << ")] = " << times[size_index] << ";" << std::endl;
	}
	std::cerr << "\b\b\b\b100%" << std::endl;
}

}

}