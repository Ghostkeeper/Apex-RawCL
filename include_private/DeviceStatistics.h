/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef DEVICESTATISTICS_H
#define DEVICESTATISTICS_H

#include <algorithm> //For find_if to trim whitespace.
#include <functional> //For cref to trim whitespace.
#include <fstream> //To read CPU information on Linux.
#ifdef _WIN32
	#include <windows.h> //To detect device information.
#endif
#include "ApexException.h" //For when we can't sample the device statistics.
#include "DeviceStatistics.h"
#include "OpenCL.h" //To obtain the statistics from OpenCL devices.

namespace apex {

/*
 * Data holder for some statistics of compute devices.
 *
 * These statistics can be used to predict how well a device would perform to
 * execute a certain task, and then choose the best algorithm for that device or
 * the best device for a task.
 */
struct DeviceStatistics {
public:
	/*
	 * Obtain the device statistics from a device.
	 *
	 * Use the nullptr device to get these statistics for the host device.
	 * \param device The device to get the statistics of.
	 */
	DeviceStatistics(const cl::Device* device) {
		if(device) { //An OpenCL device. Those are simple.
			if(device->getInfo(CL_DEVICE_TYPE, &device_type) != CL_SUCCESS) {
				throw ApexException("Couldn't get device type from OpenCL.");
			}
			if(device->getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &compute_units) != CL_SUCCESS) {
				throw ApexException("Couldn't get number of compute units from OpenCL.");
			}
			if(device->getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &items_per_compute_unit) != CL_SUCCESS) {
				throw ApexException("Couldn't get number of work items from OpenCL.");
			}
			if(device->getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &clock_frequency) != CL_SUCCESS) {
				throw ApexException("Couldn't get clock frequency from OpenCL.");
			}
			if(device->getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &global_memory) != CL_SUCCESS) {
				throw ApexException("Couldn't get the global memory size from OpenCL.");
			}
			if(device->getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &local_memory) != CL_SUCCESS) {
				throw ApexException("Couldn't get the local memory size from OpenCL.");
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
					throw ApexException("Could not open the registry key to query the processor for clock frequency.");
				}
				DWORD buffer_size = 4;
				DWORD clock_frequency_word;
				if(RegQueryValueEx(hkey, TEXT("~MHz"), NULL, NULL, (LPBYTE)&clock_frequency_word, (LPDWORD)&buffer_size) != ERROR_SUCCESS) {
					RegCloseKey(hkey);
					throw ApexException("Could not read registry value to query the processor for clock frequency.");
				}
				RegCloseKey(hkey);
				clock_frequency = clock_frequency_word;

				uint32_t l1_cache_size = 0; //Try AMD first, because it's the simplest.
				asm volatile(
					"cpuid;" : "=c"(l1_cache_size) : "a"(0x80000005) //CPUID instruction to get Extended Set 5: L1 cache.
				);
				l1_cache_size = (l1_cache_size & 0xFF) * 1024;
				if(l1_cache_size == 0) { //Not AMD. Try Intel.
					//Intel lists their cache sizes under the ecx register of 0x4.
					//It has an arbitrary number of subleafs for an arbitrary number of caches.
					//We have to keep trying until we find the correct one.
					for(unsigned int cache_index = 0; cache_index < 255; cache_index++) { //Keep reading CPUID entries until we find the L1 cache (limit of 255 for safety).
						uint32_t eax;
						uint32_t ebx;
						uint32_t ecx;
						asm volatile (
							"cpuid;" : "=a"(eax), "=b"(ebx), "=c"(ecx) : "a"(0x4), "c"(cache_index) : "edx"
						);
						/* The EAX register looks like this. Unmarked bits are irrelevant.
						 * 00011100000000000100000100100001
						 *                                ^ Intel identifier.
						 *                             ^^^ Cache sharing strategy identifier.
						 *                         ^^^ Cache level. Lower tends to be faster and smaller.
						 *                       ^ Associativity flag.
						 *       ^^^^^^^^^^^^ Thread count.
						 * EBX contains the line size and partition count.
						 * ECX contains the line count.
						 */
						if((eax & 0x1F) == 0) { //No more cache levels to read. We couldn't find it.
							break;
						}
						const uint32_t level = ((eax >> 5) & 7) - 1;
						if(level == 0) { //Found the level that we're searching for!
							const uint32_t threads = ((eax >> 14) & 0xFFF) + 1;
							const uint32_t associativity = (eax & 0x200) ? 0xFF : ((ebx >> 22) + 1);
							const uint32_t line_size = (ebx & 0xFFF) + 1;
							const uint32_t line_partitions = ((ebx >> 12) & 0x3FF) + 1;
							uint32_t this_cache_size = ((associativity < 0xFF) ? (ecx + 1) * associativity : (ecx + 1)) * line_size * line_partitions / threads;
							if((eax & 0xF) == 3) { //Cache is unified with other cores. Then only half of the cache is actually ours.
								this_cache_size /= 2;
							}
							l1_cache_size += this_cache_size;
						}
					}
				}
				local_memory = l1_cache_size;
	#else
				//Unknown.
				compute_units = 1;
				clock_frequency = 0;
				local_memory = 0;
	#endif
			}
		}
	}

	/*
	 * The type of device.
	 *
	 * This must be either CL_DEVICE_TYPE_CPU or CL_DEVICE_TYPE_GPU.
	 */
	cl_device_type device_type;

	/*
	 * The number of compute units or logical cores in the device.
	 */
	cl_uint compute_units;

	/*
	 * How many items a compute unit can process at the same time.
	 */
	size_t items_per_compute_unit;

	/*
	 * How many millions of clock cycles the device can handle per second.
	 *
	 * This is in MHz.
	 */
	cl_uint clock_frequency;

	/*
	 * How much global memory is available on the device, in bytes.
	 */
	cl_ulong global_memory;

	/*
	 * How much local memory is available on each compute unit of the device, in
	 * bytes.
	 */
	cl_ulong local_memory;

private:
	/*
	 * Trims whitespace at the beginning and ending of a string.
	 *
	 * This is a helper function to canonicalise CPU and GPU names. The string
	 * is modified in-place.
	 * \return The input but with the whitespace at the start and end removed.
	 */
	inline void trim(std::string& input) const {
		const std::function<bool(char)> is_not_whitespace = [](char character) {
			return !std::isspace<char>(character, std::locale::classic()) && character != 0;
		};
		input.erase(input.begin(), std::find_if(input.begin(), input.end(), is_not_whitespace)); //Trim whitespace at the start.
		input.erase(std::find_if(input.rbegin(), input.rend(), is_not_whitespace).base(), input.end()); //Trim whitespace at the end.
	}
};

}

#endif //DEVICESTATISTICS_H