/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <iostream> //DEBUG!
#include <algorithm> //For find_if to trim whitespace.
#include <functional> //For cref to trim whitespace.
#include <fstream> //To read CPU information on Linux.
#ifdef _WIN32
	#include <windows.h> //To detect device information.
#endif
#include "DeviceStatistics.h"
#include "ParallelogramException.h" //For when we can't sample the device statistics.

namespace parallelogram {

DeviceStatistics::DeviceStatistics(const cl::Device* device) {
	if(device) { //An OpenCL device. Those are simple.
		if(device->getInfo(CL_DEVICE_TYPE, &device_type) != CL_SUCCESS) {
			throw ParallelogramException("Couldn't get device type from OpenCL.");
		}
		if(device->getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &compute_units) != CL_SUCCESS) {
			throw ParallelogramException("Couldn't get number of compute units from OpenCL.");
		}
		if(device->getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &items_per_compute_unit) != CL_SUCCESS) {
			throw ParallelogramException("Couldn't get number of work items from OpenCL.");
		}
		if(device->getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &clock_frequency) != CL_SUCCESS) {
			throw ParallelogramException("Couldn't get clock frequency from OpenCL.");
		}
		if(device->getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &global_memory) != CL_SUCCESS) {
			throw ParallelogramException("Couldn't get the global memory size from OpenCL.");
		}
		if(device->getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &local_memory) != CL_SUCCESS) {
			throw ParallelogramException("Couldn't get the local memory size from OpenCL.");
		}
	} else { //Querying the host.
		device_type = 2u; //Always a CPU.
		items_per_compute_unit = 1u; //Only one item per compute unit.
		global_memory = std::numeric_limits<unsigned long>::max(); //Allow infinite memory. The application will crash before it has a chance to split the data up anyway.
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
					compute_units = atoi(line.c_str());
					found_siblings = true;
					if(found_cpu_mhz && found_cache_size) {
						break; //Found everything.
					}
				}
				if(!found_cpu_mhz && line.find("cpu MHz") == 0) {
					line = line.substr(start_pos);
					if(line.find(".") != std::string::npos) {
						line = line.substr(0, line.find("."));
					}
					trim(line);
					clock_frequency = atoi(line.c_str());
					found_cpu_mhz = true;
					if(found_siblings && found_cache_size) {
						break; //Found everything.
					}
				}
				if(!found_cache_size && line.find("cache size") == 0) {
					const size_t kb_pos = line.find("KB");
					line = line.substr(start_pos, kb_pos - start_pos - 1);
					if(line.find(".") != std::string::npos) {
						line = line.substr(0, line.find("."));
					}
					trim(line);
					local_memory = atoi(line.c_str()) * 1024; //Because the file lists kilobytes.
					found_cache_size = true;
					if(found_siblings && found_cpu_mhz) {
						break; //Found everything.
					}
				}
			}
		} else {
#ifdef _WIN32
			SYSTEM_INFO system_info;
			GetNativeSystemInfo(&system_info);
			compute_units = system_info.dwNumberOfProcessors;

			HKEY hkey = 0;
			if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\SYSTEM\\CentralProcessor\\0"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
				throw ParallelogramException("Could not open the registry key to query the processor for clock frequency.");
			}
			DWORD buffer_size = 4;
			DWORD clock_frequency_word;
			if(RegQueryValueEx(hkey, TEXT("~MHz"), NULL, NULL, (LPBYTE)&clock_frequency_word, (LPDWORD)&buffer_size) != ERROR_SUCCESS) {
				RegCloseKey(hkey);
				throw ParallelogramException("Could not read registry value to query the processor for clock frequency.");
			}
			RegCloseKey(hkey);
			clock_frequency = clock_frequency_word;

			unsigned int ecx = 0;
			__asm__ volatile(
				"cpuid;"
				:"=c"(ecx)
				:"a"(0x80000005) //CPUID instruction to get Extended Set 5: L1 cache.
			);
			local_memory = (ecx & 0xFF) * 1024;
#endif
		}
	}
}

inline void DeviceStatistics::trim(std::string& input) const {
	const std::function<bool(char)> is_not_whitespace = [](char character) {
		return !std::isspace<char>(character, std::locale::classic()) && character != 0;
	};
	input.erase(input.begin(), std::find_if(input.begin(), input.end(), is_not_whitespace)); //Trim whitespace at the start.
	input.erase(std::find_if(input.rbegin(), input.rend(), is_not_whitespace).base(), input.end()); //Trim whitespace at the end.
}

}