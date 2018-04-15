/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "OpenCLContext.h"
#include "OpenCLDevices.h" //To get the devices to compile for.
#include "ParallelogramException.h"
namespace parallelogram {

OpenCLContext::OpenCLContext() {
	context = cl::Context(OpenCLDevices::getInstance().getGPUs());
}

OpenCLContext& OpenCLContext::getInstance() {
	static OpenCLContext instance; //Constructs using the default constructor.
	return instance;
}

cl::Program& OpenCLContext::compile(std::string source) {
	if(programs.find(source) == programs.end()) {
		cl::Program::Sources sources;
		sources.push_back({source.c_str(), source.length()});
		programs[source] = cl::Program(context, sources);
		const std::vector<cl::Device>& devices = OpenCLDevices::getInstance().getGPUs();
		if(programs[source].build(devices) != CL_SUCCESS) {
			throw ParallelogramException((std::string("Compiling kernel failed: ") + programs[source].getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0])).c_str());
		}
	}
	return programs[source];
}

}