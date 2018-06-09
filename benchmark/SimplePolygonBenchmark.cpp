/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <cmath> //To construct a regular n-gon.
#include "Eigen/Core" //To perform calculations for interpolation between data points.
#include "Eigen/QR" //To perform QR resolution for linear least squares.
#include <iostream> //To output the benchmark results to cout and progress data to cerr.
#include "BenchmarkData.h" //To use the pre-existing benchmark data to generate interpolation vectors.
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

SimplePolygonBenchmark::SimplePolygonBenchmark(const std::string name, std::function<void(const cl::Device*, SimplePolygon&)> run): name(name), run(run) {
	//Simply store all input parameters in the fields.
}

void SimplePolygonBenchmark::benchmark(const cl::Device* device, const std::string device_identifier) const {
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
			run(device, polygon); //Perform the benchmark!
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
		std::cout << "bench_data[std::make_tuple<std::string, std::string, size_t>(\"" << name << "\", \"" << device_identifier << "\", " << input_sizes[size_index] << ")] = " << times[size_index] << ";" << std::endl;
	}
	std::cerr << "\b\b\b\b100%" << std::endl; //Otherwise it ends up at 99% at the end.
}

std::vector<double> SimplePolygonBenchmark::compute_interpolation() const {
	load_benchmarks();

	Eigen::Matrix<double, Eigen::Dynamic, 13> fit_data; //13 columns for the input size, our 6 device data points (some squared) and one constant offset. Just linear for now.
	fit_data.resize(parallelogram::benchmarks::devices.size() * input_sizes.size(), 13);
	Eigen::VectorXd time_data;
	time_data.resize(parallelogram::benchmarks::devices.size() * input_sizes.size());
	size_t entry_id = 0;
	for(std::pair<std::string, std::unordered_map<std::string, cl_ulong>> device_metadata : parallelogram::benchmarks::devices) {
		for(size_t size : input_sizes) {
			fit_data(entry_id, 0) = device_metadata.second["device_type"];
			fit_data(entry_id, 1) = device_metadata.second["compute_units"];
			fit_data(entry_id, 2) = device_metadata.second["compute_units"] * device_metadata.second["compute_units"];
			fit_data(entry_id, 3) = device_metadata.second["items_per_compute_unit"];
			fit_data(entry_id, 4) = device_metadata.second["items_per_compute_unit"] * device_metadata.second["items_per_compute_unit"];
			fit_data(entry_id, 5) = device_metadata.second["clock_frequency"];
			fit_data(entry_id, 6) = device_metadata.second["global_memory"];
			fit_data(entry_id, 7) = device_metadata.second["global_memory"] * device_metadata.second["global_memory"];
			fit_data(entry_id, 8) = device_metadata.second["local_memory"];
			fit_data(entry_id, 9) = device_metadata.second["local_memory"] * device_metadata.second["local_memory"];
			fit_data(entry_id, 10) = size;
			fit_data(entry_id, 11) = size * size;
			fit_data(entry_id, 12) = 1.0; //Constant offset.
			time_data(entry_id) = bench_data[std::tuple<std::string, std::string, size_t>(name, device_metadata.first, size)];
			entry_id++;
		}
	}
	Eigen::VectorXd solution = fit_data.fullPivHouseholderQr().solve(time_data);
	std::cout << "area_opencl_predictor[\"device_type\"] = " << solution(0) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"compute_units\"] = " << solution(1) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"compute_units^2\"] = " << solution(2) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"items_per_compute_unit\"] = " << solution(3) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"items_per_compute_unit^2\"] = " << solution(4) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"clock_frequency\"] = " << solution(5) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"global_memory\"] = " << solution(6) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"global_memory^2\"] = " << solution(7) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"local_memory\"] = " << solution(8) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"local_memory^2\"] = " << solution(9) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"size\"] = " << solution(10) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"size^2\"] = " << solution(11) << ";" << std::endl;
	std::cout << "area_opencl_predictor[\"constant\"] = " << solution(12) << ";" << std::endl;
}

}

}