/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <string> //To return the device identifiers.

namespace parallelogram {
namespace benchmarks {

class Benchmarker {
public:
	void benchmark_area();

private:
	/*
	 * Returns a string identifying the device that the host runs on.
	 */
	std::string host_identifier() const;

	/*
	 * Returns a string identifying the device that the GPU-targeted algorithms
	 * run on.
	 */
	std::string gpu_identifier() const;

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
}

#endif //BENCHMARKER_H