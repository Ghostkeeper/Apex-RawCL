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
#include "OpenCL.h" //For cl_ulong, the data type for device data.

namespace parallelogram {
namespace benchmarks {

/*
 * Holds the results of benchmarks so that they may be used to determine the
 * best strategy for solving a task.
 */
class BenchmarkData {
private:
	/*
	 * Hash function that can be used to store triplet tuples as keys in mappings.
	 *
	 * This combines the three hashes of the triplet's values in a way that spreads
	 * out the hashes reasonably to minimise collisions. It's not cryptographically
	 * safe but it does the job fast.
	 */
	struct triplet_hash {
		template<class First, class Second, class Third> size_t operator ()(const std::tuple<First, Second, Third>& triplet) const;
	};

public:
	/*
	 * Stores for each benchmark test the time that it took to run that benchmark.
	 *
	 * This stores for each test (first argument of the tuple key) for each device
	 * (second argument) and for each test size (third argument) how long it took to
	 * run that test (the values of the dictionary).
	 */
	static std::unordered_map<std::tuple<std::string, std::string, size_t>, double, triplet_hash> bench_data;

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
	static std::unordered_map<std::string, std::unordered_map<std::string, cl_ulong>> devices;

	/*
	 * Prediction vector for the time it'll take to compute area on an OpenCL
	 * device.
	 *
	 * This predictor gets filled with several properties of OpenCL devices as keys.
	 * If you then multiply the value of your OpenCL device for each of these keys
	 * with the corresponding values and add them together, you'll arrive at a
	 * prediction of how long the algorithm will take to execute based on a linear
	 * least-squares fit of the known benchmarks.
	 */
	static std::unordered_map<std::string, double> area_opencl_predictor;

	/*
	 * Loads the data of all the benchmarks.
	 *
	 * After running this, the bench_data map contains the data of all
	 * benchmarks included with this application.
	 */
	static void load_benchmarks();
};

}
}

#endif //BENCHMARKDATA_H

