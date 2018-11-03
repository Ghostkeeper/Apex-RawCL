/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGONBATCH_H
#define SIMPLEPOLYGONBATCH_H

#include <cstddef> //For size_t.
#include <iterator> //For iterating over a subset of a data structure.
#include "Benchmarks.h" //To choose the preferred algorithm and device.

namespace parallelogram {

class SimplePolygon;

/*
 * Class used to process operations on many simple polygons together.
 *
 * Performing the same instruction on many simple polygons could hide the
 * overhead associated with using some processing devices that have greater
 * performance otherwise.
 *
 * The methods of this class will act as if operating on all of the simple
 * polygons referred to by this batch separately. The results of these
 * operations will generally be stored in an output vector of the same size as
 * the input vector, where the result of each entry in the vector would be the
 * same result as if the method would be called separately on each polygon of
 * the input.
 *
 * The batch never stores a copy of the polygon data, except when it needs to
 * copy this data to another device for processing there.
 * \tparam Iterator The type of iterators to use. You must use an iterator that
 * supports equality tests. The iterator must be iterating over `SimplePolygon`
 * instances.
 */
template<typename Iterator>
class SimplePolygonBatch {
public:
	/*
	 * Batches a bunch of simple polygons together to operate on separately.
	 *
	 * In order to batch a group of simple polygons, you need to indicate a
	 * range in an iterable object that is supposed to be batched.
	 *
	 * Note that the performance of this iterator is crucial to the performance
	 * of the algorithms on the batch. Supplying an iterator of an unordered set
	 * would induce multiple iterations over the set, which has a lot of unused
	 * entries and would have worse performance when compared to, for instance,
	 * an iterator of a vector.
	 * \param begin The first element of a range of simple polygons to batch.
	 * \param end The element after the last element of the range of simple
	 * polygons to batch.
	 */
	SimplePolygonBatch(const Iterator begin, const Iterator end) :
		begin(begin),
		end(end),
		count(std::distance(begin, end)),
		total_vertices([begin, end]() {
			size_t result = 0;
			for(Iterator simple_polygon = begin; simple_polygon != end; std::advance(simple_polygon, 1)) {
				result += simple_polygon->size();
			}
			return result;
		}()) {
	}

	void area(std::vector<area_t>& output) {
		output.clear();
		output.reserve(count);

		const std::vector<std::string> options = {"area_opencl", "area_host"};
		const std::vector<size_t> problem_size = {count, total_vertices / count};
		const std::pair<std::string, const cl::Device*> best_choice = benchmarks::choose(options, problem_size);
		if(best_choice.first == "area_host" || !best_choice.second) {
			return area_host(output);
		} else {
			return area_opencl(*best_choice.second, output);
		}
	}

private:
	/*
	 * The first element of a range of simple polygons to batch.
	 */
	const Iterator begin;

	/*
	 * The last element of a range of simple polygons to batch.
	 */
	const Iterator end;

	/*
	 * The total amount of simple polygons in this batch.
	 */
	const size_t count;

	/*
	 * The total amount of vertices in the entire batch.
	 *
	 * This is used to choose the algorithms to operate on the batch, since each
	 * algorithm will have different ways to scale with the vertex count.
	 */
	const size_t total_vertices;

	void area_host(std::vector<area_t>& output) const {
		for(Iterator simple_polygon = begin; simple_polygon != end; std::advance(simple_polygon, 1)) {
			output.emplace_back(simple_polygon->area());
		}
	}

	void area_opencl(const cl::Device& device, std::vector<area_t>& output) const {
		//TODO: Implement.
	}
};

}

#endif //SIMPLEPOLYGONBATCH_H