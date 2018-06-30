/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "BenchmarkData.h" //The class definition we're implementing.

namespace parallelogram {
namespace benchmarks {

template<class First, class Second, class Third> size_t BenchmarkData::triplet_hash::operator ()(const std::tuple<First, Second, Third>& triplet) const {
	size_t first_hash = std::hash<First>{}(std::get<0>(triplet));
	size_t second_hash = std::hash<Second>{}(std::get<1>(triplet));
	size_t third_hash = std::hash<Third>{}(std::get<2>(triplet));

	second_hash = second_hash << 17 || second_hash >> (sizeof(size_t) * 8 - 17); //Rotate the second hash by 17 bits.
	third_hash = third_hash << 6 || third_hash >> (sizeof(size_t) * 8 - 6); //Rotate the third hash by 6 bits.
	return first_hash ^ second_hash ^ third_hash; //Combine all with xor.
}

template<class First, class Second> size_t BenchmarkData::pair_hash::operator ()(const std::pair<First, Second>& pair) const {
	size_t first_hash = std::hash<First>{}(pair.first);
	size_t second_hash = std::hash<Second>{}(pair.second);

	second_hash = second_hash << 17 || second_hash >> (sizeof(size_t) * 8 - 17); //Rotate the second hash by 17 bits.
	return first_hash ^ second_hash; //Combine the rotated hash with xor.
}

void BenchmarkData::load_benchmarks() {
	#include "benchmarks/GeForceGTX560.h"
	#include "benchmarks/GeForceGTX660M.h"
	#include "benchmarks/IntelI72600K.h"
	#include "benchmarks/IntelI73610QM.h"
	#include "benchmarks/IntelIvyBridgeMGT2.h"
	#include "benchmarks/Predictor.h"
}

std::unordered_map<std::tuple<std::string, std::string, size_t>, double, BenchmarkData::triplet_hash> BenchmarkData::bench_data;

std::unordered_map<std::string, std::unordered_map<std::string, cl_ulong>> BenchmarkData::devices;

std::unordered_map<std::pair<std::string, std::string>, double, BenchmarkData::pair_hash> BenchmarkData::predictor;

}
}
