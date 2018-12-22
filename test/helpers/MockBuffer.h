/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef MOCKBUFFER_H
#define MOCKBUFFER_H

#include "MockContext.h" //We can only create these buffers on a MockContext. They should not be used together with real OpenCL devices.
#include "OpenCL.h" //For cl_mem_flags.

namespace apex {

/*
 * Mimics the cl::Buffer class, so that your tests can run without running them
 * on actual OpenCL devices.
 *
 * This buffer simply keeps an internal array in memory onto which it can write
 * and from which it can read.
 */
class MockBuffer {
public:
	/*
	 * Creates a buffer. Needs to have the same syntax as
	 * ``cl::Buffer::Buffer()``.
	 */
	MockBuffer(const MockContext& context, const cl_mem_flags flags, const size_t size);

	/*
	 * Stores the data. For this mock, just on the host.
	 *
	 * This is allocated once, during the constructor.
	 */
	std::vector<uint8_t> data;
};

}

#endif //MOCKBUFFER_H