/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "OpenCLDevices.h" //To choose the preferred device.
#include "ParallelogramException.h"
#include "SimplePolygon.h"

namespace parallelogram {

SimplePolygon::SimplePolygon() {
	//Only construct the vector of vertices.
}

area_t SimplePolygon::area() const {
	//TODO: Use benchmarks to choose between implementations.
	if(size() >= 60000) { //For now, use a measured threshold from a single benchmark. About here the GPU starts to become faster.
		OpenCLDevices& devices = OpenCLDevices::getInstance();
		cl::Device device;
		if(!devices.getGPUs().empty()) {
			device = devices.getGPUs()[0];
		} else if(!devices.getCPUs().empty()) {
			device = devices.getCPUs()[0];
		} else {
			throw ParallelogramException("No supported OpenCL devices!");
		}
		return area_opencl(device);
	} else {
		return area_host();
	}
}

void SimplePolygon::translate(const coord_t x, const coord_t y) {
	for(size_t vertex = 0; vertex < size(); vertex++) {
		at(vertex) += Point2(x, y);
	}
}

}