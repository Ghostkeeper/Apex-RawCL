/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef OPENCLDEVICES_H
#define OPENCLDEVICES_H

#include <vector> //To store the platforms.
#include "OpenCL.h" //To call the OpenCL API.

namespace parallelogram {

/*
 * Proxy for getting the available OpenCL devices.
 *
 * This is a caching proxy, so that the devices only need to be found once. The
 * actual device detection is done upon first getting the devices and then
 * cached for performance.
 */
class OpenCLDevices {
public:
	/*
	 * Statically gets the instance of this class.
	 *
	 * Since this class is a singleton, there can be only one instance of this
	 * class.
	 *
	 * Upon first calling this function, the OpenCL devices will be detected on
	 * the computer.
	 */
	static OpenCLDevices& getInstance();

	/*
	 * Get all devices available to compute with.
	 */
	const std::vector<cl::Device>& getAll() const;

	/*
	 * Get the CPU devices available to compute with.
	 */
	const std::vector<cl::Device>& getCPUs() const;

	/*
	 * Get the GPU devices available to compute with.
	 */
	const std::vector<cl::Device>& getGPUs() const;

	/*
	 * Since this is a singleton, the copy constructor should not be
	 * implemented.
	 */
	OpenCLDevices(const OpenCLDevices& original) = delete;

	/*
	 * Since this is a singleton, the copy assignment operator should not be
	 * implemented.
	 */
	void operator =(const OpenCLDevices& original) = delete;

protected:
	/*
	 * Creates a new instance of the OpenCL devices manager.
	 *
	 * During construction the OpenCL devices on this system will be detected.
	 */
	OpenCLDevices();

	/*
	 * All detected devices.
	 */
	std::vector<cl::Device> all_devices;

	/*
	 * All detected CPU-type devices.
	 *
	 * This is used by the scheduler if it expects a task to be more suitable to
	 * a CPU-type device, for instance if the task is not well parallelised, or
	 * requires lots of branching.
	 */
	std::vector<cl::Device> cpu_devices;

	/*
	 * All detected GPU-type devices.
	 *
	 * This is used by the scheduler if it expects a task to be more suitable to
	 * a GPU-type device, for instance if the task parallelises well and doesn't
	 * have a lot of branching.
	 */
	std::vector<cl::Device> gpu_devices;
};

}

#endif //OPENCLDEVICES_H

