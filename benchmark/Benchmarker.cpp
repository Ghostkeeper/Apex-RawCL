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
#include "ParallelogramException.h"
#include "Point2.h" //To construct vertices for polygons.
#include "SimplePolygon.h" //A class of which we're benchmarking performance.

namespace parallelogram {
namespace benchmarks {

Benchmarker::Benchmarker(const cl::Device* device) : device(device) { }

void Benchmarker::benchmark_area() const {
	//The polygon sizes we'll be testing with.
	const std::vector<size_t> sizes = {1, 10, 100, 1000, 10000, 20000, 40000, 80000, 160000, 320000, 640000, 1000000, 2000000, 4000000, 8000000};
	//How many repeats to perform. More increases accuracy of timing.
	constexpr unsigned int repeats = 50;

	//Debug output for progress reporting goes through std::cerr.
	std::cerr << "Area:   0%";

	//Results of the timing.
	std::vector<double> times(sizes.size(), 0.0);

	for(size_t size_index = 0; size_index < sizes.size(); size_index++) {
		//Generate a polygon of the appropriate size to test on.
		SimplePolygon polygon;
		for(size_t vertex = 0; vertex < sizes[size_index]; vertex++) {
			polygon.emplace_back(vertex, vertex);
		}

		unsigned long total_time = 0;
		unsigned long start_time = 0;
		unsigned long end_time = 0;
		for(unsigned int repeat = 0; repeat < repeats; repeat++) {
			if(!device) {
				start_time = clock();
				polygon.area_host();
				end_time = clock();
			} else {
				start_time = clock();
				polygon.area_opencl(*device);
				end_time = clock();
			}
			total_time += end_time - start_time;

			const int progress = (repeat + size_index * repeats) * 100 / (sizes.size() * repeats);
			std::cerr << "\b\b\b";
			if(progress < 10) {
				std::cerr << " ";
			}
			std::cerr << progress << "%";
		}
		times[size_index] = static_cast<double>(total_time) / CLOCKS_PER_SEC / repeats;
	}

	//Output the results to cout.
	std::string device_identifier = identifier();
	for(size_t size_index = 0; size_index < sizes.size(); size_index++) {
		if(!device) {
			std::cout << "area_host_time[std::make_pair(\"" << device_identifier << "\", " << sizes[size_index] << ")] = " << times[size_index] << ";" << std::endl;
		} else {
			std::cout << "area_opencl_time[std::make_pair(\"" << device_identifier << "\", " << sizes[size_index] << ")] = " << times[size_index] << ";" << std::endl;
		}
	}
	std::cerr << "\b\b\b\b100%" << std::endl;
}

void Benchmarker::device_statistics() const {
	std::string identity = identifier();
	const cl::Device* device_to_request = device ? device : &OpenCLDevices::getInstance().getCPUs()[0]; //Assume that the host is the first CPU.
	const std::vector<std::pair<std::string, cl_device_info>> information_to_request = { //Which info to request.
		{"device_type", CL_DEVICE_TYPE},
		{"compute_units", CL_DEVICE_MAX_COMPUTE_UNITS},
		{"items_per_compute_unit", CL_DEVICE_MAX_WORK_GROUP_SIZE},
		{"clock_frequency", CL_DEVICE_MAX_CLOCK_FREQUENCY},
		{"global_memory", CL_DEVICE_GLOBAL_MEM_SIZE},
		{"local_memory", CL_DEVICE_LOCAL_MEM_SIZE}
	};

	for(const std::pair<std::string, cl_device_info>& request : information_to_request) {
		cl_ulong result;
		if(device_to_request->getInfo(request.second, &result) != CL_SUCCESS) {
			throw ParallelogramException((std::string("Couldn't get information on device ") + identity + std::string(": ") + request.first).c_str());
		}
		std::cout << "device[\"" << identity << "\"][\"" << request.first << "\"] = " << result << ";" << std::endl;
	}
}

std::string Benchmarker::identifier() const {
	std::string result;
	const cl::Device* device_to_identify = device ? device : &OpenCLDevices::getInstance().getCPUs()[0]; //Assume that the host is the first CPU.
	if(device_to_identify->getInfo(CL_DEVICE_NAME, &result) != CL_SUCCESS) {
		return std::string("unknown");
	}
	trim(result);
	return result;
}

void Benchmarker::run() const {
	benchmark_area();
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
	parallelogram::OpenCLDevices& devices = parallelogram::OpenCLDevices::getInstance();
	for(const cl::Device& device : devices.getAll()) {
		parallelogram::benchmarks::Benchmarker benchmarker(&device);
		std::cerr << "Benchmarking: " << benchmarker.identifier() << std::endl;
		benchmarker.device_statistics();
		benchmarker.run();
	}

	parallelogram::benchmarks::Benchmarker benchmarker(nullptr);
	std::cerr << "Benchmarking: Host" << std::endl;
	benchmarker.run();

	return 0;
}