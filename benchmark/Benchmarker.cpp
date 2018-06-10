/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <algorithm> //For find_if to trim whitespace.
#include "Eigen/Core" //To perform calculations for interpolation between data points.
#include "Eigen/QR" //To perform QR resolution for linear least squares.
#include <fstream> //To read CPU information on Linux.
#include <functional> //For cref to trim whitespace.
#include <iostream> //To output the benchmark data to stdout.
#include <time.h> //For high-resolution timers to measure benchmarks.
#include <vector> //Lists of problem sizes to test with.
#ifdef _WIN32
	#include <windows.h> //To detect device information.
#endif
#include "BenchmarkData.h" //To use the pre-existing benchmark data to generate interpolation vectors.
#include "Benchmarker.h" //The header we're implementing.
#include "OpenCL.h" //To get device information.
#include "OpenCLDevices.h" //To find the identifiers of the devices the benchmark is performed on.
#include "ParallelogramException.h"
#include "Point2.h" //To construct vertices for polygons.
#include "SimplePolygon.h" //A class of which we're benchmarking performance.
#include "SimplePolygonBenchmark.h" //The benchmark runners.

namespace parallelogram {
namespace benchmarks {

const std::vector<SimplePolygonBenchmark> Benchmarker::device_benchmarks = {
	SimplePolygonBenchmark("area_opencl", [](const cl::Device* device, SimplePolygon& polygon) {polygon.area_opencl(*device);}),
};

const std::vector<SimplePolygonBenchmark> Benchmarker::host_benchmarks = {
	SimplePolygonBenchmark("area_host", [](const cl::Device* device, SimplePolygon& polygon) {polygon.area_host();})
};

Benchmarker::Benchmarker(const cl::Device* device) : device(device) { }

void Benchmarker::device_statistics() const {
	std::string identity = identifier();
	if(device) {
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
			if(device->getInfo(request.second, &result) != CL_SUCCESS) {
				throw ParallelogramException((std::string("Couldn't get information on device ") + identity + std::string(": ") + request.first).c_str());
			}
			std::cout << "devices[\"" << identity << "\"][\"" << request.first << "\"] = " << result << "u;" << std::endl;
		}
	} else { //Querying the host.
		std::cout << "devices[\"" << identity << "\"][\"device_type\"] = 2u;" << std::endl; //Always a CPU.
		std::cout << "devices[\"" << identity << "\"][\"items_per_compute_unit\"] = 1u;" << std::endl; //Only one item per compute unit.
		std::cout << "devices[\"" << identity << "\"][\"global_memory\"] = " << std::numeric_limits<size_t>::max() << "u;" << std::endl; //Allow infinite memory. The application will crash before it gets a chance to split the data up anyway.
		std::ifstream cpuinfo("/proc/cpuinfo"); //First try /proc/cpuinfo on Linux systems.
		if(cpuinfo.is_open()) { //Yes, is Linux!
			std::string line;
			bool found_siblings = false; //Find each item only once, even if there are multiple cores on this computer.
			bool found_cpu_mhz = false;
			bool found_cache_size = false;
			while(std::getline(cpuinfo, line)) {
				const size_t start_pos = line.find(":") + 2; //Only valid on lines with a colon in it.
				if(!found_siblings && line.find("siblings") == 0) {
					line = line.substr(start_pos);
					trim(line);
					std::cout << "devices[\"" << identity << "\"][\"compute_units\"] = " << line << "u;" << std::endl;
					found_siblings = true;
				}
				if(!found_cpu_mhz && line.find("cpu MHz") == 0) {
					line = line.substr(start_pos);
					if(line.find(".") != std::string::npos) {
						line = line.substr(0, line.find("."));
					}
					trim(line);
					std::cout << "devices[\"" << identity << "\"][\"clock_frequency\"] = " << line << "u;" << std::endl;
					found_cpu_mhz = true;
				}
				if(!found_cache_size && line.find("cache size") == 0) {
					const size_t kb_pos = line.find("KB");
					line = line.substr(start_pos, kb_pos - start_pos - 1);
					if(line.find(".") != std::string::npos) {
						line = line.substr(0, line.find("."));
					}
					trim(line);
					size_t local_memory_size = atoi(line.c_str());
					local_memory_size *= 1024; //Because the file lists kilobytes.
					std::cout << "devices[\"" << identity << "\"][\"local_memory\"] = " << local_memory_size << "u;" << std::endl;
					found_cache_size = true;
				}
			}
		} else {
#ifdef _WIN32
			SYSTEM_INFO system_info;
			GetNativeSystemInfo(&system_info);
			std::cout << "devices[\"" << identity << "\"][\"compute_units\"] = " << system_info.dwNumberOfProcessors << "u;" << std::endl;

			HKEY hkey = 0;
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\SYSTEM\\CentralProcessor\\0"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
				std::cerr << "Could not open registry key to query the processor for clock frequency." << std::endl;
				exit(1); //Could not open registry key. This is required.
			}
			DWORD buffer_size = 4;
			DWORD clock_frequency;
			if(RegQueryValueEx(hkey, TEXT("~MHz"), NULL, NULL, (LPBYTE)&clock_frequency, (LPDWORD)&buffer_size) != ERROR_SUCCESS) {
				std::cerr << "Could not read registry value to query the processor for clock frequency." << std::endl;
				RegCloseKey(hkey);
				exit(1); //Could not read registry key. This is required.
			}
			RegCloseKey(hkey);
			std::cout << "devices[\"" << identity << "\"][\"clock_frequency\"] = " << clock_frequency << "u;" << std::endl;

			unsigned int ecx = 0;
			__asm__ volatile(
				"cpuid;"
				:"=c"(ecx)
				:"a"(0x80000005) //CPUID instruction to get Extended Set 5: L1 cache.
			);
			std::cout << "devices[\"" << identity << "\"][\"local_memory\"] = " << (ecx & 0xFF) * 1024 << "u;" << std::endl;
#endif
		}
	}
}

