/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <algorithm> //For std::min.
#ifdef __APPLE__
	#include "OpenCL/opencl.h"
#else
	#include "CL/cl.h"
#endif

#include "OpenCLDevices.h" //To get the OpenCL devices we can run on.
#include "ParallelogramException.h"
#include "SimplePolygon.h" //We're implementing functions from this header.

namespace parallelogram {

coord_t SimplePolygon::area_gpu() const {
	//TODO: We want to get the preferred device from the benchmarks::choose function.
	OpenCLDevices& devices = OpenCLDevices::getInstance();
	cl::Device device;
	if(!devices.getGPUs().empty()) {
		device = devices.getGPUs()[0];
	} else if(!devices.getCPUs().empty()) {
		device = devices.getCPUs()[0];
	} else {
		throw ParallelogramException("No supported OpenCL devices!");
	}

	cl::Context context({device});
	cl::CommandQueue queue(context, device);

	//Load the source code.
	cl::Program::Sources kernel_sources;
	const std::string kernel_source = R"kernel(
//TODO.
)kernel";
	kernel_sources.push_back({kernel_source.c_str(), kernel_source.length()});
	cl::Program program(context, kernel_sources);
	if(program.build({device}) != CL_SUCCESS) {
		throw ParallelogramException("Compiling kernel for Polygon::area failed.");
	}

	//We might need to make multiple passes if the device has a very limited amount of memory.
	cl_ulong constant_buffer_size;
	if(device.getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &constant_buffer_size) != CL_SUCCESS) {
		constant_buffer_size = 64 << 10; //OpenCL standard says that the minimum is 64kB.
	}
	cl_ulong local_buffer_size;
	if(device.getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &local_buffer_size) != CL_SUCCESS) {
		local_buffer_size = 32 << 10; //OpenCL standard says that the minimum is 32kB.
	}
	cl_uint compute_units;
	if(device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &compute_units) != CL_SUCCESS) {
		compute_units = 1; //OpenCL standard says that there must be 1 compute unit.
	}
	constant_buffer_size = std::min(constant_buffer_size, compute_units * local_buffer_size * 2); //If the sum of the local buffers isn't large enough to hold the intermediary values, make more passes.

	const size_t vertices_per_pass = constant_buffer_size / (sizeof(coord_t) * 2);
	coord_t total_area = 0; //Result sum of all passes.
	for(size_t pivot_vertex = 0; pivot_vertex < size(); pivot_vertex += vertices_per_pass - 2) { //If the total data size is more than what fits in constant memory, we'll have to make multiple passes.
		/* Each pass will compute the area of a part of the polygon.
		 * We'll take a part of the perimeter and close that polyline off with
		 * the initial vertex to create a polygon and compute that area. We save
		 * the beginning and ending vertex of each such polygon to compute the
		 * area that we missed in the middle of these pieces of polygon (the
		 * pivot vertices). */

		size_t pivot_vertex_after = pivot_vertex + vertices_per_pass - 2; //-1 because we need to store the pivot_vertex twice, and -1 because the pivot vertex of the next pass is the last vertex of this pass.
		size_t vertices_this_pass = vertices_per_pass;
		if(pivot_vertex_after >= size()) {
			pivot_vertex_after = 0;
			vertices_this_pass = size() - pivot_vertex + 1;
		}

		//Allocate constant memory on the device for the input.
		cl_ulong this_constant_buffer_size = vertices_this_pass * sizeof(coord_t) * 2;
		cl::Buffer input_points(context, CL_MEM_READ_ONLY, this_constant_buffer_size);
		queue.enqueueWriteBuffer(input_points, CL_TRUE, 0, this_constant_buffer_size - sizeof(coord_t) * 4, &(*this)[pivot_vertex]); //Write the polyline and first pivot vertex.
		queue.enqueueWriteBuffer(input_points, CL_TRUE, this_constant_buffer_size - sizeof(coord_t) * 4, sizeof(coord_t) * 2, &(*this)[pivot_vertex_after]); //Write the second pivot vertex.
		queue.enqueueWriteBuffer(input_points, CL_TRUE, this_constant_buffer_size - sizeof(coord_t) * 2, sizeof(coord_t) * 2, &(*this)[pivot_vertex]); //Write the first pivot vertex again to close the polygon.

		//TODO: Call the kernel to compute the area of this polygon and add it to total_area.

		total_area += (*this)[pivot_vertex].x * (*this)[pivot_vertex_after].y - (*this)[pivot_vertex].y * (*this)[pivot_vertex_after].x;
	}

	return total_area;
}

coord_t SimplePolygon::area_host() const {
	//Apothem method to compute the area.
	coord_t area = 0;
	size_t previous = size() - 1;
	for(size_t vertex = 0, previous = size() - 1; vertex < size(); vertex++) {
		area += (*this)[previous].x * (*this)[vertex].y - (*this)[previous].y * (*this)[vertex].x;
		previous = vertex;
	}
	return area >> 1;
}

}