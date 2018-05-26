/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGONBENCHMARK_H
#define SIMPLEPOLYGONBENCHMARK_H

#include <vector>

/*
 * This class holds the parameters to run a benchmark to measure the performance
 * of a method of SimplePolygon.
 *
 * It functions as a factory for SimplePolygon instances, since the benchmark
 * will get run on polygons of different sizes. And it automates the running of
 * a benchmark.
 */
class SimplePolygonBenchmark {
public:
	/*
	 * Constructs a new benchmark.
	 *
	 * You can supply the parameters for the benchmark here.
	 * \param input_sizes The sizes to run the benchmark on, in order to test
	 * performance at various complexities of input. It is assumed that the
	 * algorithm runs in at most quadratic time to this input size (for
	 * predicting the performance on different devices).
	 */
	SimplePolygonBenchmark(std::vector<size_t> input_sizes);

private:
	/*
	 * The sizes to run the benchmark on.
	 */
	std::vector<size_t> input_sizes;
};

#endif //SIMPLEPOLYGONBENCHMARK_H