std::string Benchmarker::identifier() const {
	if(device) {
		std::string result;
		if(device->getInfo(CL_DEVICE_NAME, &result) != CL_SUCCESS) {
			return std::string("unknown");
		}
		trim(result);
		return result;
	} else { //Querying the host.
		std::ifstream cpuinfo("/proc/cpuinfo"); //First try /proc/cpuinfo on Linux systems.
		if(cpuinfo.is_open()) { //Yes, is Linux!
			std::string line;
			while(std::getline(cpuinfo, line)) {
				if(line.find("model name") == 0) {
					const size_t start_pos = line.find(":") + 2;
					line = line.substr(start_pos);
					trim(line);
					return line;
				}
			}
			return "unknown";
		}
#ifdef _WIN32
		HKEY hkey = 0;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
			return "unknown"; //Could not open registry key.
		}
		DWORD buffer_size = 255;
		char value[255];
		if(RegQueryValueEx(hkey, TEXT("ProcessorNameString"), NULL, NULL, (LPBYTE)value, &buffer_size) != ERROR_SUCCESS) {
			RegCloseKey(hkey);
			return "unknown"; //Could not read registry value.
		}
		RegCloseKey(hkey);
		return value;
#endif
		return "unknown"; //Unknown operating system. I don't know how to query.
	}
}

void Benchmarker::run() const {
	if(device) {
		for(const SimplePolygonBenchmark& benchmark : device_benchmarks) {
			benchmark.benchmark(device, identifier());
		}
	} else {
		for(const SimplePolygonBenchmark& benchmark : host_benchmarks) {
			benchmark.benchmark(nullptr, identifier());
		}
	}
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
	benchmarker.device_statistics();
	benchmarker.run();

	for(const parallelogram::benchmarks::SimplePolygonBenchmark& benchmark : parallelogram::benchmarks::Benchmarker::device_benchmarks) {
		benchmark.compute_interpolation();
	}
	for(const parallelogram::benchmarks::SimplePolygonBenchmark& benchmark : parallelogram::benchmarks::Benchmarker::host_benchmarks) {
		benchmark.compute_interpolation();
	}

	return 0;
}