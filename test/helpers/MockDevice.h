/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef MOCKDEVICE_H
#define MOCKDEVICE_H

#include <cstddef> //For std::size_t, for the unique device ID.
#include <functional> //For adding specialisation to std::hash.
#include "OpenCL.h" //For all the constants and type declarations in there.

namespace apex {

/*
 * Mock for cl::Device.
 */
class MockDevice {
public:
	/*
	 * An identifier that is unique to this device, so that it can be
	 * disambiguated from other mock devices.
	 *
	 * Note that this device ID may get collisions with actual cl::Device
	 * instances.
	 */
	size_t device_id;

	/*
	 * The device type of the mock device.
	 *
	 * Normally this won't really matter for our tests.
	 */
	cl_device_type device_type = CL_DEVICE_TYPE_CPU;

	/*
	 * The number of compute units or logical cores in the mock device.
	 *
	 * The default is chosen to keep tests small but meaningful.
	 */
	cl_uint compute_units = 2;

	/*
	 * How many items a compute unit can process at the same time.
	 *
	 * The default is chosen so that it doesn't need a lot of data to set up a
	 * test.
	 */
	size_t items_per_compute_unit = 8;

	/*
	 * How many millions of clock cycles the mock device can handle per second.
	 *
	 * This won't really matter for our tests.
	 */
	cl_uint clock_frequency = 10;

	/*
	 * How much global memory is available on the device, in bytes.
	 *
	 * The default is chosen to keep the tests small but meaningful.
	 */
	cl_ulong global_memory = 1024;

	/*
	 * How much local memory is available on the device, in bytes.
	 *
	 * The default is chosen to keep the tests small but meaningful.
	 */
	cl_ulong local_memory = 256;

	/*
	 * Constructor that assigns this mock device a unique ID so that it can be
	 * stored in a hash table.
	 */
	MockDevice();

	/*
	 * Checks whether two mock device instances are the same device, by their
	 * device ID.
	 */
	bool operator ==(const MockDevice& other) const;

	/*
	 * Mocks getting info about the device.
	 *
	 * This info is set in the following fields of the mock device:
	 * - device_type
	 * - compute_units
	 * - items_per_compute_unit
	 * - clock_frequency
	 * - global_memory
	 * - local_memory
	 *
	 * To change the info that is to be returned by this function, change the
	 * appropriate fields before calling the ``getInfo`` function.
	 */
	template<typename T> cl_int getInfo(const cl_device_info name, T* output) const;

private:
	/*
	 * The device ID to assign to the mock device that is constructed next.
	 */
	static size_t next_device_id;
};

}

namespace std {

/*
 * Hashes MockDevices.
 *
 * Each ``MockDevice`` instance gets a unique ID by which it is hashed.
 */
template<> struct hash<apex::MockDevice> {
	size_t operator ()(const apex::MockDevice& device) const;
};

}

#endif //MOCKDEVICE_H