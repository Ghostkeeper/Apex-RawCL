/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "OpenCL.h" //To compute point-in-polygon via OpenCL.

#include "DeviceStatistics.h" //To get the capacities of the OpenCL devices to adjust the algorithm to them.
#include "OpenCLContext.h" //To get the OpenCL context to run on.
#include "ParallelogramException.h"
#include "SimplePolygon.h" //The class whose algorithms we're implementing.

namespace parallelogram {

bool SimplePolygon::contains_opencl(const cl::Device& device, const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	//TODO: If pre-calculation is allowed, obtain the AABB of the polygon and do that check first.

	if(size() == 2) {
		//This polygon is a line. Only report true if we consider edges inside and we're on the edge.
		return include_edge == EdgeInclusion::INSIDE && point.isLeftOfLineSegment(front(), back()) == 0;
	}
	if(size() == 1) {
		//This polygon is a point. Only report true if we consider edges inside and we're on the vertex.
		return include_edge == EdgeInclusion::INSIDE && point == front();
	}
	if(size() == 0) {
		return false;
	}

	cl::Context& context = OpenCLContext::getInstance().contexts[device];
	cl::CommandQueue queue(context, device);

	//Load the source code.
	cl::Program& program = OpenCLContext::getInstance().compile(device,
		#include "Contains.cl"
	);

	//We might need to make multiple passes if the device has a very limited amount of memory.
	DeviceStatistics statistics(&device);
	constexpr size_t vertex_size = sizeof(coord_t) * 2;
	const cl_ulong local_buffer_size = statistics.local_memory / vertex_size * vertex_size; //Make sure that the memory buffers hold an integer number of vertices.
	const cl_ulong global_buffer_size = statistics.global_memory / vertex_size * vertex_size;

	const size_t vertices_per_pass = global_buffer_size / vertex_size;
	int total_winding = 0; //Result sum of all passes.
	for(size_t pivot_vertex = 0; pivot_vertex < size(); pivot_vertex += vertices_per_pass - 1) { //If the total data size is more than what fits in constant memory, we'll have to make multiple passes.
		//Each item works on a line segment, which requires two vertices.
		//So we must leave space for 1 extra vertex in memory.
		size_t pivot_vertex_after = pivot_vertex + vertices_per_pass - 1; //-1 because the pivot vertex of the next pass is the last vertex of this pass.
		size_t vertices_this_pass = vertices_per_pass;
		if(pivot_vertex_after >= size()) {
			pivot_vertex_after = 0;
			vertices_this_pass = size() - pivot_vertex;
		}

		//Dividing the work over as many work groups as possible.
		size_t this_work_groups = std::min(static_cast<size_t>(statistics.compute_units), vertices_this_pass);
		size_t vertices_per_work_group = (vertices_this_pass + this_work_groups - 1) / this_work_groups;
		vertices_per_work_group = std::min(vertices_per_work_group, statistics.items_per_compute_unit); //However the work group size is limited by hardware and the number of compute units scales then.
		vertices_per_work_group = std::min(vertices_per_work_group, local_buffer_size / vertex_size); //We must also limit the work group size by the memory that the work groups can use locally.
		this_work_groups = (vertices_this_pass + vertices_per_work_group - 1) / vertices_per_work_group;
		//Round the global work size up to multiple of vertices_per_work_group. The kernel itself handles work items that need to idle.
		const size_t global_work_size = (vertices_this_pass + vertices_per_work_group - 1) / vertices_per_work_group * vertices_per_work_group;

		//Allocate constant memory on the device for the input.
		const cl_ulong this_constant_buffer_size = (vertices_this_pass + 1) * vertex_size;
		cl::Buffer input_points(context, CL_MEM_READ_ONLY, (global_work_size + 1) * vertex_size);
		queue.enqueueWriteBuffer(input_points, CL_TRUE, 0, this_constant_buffer_size - vertex_size, &(*this)[pivot_vertex]); //Write the polyline and first pivot vertex.
		queue.enqueueWriteBuffer(input_points, CL_TRUE, this_constant_buffer_size - vertex_size, vertex_size, &(*this)[pivot_vertex_after]); //Write the second pivot vertex.

		//Allocate an output buffer: One int for each work group as their output.
		cl_ulong this_output_buffer_size = this_work_groups * sizeof(cl_int);
		cl::Buffer output_winding_numbers(context, CL_MEM_WRITE_ONLY, this_output_buffer_size);

		//Call the kernel to compute the winding number of this polygon and add it to total_winding.
		cl::Kernel contains_kernel(program, "contains");
		contains_kernel.setArg(0, input_points);
		contains_kernel.setArg(1, vertices_this_pass);
		cl_int2 point_vector = {point.x, point.y};
		contains_kernel.setArg(2, point_vector);
		contains_kernel.setArg(3, include_edge == EdgeInclusion::INSIDE);
		contains_kernel.setArg(4, output_winding_numbers);
		contains_kernel.setArg(5, cl::Local(vertices_per_work_group * sizeof(cl_int)));

		queue.enqueueNDRangeKernel(contains_kernel, cl::NullRange, cl::NDRange(global_work_size), cl::NDRange(vertices_per_work_group));
		cl_int result = queue.finish(); //Let the device do its thing!
		if(result != CL_SUCCESS) {
			throw ParallelogramException("Error executing command queue for point-in-polygon computation.");
		}

		//Read the output data in.
		std::vector<int> winding_numbers;
		winding_numbers.resize(this_work_groups);
		queue.enqueueReadBuffer(output_winding_numbers, CL_TRUE, 0, this_output_buffer_size, &(winding_numbers[0]));
		queue.finish();
		for(const int winding_number : winding_numbers) {
			total_winding += winding_number;
		}
	}

	switch(fill_type) {
		default:
		case FillType::EVEN_ODD:
			return total_winding & 1;
		case FillType::NONZERO:
			return total_winding != 0;
	}
}

bool SimplePolygon::contains_host(const Point2& point, const EdgeInclusion& include_edge, const FillType& fill_type) const {
	//TODO: If pre-calculation is allowed, obtain the AABB of the polygon and do that check first.

	if(size() == 2) {
		//This polygon is a line. Only report true if we consider edges inside and we're on the edge.
		return include_edge == EdgeInclusion::INSIDE && point.isLeftOfLineSegment(front(), back()) == 0;
	}
	if(size() == 1) {
		//This polygon is a point. Only report true if we consider edges inside and we're on the vertex.
		return include_edge == EdgeInclusion::INSIDE && point == front();
	}
	if(size() == 0) {
		return false;
	}

	//This is the winding number algorithm to determine if a point is inside a polygon.
	int winding_number = 0;
	Point2 previous = back();
	for(size_t index = 0; index < size(); index++) //Take two adjacent vertices of the polygon.
	{
		const Point2& next = (*this)[index];
		/* Cast a ray from the point towards the right and figure out whether
		the line segment between these two vertices crosses it and in which
		direction. */
		if(previous.y < next.y) { //Rising edge.
			//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
			if(point.y >= previous.y && point.y < next.y) { //Crosses height of point.
				const coord_t point_is_left = point.isLeftOfLineSegment(previous, next);
				if(point_is_left > 0 || (point_is_left == 0 && include_edge == EdgeInclusion::INSIDE)) { //Line is absolutely right of point. Point is relatively left of line.
					winding_number++;
				}
			}
		} else if(previous.y > next.y) { //Falling edge (next vertex is lower than previous vertex).
			//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
			if(point.y < previous.y && point.y >= next.y) { //Crosses height of point.
				const coord_t point_is_left = point.isLeftOfLineSegment(previous, next);
				if(point_is_left < 0 || (point_is_left == 0 && include_edge == EdgeInclusion::OUTSIDE)) { //Line is absolutely right of point. Point is relatively right of line.
					winding_number--;
				}
			}
		} else if(previous.y == point.y) { //Horizontal line at exactly the height of the point.
			if(previous.x < next.x && point.x >= previous.x && point.x <= next.x) { //Going to the left.
				if(include_edge == EdgeInclusion::OUTSIDE) {
					winding_number--;
				}
			} else if(previous.x >= next.x && point.x <= previous.x && point.x >= next.x) { //Going to the right.
				if(include_edge == EdgeInclusion::INSIDE) {
					winding_number++;
				}
			}
		}

		previous = next;
	}

	switch(fill_type) {
		default:
		case FillType::EVEN_ODD:
			return winding_number & 1;
		case FillType::NONZERO:
			return winding_number != 0;
	}
}

}