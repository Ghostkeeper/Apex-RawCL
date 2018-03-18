/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef OPENCLDEVICES_H
#define OPENCLDEVICES_H

namespace parallelogram {

/*
 * Proxy for getting the available OpenCL devices.
 *
 * This is a caching proxy, so that the devices only need to be found once. The
 * actual device detection is done upon first getting the devices and then
 * cached for performance.
 *
 * It is also a protecting proxy, protecting the OpenCL devices from multi-
 * threaded access if the OpenCL API version is less than 1.0. If the OpenCL API
 * version is 1.1, no such protection is needed since the API specifies that all
 * API calls must be thread-safe, so it is then left to the OpenCL
 * implementation on your operating system to limit concurrency where necessary.
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
};

}

#endif //OPENCLDEVICES_H

