/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <cassert>
#include "MockBuffer.h"

namespace apex {

MockBuffer::MockBuffer(const MockContext& context, const cl_mem_flags flags, const size_t size) {
	data.resize(size); //Fill buffer with 0's.
}

cl_int MockBuffer::getInfo(cl_mem_info name, size_t* output) {
	assert(name == CL_MEM_SIZE);
	*output = data.capacity();
	return CL_SUCCESS;
}

}