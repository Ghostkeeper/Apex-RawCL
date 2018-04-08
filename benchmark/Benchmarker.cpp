/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <algorithm> //For find_if to trim whitespace.
#include <functional> //For cref to trim whitespace.
#include <iostream> //To output the benchmark data to stdout.
#include <time.h> //For high-resolution timers to measure benchmarks.
#include <vector> //Lists of problem sizes to test with.
#include "OpenCL.h" //To get device information.
#include "OpenCLDevices.h" //To find the identifiers of the devices the benchmark is performed on.
#include "Point2.h" //To construct vertices for polygons.
#include "SimplePolygon.h" //A class of which we're benchmarking performance.

namespace parallelogram {
namespace benchmarks {

void Benchmarker::benchmark_area() {
	//The polygon sizes we'll be testing with.
	const std::vector<size_t> sizes = {1, 10, 100, 1000, 10000, 1000000, 2000000, 4000000, 8000000, 16000000, 32000000, 64000000, 128000000, 256000000, 512000000, 1024000000};
	//How many repeats to perform. More increases accuracy of timing.
	constexpr unsigned int repeats = 10;

	//Debug output for progress reporting goes through std::cerr.
	std::cerr << "Area (Host):   0%";

	std::string host_device = host_identifier();
	for(size_t size_index = 0; size_index < sizes.size(); size_index++) {
		std::cout << "area_host_time[std::make_pair(\"" << host_device << "\", " << sizes[size_index] << ")] = ";
		SimplePolygon polygon;
		for(size_t vertex = 0; vertex < sizes[size_index]; vertex++) {
			polygon.emplace_back(vertex, vertex);
		}
		const unsigned long start_time = clock();
		for(unsigned int repeat = 0; repeat < repeats; repeat++) {
			polygon.area_host();
			const int progress = (repeat + size_index * repeats) * 100 / (sizes.size() * repeats);
			std::cerr << "\b\b\b";
			if(progress < 10) {
				std::cerr << " ";
			}
			std::cerr << progress << "%";
		}
		const unsigned long end_time = clock();
		const double time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC / repeats;
		std::cout << time << ";" << std::endl;
	}
	std::cerr << "\b\b\b\b" << "100%" << std::endl;

	std::cerr << "Area (GPU):   0%";
	std::string gpu_device = gpu_identifier();
	for(size_t size_index = 0; size_index < sizes.size(); size_index++) {
		std::cout << "area_gpu_time[std::make_pair(\"" << gpu_device << "\", " << sizes[size_index] << ")] = ";
		SimplePolygon polygon;
		for(size_t vertex = 0; vertex < sizes[size_index]; vertex++) {
			polygon.emplace_back(vertex, vertex);
		}
		const unsigned long start_time = clock();
		for(unsigned int repeat = 0; repeat < repeats; repeat++) {
			polygon.area_gpu();
			const int progress = (repeat + size_index * repeats) * 100 / (sizes.size() * repeats);
			std::cerr << "\b\b\b";
			if(progress < 10) {
				std::cerr << " ";
			}
			std::cerr << progress << "%";
		}
		const unsigned long end_time = clock();
		const double time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC / repeats;
		std::cout << time << ";" << std::endl;
	}
	std::cerr << "\b\b\b\b" << "100%" << std::endl;
}

std::string Benchmarker::gpu_identifier() const {
	OpenCLDevices& devices = OpenCLDevices::getInstance();
	const cl::Device& device = devices.getGPUs()[0]; //This makes the assumption that the benchmarks are running on the first available GPU.
	std::string result;
	if(device.getInfo(CL_DEVICE_NAME, &result) != CL_SUCCESS) {
		return std::string("unknown_gpu");
	}
	trim(result);
	return result;
}

std::string Benchmarker::host_identifier() const {
	OpenCLDevices& devices = OpenCLDevices::getInstance();
	const cl::Device& device = devices.getCPUs()[0]; //This makes the assumption that the first CPU is the host.
	std::string result;
	if(device.getInfo(CL_DEVICE_NAME, &result) != CL_SUCCESS) {
		return std::string("unknown_host");
	}
	trim(result);
	return result;
}

inline void Benchmarker::trim(std::string& input) const {
	const std::function<bool(char)> is_not_whitespace = [](char character) {
		return !std::isspace<char>(character, std::locale::classic()) && character != 0;
	};
	input.erase(input.begin(), std::find_if(input.begin(), input.end(), is_not_whitespace)); //Trim whitespace at the start.
	input.erase(std::find_if(input.rbegin(), input.rend(), is_not_whitespace).base(), input.end()); //Trim whitespace at the end.
}

}
}

/*
 * Writes a C++ file to stdout that defines the benchmark results.
 */
int main(int argc, char** argv) {
	std::cout << "//Generated by Parallelogram's benchmarker." << std::endl;
	parallelogram::benchmarks::Benchmarker benchmarker;
	benchmarker.benchmark_area();
	return 0;
}