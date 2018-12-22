/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <cassert> //Checking that our memory access stays within the range of the buffer.
#include <cstring> //For memcpy.
#include "MockCommandQueue.h"
#include "MockBuffer.h" //The type of buffer that this queue can write and read.

namespace apex {

cl_int MockCommandQueue::enqueueWriteBuffer(MockBuffer& buffer, const cl_bool blocking, const size_t offset, const size_t size, const void* const source) {
	assert(offset + size < buffer.data.size()); //Make sure we're not writing out of bounds.

	std::memcpy(&buffer.data[offset], source, size);

	return CL_SUCCESS;
}

}