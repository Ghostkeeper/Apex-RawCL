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
	 * Divide the polygons in this batch over a number of work groups.
	 *
	 * If the number of polygons is too great to fit in the local memory of a
	 * work group, some polygons or some part of some polygon will have to be
	 * processed by a different work group. This function determines which
	 * polygons will get processed by which work group.
	 *
	 * Similarly, if the global memory is limited such that not all input data
	 * is able to fit, the work groups will have to be divided over a number of
	 * passes where the data is cycled through global memory. This function also
	 * computes the number of work groups that can be processed in each pass.
	 *
	 * This version divides up the data such that every edge is situated in some
	 * work group. That means that some vertices will be placed twice: Once for
	 * the endpoint of one edge and once for the starting point of another edge.
	 *
	 * The function also allows for a certain overhead per polygon in global
	 * memory. This is intended to allow storing some extra data such as the
	 * output of the kernel.
	 * \param statistics The statistics of the device we'll be computing with.
	 * \param global_overhead_per_polygon How much global memory to reserve.
	 * This limits the number of work groups that can be stored per pass.
	 * \param start_positions Output parameter to store the starting positions
	 * of each polygon in each work group. The result contains one vector for
	 * each work group. In each vector is the starting positions of a number of
	 * polygons in the buffer, divided by the size of a vertex. So in effect,
	 * this is the number of vertices that precede the polygon in the buffer of
	 * its work group.
	 * \param work_groups_per_pass Output parameter to store the number of work
	 * groups that can be computed per pass.
	 */
	void divide_edges(const DeviceStatistics& statistics, const cl_ulong global_overhead_per_polygon, std::vector<std::vector<size_t>>& start_positions, std::vector<size_t>& work_groups_per_pass) {
		constexpr cl_ulong vertex_size = sizeof(coord_t) * 2;
		const size_t vertices_per_work_group = std::min(statistics.items_per_compute_unit, statistics.local_memory / vertex_size);

		start_positions.reserve(count * 3 / vertices_per_work_group + 1); //Estimate of how many work groups we'll need in total. If there's that many polygons, they are probably triangles!
		start_positions.emplace_back(); //Need at least one group if the batch is not empty.
		work_groups_per_pass.reserve(start_positions.capacity() / statistics.compute_units + 1); //Estimate of how many passes we'll need. This estimate assumes that all work groups simply fit in global memory which will usually be the case.

		size_t current_position = 0; //Counts up to vertices_per_workgroup.
		cl_ulong global_memory_used = 0; //This pass.
		size_t work_groups_this_pass = 0;
		for(Iterator polygon = begin; polygon != end; std::advance(polygon, 1)) {
			while(current_position + 1 >= vertices_per_work_group) { //Doesn't fit in this work group any more. Start a new one.
				const cl_ulong global_memory_this_work_group = (start_positions.back().size() + 1) * global_overhead_per_polygon + (current_position + 1) * vertex_size;
				if(global_memory_used + global_memory_this_work_group > statistics.global_memory) {
					//Doesn't fit in this pass any more. Start a new one.
					work_groups_per_pass.emplace_back(work_groups_this_pass);
					global_memory_used = 0;
					work_groups_this_pass = 0;
				}
				//TODO: This makes the assumption that at least one work group fits in global memory. Safe assumption?
				work_groups_this_pass++;
				global_memory_used += global_memory_this_work_group;

				current_position -= vertices_per_work_group + 1; //One extra as pivot vertex.
				start_positions.emplace_back();
			}
			start_positions.back().emplace_back(current_position);
			current_position += polygon->size() + 1; //One extra to close the polygon.
		}
		work_groups_per_pass.emplace_back(work_groups_this_pass);
	}
};

}

#endif //SIMPLEPOLYGONBATCH_H