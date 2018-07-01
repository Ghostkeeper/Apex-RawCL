/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef DEVICESTATISTICS_H
#define DEVICESTATISTICS_H

#include "OpenCL.h" //To obtain the statistics from OpenCL devices.

namespace parallelogram {

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
	DeviceStatistics(const cl::Device* device);

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
	inline void trim(std::string& input) const;
};

}

#endif //DEVICESTATISTICS_H