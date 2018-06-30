/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <algorithm> //For find_if to trim whitespace.
#include <fstream> //To read CPU information on Linux.
#include <functional> //For cref to trim whitespace.
#ifdef _WIN32
	#include <windows.h> //To detect the identifier of the host CPU.
#endif
#include "OpenCLDevices.h"

namespace parallelogram {

OpenCLDevices::OpenCLDevices() {
	//Detect the platforms on this computer.
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	/* We're only really interested in the devices.
	 * Platforms may have additional limitations (e.g. if they are remote and
	 * the bandwidth is limited. But this is not modelled here. */
	for(const cl::Platform& platform : platforms) {
		std::vector<cl::Device> cpus;
		platform.getDevices(CL_DEVICE_TYPE_CPU, &cpus);
		for(const cl::Device& cpu : cpus) {
			cpu_devices.push_back(cpu);
			all_devices.push_back(cpu);
		}
		std::vector<cl::Device> gpus;
		platform.getDevices(CL_DEVICE_TYPE_GPU, &gpus);
		for(const cl::Device& gpu : gpus) {
			gpu_devices.push_back(gpu);
			all_devices.push_back(gpu);
		}
	}
}

OpenCLDevices& OpenCLDevices::getInstance() {
	static OpenCLDevices instance; //Constructs using the default constructor.
	return instance;
}

const std::vector<cl::Device>& OpenCLDevices::getAll() const {
	return all_devices;
}

const std::vector<cl::Device>& OpenCLDevices::getCPUs() const {
	return cpu_devices;
}

const std::vector<cl::Device>& OpenCLDevices::getGPUs() const {
	return gpu_devices;
}

const std::string OpenCLDevices::getIdentifier(const cl::Device* device) const {
	return identifiers.find(device)->second;
}

std::string OpenCLDevices::getHostIdentifier() const {
	std::ifstream cpuinfo("/proc/cpuinfo"); //First try /proc/cpuinfo on Linux systems.
	if(cpuinfo.is_open()) { //Yes, is Linux!
		std::string line;
		while(std::getline(cpuinfo, line)) {
			if(line.find("model name") == 0) { //Parse this line.
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
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) { //Read the first core of the first processor. Assuming that's the host.
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

inline void OpenCLDevices::trim(std::string& input) const {
	const std::function<bool(char)> is_not_whitespace = [](char character) {
		return !std::isspace<char>(character, std::locale::classic()) && character != 0;
	};
	input.erase(input.begin(), std::find_if(input.begin(), input.end(), is_not_whitespace)); //Trim whitespace at the start.
	input.erase(std::find_if(input.rbegin(), input.rend(), is_not_whitespace).base(), input.end()); //Trim whitespace at the end.
}

}