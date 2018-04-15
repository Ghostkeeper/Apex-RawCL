/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

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

}