/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef MOCKCOMMANDQUEUE_H
#define MOCKCOMMANDQUEUE_H

#include "OpenCL.h" //For cl_int and cl_bool.

namespace apex {

class MockBuffer;

/*
 * Mocks the cl::CommandQueue class in order to not actually send commands to
 * OpenCL.
 *
 * This mock records the commands given to the queue.
 */
class MockCommandQueue {
public:
	/*
	 * Mocks enqueueing a command to write to a buffer.
	 *
	 * The signature of this command must be the same as
	 * ``cl::CommandQueue::enqueueWriteBuffer``.
	 *
	 * This method only works with ``MockBuffer`` instances. It will write to
	 * the internal buffer of that class. It will also assert that all writes
	 * stay inside that buffer, so that we will discover errors due to writing
	 * out of bounds.
	 */
	cl_int enqueueWriteBuffer(MockBuffer& buffer, const cl_bool blocking, const size_t offset, const size_t size, const void* const source);
};

}

#endif //MOCKCOMMANDQUEUE_H