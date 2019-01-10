/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef APEX_DEVICE_H
#define APEX_DEVICE_H

#include "OpenCL.h" //For the wrapped cl::Device.

namespace apex {

/*
 * Wrapper for OpenCL devices that also tracks which data buffers are stored on
 * the device at the moment.
 *
 * This is important for performance, since keeping the data on the device when
 * performing multiple operations on it may prevent having to copy it back and
 * forth. This is crucial because data transfer to the device is usually the
 * bottleneck.
 */
template<class CLDevice = cl::Device>
class Device {
public:
	/*
	 * Constructs the wrapper around the device.
	 *
	 * This assumes that no data has been stored on the device yet.
	 */
	Device(CLDevice device);
private:
	/*
	 * The OpenCL device that this wrapper wraps around.
	 */
	CLDevice device;
};

}

#endif //APEX_DEVICE_H