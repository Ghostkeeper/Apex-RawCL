/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <algorithm> //For std::min.
#include "OpenCL.h" //To call the OpenCL API.

#include "DeviceStatistics.h" //To get the capacities of the OpenCL devices to adjust the algorithm to them.
#include "OpenCLContext.h" //To get the OpenCL context to run on.
#include "ApexException.h"
#include "SimplePolygon.h" //The class we're implementing.

namespace apex {

area_t SimplePolygon::area_opencl(const Device<>& device) const {
	cl::Context& context = OpenCLContext::getInstance().contexts[device];
	cl::CommandQueue& queue = OpenCLContext::getInstance().queues[device];

	//Load the source code.
	cl::Program& program = OpenCLContext::getInstance().compile(device.cl_device,
		#include "Area.cl"
	);

	//We might need to make multiple passes if the device has a very limited amount of memory.
	DeviceStatistics statistics(&device);
	constexpr size_t vertex_size = sizeof(coord_t) * 2;
	const cl_ulong local_buffer_size = statistics.local_memory / vertex_size * vertex_size; //Make sure that the memory buffers hold an integer number of vertices.
	const cl_ulong global_buffer_size = statistics.global_memory / vertex_size * vertex_size;

	const size_t vertices_per_pass = global_buffer_size / vertex_size;
	area_t total_area = 0; //Result sum of all passes.
	for(size_t pivot_vertex = 0; pivot_vertex < size(); pivot_vertex += vertices_per_pass - 1) { //If the total data size is more than what fits in global memory, we'll have to make multiple passes.
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
		vertices_per_work_group = std::min(vertices_per_work_group, local_buffer_size / sizeof(area_t)); //We must also limit the work group size by the memory that the work groups can use locally.
		this_work_groups = (vertices_this_pass + vertices_per_work_group - 1) / vertices_per_work_group;
		//Round the global work size up to multiple of vertices_per_work_group. The kernel itself handles work items that need to idle.
		const size_t global_work_size = (vertices_this_pass + vertices_per_work_group - 1) / vertices_per_work_group * vertices_per_work_group;

		//Allocate global memory on the device for the input.
		const cl_ulong this_global_buffer_size = (vertices_this_pass + 1) * vertex_size;
		cl::Buffer input_points(context, CL_MEM_READ_ONLY, (global_work_size + 1) * vertex_size);
		queue.enqueueWriteBuffer(input_points, CL_TRUE, 0, this_global_buffer_size - vertex_size, &(*this)[pivot_vertex]); //Write the polyline and first pivot vertex.
		queue.enqueueWriteBuffer(input_points, CL_TRUE, this_global_buffer_size - vertex_size, vertex_size, &(*this)[pivot_vertex_after]); //Write the second pivot vertex.

		//Allocate an output buffer: One area_t for each work group as their output.
		cl_ulong this_output_buffer_size = this_work_groups * sizeof(area_t);
		cl::Buffer output_areas(context, CL_MEM_WRITE_ONLY, this_output_buffer_size);

		//Call the kernel to compute the area of this polygon and add it to total_area.
		cl::Kernel area_kernel(program, "area");
		area_kernel.setArg(0, input_points);
		area_kernel.setArg(1, vertices_this_pass);
		area_kernel.setArg(2, output_areas);
		area_kernel.setArg(3, cl::Local(vertices_per_work_group * sizeof(area_t)));
		queue.enqueueNDRangeKernel(area_kernel, cl::NullRange, cl::NDRange(global_work_size), cl::NDRange(vertices_per_work_group));
		cl_int result = queue.finish(); //Let the device do its thing!
		if(result != CL_SUCCESS) {
			throw ApexException("Error executing command queue for area computation.");
		}

		//Read the output data in.
		std::vector<area_t> areas;
		areas.resize(this_work_groups);
		queue.enqueueReadBuffer(output_areas, CL_TRUE, 0, this_output_buffer_size, &(areas[0]));
		queue.finish();
		for(const area_t area : areas) {
			total_area += area;
		}
	}

	return total_area >> 1;
}

area_t SimplePolygon::area_host() const {
	//Shoelace formula to compute the area.
	area_t area = 0;
	for(size_t vertex = 0, previous = size() - 1; vertex < size(); vertex++) {
		area += (area_t)(*this)[previous].x * (area_t)(*this)[vertex].y - (area_t)(*this)[previous].y * (area_t)(*this)[vertex].x;
		previous = vertex;
	}
	return area >> 1;
}

}