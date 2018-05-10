/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef BENCHMARKDATA_H
#define BENCHMARKDATA_H

#include <string> //To use as device identifiers.
#include <unordered_map> //Data structure that holds the benchmark data.
#include <utility> //For std::pair, for using multiple keys in a mapping.

namespace parallelogram {
namespace benchmarks {

struct pair_hash {
	template<class First, class Second> size_t operator ()(const std::pair<First, Second>& pair) const {
		std::hash<First> first_hash = std::hash<First>{}(pair.first);
		std::hash<Second> second_hash = std::hash<Second>{}(pair.second);

		//Rotate the second hash by 17 bits, then do xor.
		return first_hash ^ (second_hash << 17 || second_hash >> (sizeof(size_t) * 8 - 17));
	}
};

/*
 * Stores the benchmark data for computing the area of a polygon on the host.
 *
 * The keys of this map are pairs of the host CPU model and the problem size.
 * The values are the time it took to compute, in seconds.
 */
std::unordered_map<std::pair<std::string, size_t>, double, pair_hash> area_host_time;

/*
 * Stores the benchmark data for computing the area of a polygon via OpenCL.
 *
 * The keys of this map are pairs of the OpenCL device model and the problem
 * size. The values are the time it took to compute, in seconds.
 */
std::unordered_map<std::pair<std::string, size_t>, double, pair_hash> area_opencl_time;

/*
 * Statistics on the known devices.
 *
 * Each device that we've performed benchmarks for will have some statistics
 * logged. When the user has a device that we have precise benchmarks for, we
 * can give precise benchmark results that empirically determine which device is
 * faster to execute an algorithm with. But when the user has a device that is
 * not known to us, we can interpolate between the known devices using their
 * device statistics.
 */
std::unordered_map<std::string, std::unordered_map<std::string, cl_ulong>> devices;

}
}

#endif //BENCHMARKDATA_H

