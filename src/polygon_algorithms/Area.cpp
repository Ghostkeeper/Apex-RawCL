/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include <algorithm> //For std::min.
#ifdef __APPLE__
	#include "OpenCL/opencl.hpp"
#else
	#include "CL/cl.hpp"
#endif

#include "OpenCLDevices.h" //To get the OpenCL devices we can run on.
#include "ParallelogramException.h"
#include "SimplePolygon.h" //We're implementing functions from this header.

namespace parallelogram {

area_t SimplePolygon::area_gpu() const {
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
	const std::string kernel_source =
	#include "Area.cl"
	;
	kernel_sources.push_back({kernel_source.c_str(), kernel_source.length()});
	cl::Program program(context, kernel_sources);
	if(program.build({device}) != CL_SUCCESS) {
		throw ParallelogramException((std::string("Compiling kernel for Polygon::area failed: ") + program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)).c_str());
	}

	//We might need to make multiple passes if the device has a very limited amount of memory.
	constexpr size_t vertex_size = sizeof(coord_t) * 2;
	cl_uint compute_units;
	if(device.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &compute_units) != CL_SUCCESS) {
		compute_units = 1; //OpenCL standard says that there must be 1 compute unit.
	}
	cl_ulong local_buffer_size;
	if(device.getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &local_buffer_size) != CL_SUCCESS) {
		local_buffer_size = 32 << 10; //OpenCL standard says that the minimum is 32kB.
	}
	local_buffer_size = local_buffer_size / vertex_size * vertex_size;
	cl_ulong constant_buffer_size;
	if(device.getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &constant_buffer_size) != CL_SUCCESS) {
		constant_buffer_size = 64 << 10; //OpenCL standard says that the minimum is 64kB.
	}
	constant_buffer_size = constant_buffer_size / vertex_size * vertex_size; //Make sure that the constant buffer holds an integer number of vertices.
	constant_buffer_size = std::min(constant_buffer_size, compute_units * local_buffer_size * 2); //If the sum of the local buffers isn't large enough to hold the intermediary values, make more passes.

	const size_t vertices_per_pass = constant_buffer_size / vertex_size;
	area_t total_area = 0; //Result sum of all passes.
	for(size_t pivot_vertex = 0; pivot_vertex < size(); pivot_vertex += vertices_per_pass - 1) { //If the total data size is more than what fits in constant memory, we'll have to make multiple passes.
		//Each item works on a line segment, which requires two vertices.
		//So we must leave space for 1 extra vertex in memory.
		size_t pivot_vertex_after = pivot_vertex + vertices_per_pass - 1; //-1 because the pivot vertex of the next pass is the last vertex of this pass.
		size_t vertices_this_pass = vertices_per_pass;
		if(pivot_vertex_after >= size()) {
			pivot_vertex_after = 0;
			vertices_this_pass = size() - pivot_vertex;
		}

		//Allocate constant memory on the device for the input.
		cl_ulong this_constant_buffer_size = (vertices_this_pass + 1) * vertex_size;
		cl::Buffer input_points(context, CL_MEM_READ_ONLY, this_constant_buffer_size);
		queue.enqueueWriteBuffer(input_points, CL_TRUE, 0, this_constant_buffer_size - vertex_size, &(*this)[pivot_vertex]); //Write the polyline and first pivot vertex.
		queue.enqueueWriteBuffer(input_points, CL_TRUE, this_constant_buffer_size - vertex_size, vertex_size, &(*this)[pivot_vertex_after]); //Write the second pivot vertex.

		//Dividing the work over as many work groups as possible.
		const size_t this_compute_units = std::min(static_cast<size_t>(compute_units), vertices_this_pass);
		const size_t vertices_per_compute_unit = vertices_this_pass / this_compute_units;

		//Allocate an output buffer: One area_t for each work group as their output.
		cl_ulong this_output_buffer_size = this_compute_units * sizeof(area_t);
		cl::Buffer output_areas(context, CL_MEM_WRITE_ONLY, this_output_buffer_size);

		//Call the kernel to compute the area of this polygon and add it to total_area.
		cl::make_kernel<cl::Buffer, cl::Buffer, cl::LocalSpaceArg> area_kernel(program, "area");
		cl::EnqueueArgs enqueue_arguments(queue, cl::NullRange, cl::NDRange(vertices_this_pass), cl::NDRange(vertices_per_compute_unit));
		cl::Event compute_result = area_kernel(enqueue_arguments, input_points, output_areas, cl::Local(vertices_per_compute_unit * vertex_size));
		cl_int result = compute_result.wait();
		if(result != CL_SUCCESS) { //Let the device do its thing!
			throw ParallelogramException("Error executing command queue for area computation.");
		}

		//Read the output data in.
		std::vector<area_t> areas;
		areas.resize(this_compute_units);
		queue.enqueueReadBuffer(output_areas, CL_TRUE, 0, this_output_buffer_size, &(areas[0]));
		queue.finish();
		for(const area_t area : areas) {
			total_area += area;
		}
	}

	return total_area >> 1;
}

area_t SimplePolygon::area_host() const {
	//Apothem method to compute the area.
	area_t area = 0;
	for(size_t vertex = 0, previous = size() - 1; vertex < size(); vertex++) {
		area += (area_t)(*this)[previous].x * (area_t)(*this)[vertex].y - (area_t)(*this)[previous].y * (area_t)(*this)[vertex].x;
		previous = vertex;
	}
	return area >> 1;
}

}