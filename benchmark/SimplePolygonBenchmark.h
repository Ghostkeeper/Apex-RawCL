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
#include "SimplePolygon.h"

namespace parallelogram {

namespace benchmark {

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
	 * Constructs a regular n-gon.
	 *
	 * Use this as parameter for the constructor of `SimplePolygonBenchmark` to
	 * use regular n-gons as input for the benchmarks.
	 */
	static SimplePolygon regularNGon(const size_t size);

	/*
	 * Constructs a new benchmark.
	 *
	 * You can supply the parameters for the benchmark here.
	 * \param run A function that executes the method you wish to benchmark. The
	 * function must accept one input `SimplePolygon` instance. Any other
	 * parameters must be generated before the function. Keep in mind that this
	 * is the function that we benchmark, so keep overhead to an absolute
	 * minimum.
	 * \param input_sizes The sizes to run the benchmark on, in order to test
	 * performance at various complexities of input. It is assumed that the
	 * algorithm runs in at most quadratic time to this input size (for
	 * predicting the performance on different devices).
	 * \param construct_polygon The function to use to construct the input
	 * polygons of the provided input sizes.
	 */
	SimplePolygonBenchmark(const void(*run)(SimplePolygon), const std::vector<size_t> input_sizes, const SimplePolygon(*construct_polygon)(const size_t));

private:
	/*
	 * The function to use to construct the input polygons.
	 */
	const SimplePolygon(*construct_polygon)(const size_t);

	/*
	 * The sizes to run the benchmark on.
	 */
	const std::vector<size_t> input_sizes;

	/*
	 * The function that runs one test with a pre-generated polygon.
	 */
	const void(*run)(SimplePolygon);
};

}

}

#endif //SIMPLEPOLYGONBENCHMARK_H