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
#include "DeviceStatistics.h" //To split tasks up based on the available memory in devices.
#include "OpenCLContext.h" //To get the OpenCL context to run on.

namespace apex {

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
	template<typename GroperIterator> friend class SimplePolygonBatchGroper;
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

	/*
	 * Copies a batch of simple polygons.
	 *
	 * This does not copy the actual polygons. They are retained by reference.
	 * \param original The batch to copy.
	 */
	SimplePolygonBatch(const SimplePolygonBatch& original) :
		begin(original.begin),
		end(original.end),
		count(original.count),
		total_vertices(original.total_vertices) {
	}

	/*
	 * Moves the batch of simple polygons to a different memory location.
	 * \param original The batch to move.
	 */
	SimplePolygonBatch(SimplePolygonBatch&& original) :
		begin(std::move(original.begin)),
		end(std::move(original.end)),
		count(std::move(original.count)),
		total_vertices(std::move(original.total_vertices)) {
	}

	/*
	 * Assigns a copy of the batch to a different variable.
	 *
	 * This does not copy the actual polygons. They are retained by reference.
	 * \param other The batch to assign to oneself.
	 * \return A reference to this batch.
	 */
	SimplePolygonBatch& operator =(const SimplePolygonBatch& other) {
		begin = other.begin;
		end = other.end;
		count = other.count;
		total_vertices = other.total_vertices;
		return *this;
	}

	/*
	 * Compute the total surface area of the simple polygons.
	 * \param output A vector that will be filled with the resulting areas.
	 */
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
	 * In order to enable keeping batches on the device memory as long as
	 * possible, and to simplify some algorithms, each batch will only be as
	 * large as what can fit into the global memory of the device it is used on.
	 * Since the devices it is used on will not be known yet until they are
	 * used, the batches will get split up lazily upon use into subbatches. This
	 * stores these subbatches.
	 *
	 * From the outside, this subbatch system is completely transparent. The
	 * only way to notice is that in order to execute an algorithm, these
	 * subbatches have to be cycled in and out of the memory of the device, so
	 * the algorithm will run slower.
	 *
	 * When the batch is run on multiple different (non-host) devices, the batch
	 * size of the subbatches should end up at the smallest of their global
	 * memory sizes, so that it fits on all devices. Batches will not nest more
	 * than one layer.
	 */
	std::vector<SimplePolygonBatch<Iterator>> subbatches;

	/*
	 * The first element of a range of simple polygons to batch.
	 */
	Iterator begin;

	/*
	 * The last element of a range of simple polygons to batch.
	 */
	Iterator end;

	/*
	 * The total amount of simple polygons in this batch.
	 */
	size_t count;

	/*
	 * The total amount of vertices in the entire batch.
	 *
	 * This is used to choose the algorithms to operate on the batch, since each
	 * algorithm will have different ways to scale with the vertex count.
	 */
	size_t total_vertices;

	/*
	 * Compute the total surface area of the simple polygons using the host CPU.
	 * \param output A vector that will be filled with the resulting areas.
	 */
	void area_host(std::vector<area_t>& output) const {
		for(Iterator simple_polygon = begin; simple_polygon != end; std::advance(simple_polygon, 1)) {
			output.emplace_back(simple_polygon->area());
		}
	}

	/*
	 * Compute the total surface area of the simple polygons using an OpenCL
	 * device.
	 * \param output A vector that will be filled with the resulting areas.
	 */
	void area_opencl(const cl::Device& device, std::vector<area_t>& output) const {
		//TODO: Implement.
	}

	/*
	 * Splits this batch into subbatches such that the batch fits within a
	 * limited amount of memory.
	 *
	 * If the batch already fits in the available memory, it remains untouched.
	 * \param maximum_memory The amount of memory that the batches must fit in.
	 * \return Whether the splitting was successful. If there is a polygon in
	 * this batch that is too large to fit in maximum memory on its own, it will
	 * fail.
	 */
	bool ensure_fit(cl_ulong maximum_memory) {
		constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;

		//Check if it's even necessary to rebatch. Maybe it already fits in memory.
		if(subbatches.empty()) {
			if((total_vertices + count) * vertex_size <= maximum_memory) {
				return true; //Already fits. Don't need to do anything.
			}
		} else {
			bool rebatch_necessary = false;
			for(SimplePolygonBatch<Iterator>& subbatch : subbatches) {
				if((subbatch.total_vertices + subbatch.count) * vertex_size > maximum_memory) {
					rebatch_necessary = true;
					break;
				}
			}
			if(!rebatch_necessary) {
				return true; //Already fits.
			}
		}

		//Rebatch is necessary.
		subbatches.clear();
		Iterator batch_start = begin;
		cl_ulong batch_memory = 0;
		for(Iterator batch_end = begin; batch_end != end; std::advance(batch_end, 1)) {
			const cl_ulong poly_size = (batch_end->size() + 1) * vertex_size; //+1 for the end marker.
			if(batch_memory + poly_size <= maximum_memory) { //Next polygon would still fit.
				batch_memory += poly_size;
			} else { //Next polygon no longer fits.
				if(batch_memory == 0) { //The next polygon on its own is too large already.
					return false;
				}
				subbatches.emplace_back(batch_start, batch_end);
				batch_start = batch_end;
				batch_memory = 0;
			}
		}
		return true;
	}
};

}

#endif //SIMPLEPOLYGONBATCH_H