/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef OPENCLDEVICES_H
#define OPENCLDEVICES_H

#include <functional> //For reference_wrapper to make the cpu_devices and gpu_devices link to all_devices.
#include <unordered_map> //To store the device identifier for each device.
#include <vector> //To store the platforms.
#include "DeviceStatistics.h" //To get the statistics on the devices.
#include "OpenCL.h" //To call the OpenCL API.

namespace apex {

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
	 *
	 * These pointers are referencing to the array returned by getAll().
	 */
	const std::vector<cl::Device*>& getCPUs() const;

	/*
	 * Get the GPU devices available to compute with.
	 *
	 * These pointers are referencing to the array returned by getAll().
	 */
	const std::vector<cl::Device*>& getGPUs() const;

	/*
	 * Get a device identifier, chosen by the manufacturer of the device.
	 *
	 * Devices should be uniquely identified by this identifier, but the string
	 * is chosen by the manufacturer so conflicts are theoretically possible. In
	 * practice, manufacturers tend to put their brand name in this identifier,
	 * so it should not occur.
	 *
	 * The identifier should be a human-readable string.
	 *
	 * If we can't detect the device identifier, ``unknown`` will be returned.
	 * \param device The device to get the identifier of. Use nullptr to obtain
	 * the identifier of the host CPU device.
	 * \return A device identifier for the specified device.
	 */
	const std::string& getIdentifier(const cl::Device* device) const;

	/*
	 * Gets the device statistics of the specified device.
	 *
	 * These device statistics are obtained when the devices are first detected
	 * and then cached.
	 * \param The device to get the statistics of. Use nullptr to obtain the
	 * statistics of the host CPU device.
	 * \return A ``DeviceStatistics`` object containing some statistics on the
	 * specified device.
	 */
	const DeviceStatistics<>& getStatistics(const cl::Device* device) const;

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
	std::vector<cl::Device*> cpu_devices;

	/*
	 * All detected GPU-type devices.
	 *
	 * This is used by the scheduler if it expects a task to be more suitable to
	 * a GPU-type device, for instance if the task parallelises well and doesn't
	 * have a lot of branching.
	 */
	std::vector<cl::Device*> gpu_devices;

	/*
	 * For each device as well as the host (nullptr) device, an identifier
	 * to identify the device with.
	 */
	std::unordered_map<const cl::Device*, std::string> identifiers;

	/*
	 * For each device as well as the host (nullptr) device, some relevant
	 * statistics about the device that could indicate what sort of performance
	 * to expect from the device.
	 */
	std::unordered_map<const cl::Device*, DeviceStatistics<>> statistics;

private:
	/*
	 * Gets the identifier of the host CPU.
	 *
	 * This is an operation that is quite dependant on the operating system.
	 * Currently, the following operating systems are supported:
	 * * Linux, using /proc/cpuinfo.
	 * * Windows, using the HARDWARE/DESCRIPTION/System/CentralProcessor/0
	 *   registry key.
	 */
	std::string getHostIdentifier() const;

	/*
	 * Trims whitespace at the beginning and ending of a string.
	 *
	 * This is a helper function to canonicalise CPU and GPU names. The string
	 * is modified in-place.
	 * \return The input but with the whitespace at the start and end removed.
	 */
	inline void trim(std::string& input) const;
};

}

#endif //OPENCLDEVICES_H

