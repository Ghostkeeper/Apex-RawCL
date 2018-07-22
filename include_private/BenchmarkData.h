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
#include <utility> //For std::pair and std::tuple, for using multiple keys in a mapping.
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
	 * Hash function that can be used to store triplet tuples as keys in
	 * mappings.
	 *
	 * This combines the three hashes of the triplet's values in a way that
	 * spreads out the hashes reasonably to minimise collisions. It's not
	 * cryptographically safe but it does the job fast.
	 */
	struct triplet_hash {
		/*
		 * Create the hash.
		 * \param triplet The tuple to create the hash for.
		 */
		template<class First, class Second, class Third> size_t operator ()(const std::tuple<First, Second, Third>& triplet) const {
			size_t first_hash = std::hash<First>{}(std::get<0>(triplet));
			size_t second_hash = std::hash<Second>{}(std::get<1>(triplet));
			size_t third_hash = std::hash<Third>{}(std::get<2>(triplet));

			second_hash = second_hash << 17 || second_hash >> (sizeof(size_t) * 8 - 17); //Rotate the second hash by 17 bits.
			third_hash = third_hash << 6 || third_hash >> (sizeof(size_t) * 8 - 6); //Rotate the third hash by 6 bits.
			return first_hash ^ second_hash ^ third_hash; //Combine all with xor.
		};
	};

	/*
	 * Hash function that can be used to store pairs as keys in mappings.
	 *
	 * This combines the two hashes of the pair's values in a way that spreads
	 * out the hashes reasonably to minimise collisions. It's not
	 * cryptographically safe but it does the job fast.
	 */
	struct pair_hash {
		/*
		 * Create the hash.
		 * \param pair The pair to create the hash for.
		 */
		template<class First, class Second> size_t operator ()(const std::pair<First, Second>& pair) const {
			size_t first_hash = std::hash<First>{}(pair.first);
			size_t second_hash = std::hash<Second>{}(pair.second);

			second_hash = second_hash << 17 || second_hash >> (sizeof(size_t) * 8 - 17); //Rotate the second hash by 17 bits.
			return first_hash ^ second_hash; //Combine the rotated hash with xor.
		};
	};

public:
	/*
	 * Stores for each benchmark test the time that it took to run that
	 * benchmark.
	 *
	 * This stores for each test (first argument of the tuple key) for each
	 * device (second argument) and for each test size (third argument) how long
	 * it took to run that test (the values of the dictionary).
	 */
	std::unordered_map<std::tuple<std::string, std::string, size_t>, double, triplet_hash> bench_data;

	/*
	 * Statistics on the known devices.
	 *
	 * Each device that we've performed benchmarks for will have some statistics
	 * logged. When the user has a device that we have precise benchmarks for,
	 * we can give precise benchmark results that empirically determine which
	 * device is faster to execute an algorithm with. But when the user has a
	 * device that is not known to us, we can interpolate between the known
	 * devices using their device statistics.
	 */
	std::unordered_map<std::string, std::unordered_map<std::string, cl_ulong>> devices;

	/*
	 * Prediction vector for the time it'll take to compute area on an OpenCL
	 * device.
	 *
	 * This predictor gets filled with several properties of OpenCL devices as
	 * keys. If you then multiply the value of your OpenCL device for each of
	 * these keys with the corresponding values and add them together, you'll
	 * arrive at a prediction of how long the algorithm will take to execute
	 * based on a linear least-squares fit of the known benchmarks.
	 *
	 * The key in this map is a pair consisting of, firstly, the algorithm that
	 * it predicts the runtime of, and secondly the property of the device that
	 * it scales with. The values are the scaling differentials.
	 */
	std::unordered_map<std::pair<std::string, std::string>, double, pair_hash> predictor;

	/*
	 * Statically gets the instance of this class.
	 *
	 * Since this class is a singleton, there can be only one instance of this
	 * class.
	 *
	 * Upon first calling this function, the benchmark data will be loaded.
	 */
	static BenchmarkData& getInstance();

	/*
	 * Loads the data of all the benchmarks.
	 *
	 * After running this, the bench_data map contains the data of all
	 * benchmarks included with this application.
	 */
	static void load_benchmarks();

private:
	/*
	 * Constructs the singleton instance of BenchmarkData.
	 *
	 * This loads the pre-generated benchmark data. After running this, the
	 * fields of the class are filled with data of all benchmarks included with
	 * this application.
	 */
	BenchmarkData();
};

}
}

#endif //BENCHMARKDATA_H

