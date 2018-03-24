/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef PARALLELOGRAMEXCEPTION_H
#define PARALLELOGRAMEXCEPTION_H

#include <exception> //To extend from the built-in exception class.

namespace parallelogram {

/*
 * All exceptions raised by this library should be instances of this struct.
 *
 * This allows users of the library to catch this exception in order to filter
 * out problems with calls to Parallelogram.
 */
struct ParallelogramException : public std::exception {
public:
	/*
	 * Constructs the exception instance with a specified message.
	 */
	ParallelogramException(const char* message);

	/*
	 * Returns the message stored with this exception.
	 */
	const char* what();

private:
	/*
	 * The error message stored with this exception.
	 */
	const char* message;
};

}

#endif //PARALLELOGRAMEXCEPTION_H