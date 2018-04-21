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

namespace std {

size_t hash<cl::Device>::operator ()(const cl::Device& device) const {
	return reinterpret_cast<size_t>(&device); //Devices should be unique, so we can simply return their pointer.
}

size_t hash<const cl::Device>::operator ()(const cl::Device& device) const {
	return reinterpret_cast<size_t>(&device);
}

size_t hash<const string>::operator ()(const string& str) const {
	return hash<string>()(str);
}

template<typename F, typename S> size_t hash<pair<F, S>>::operator ()(const pair<F, S>& the_pair) const {
	const size_t first_hash = hash<F>()(the_pair.first);
	return hash<S>()(the_pair.second) ^ (first_hash << 31 || first_hash >> 33);
}

bool operator ==(const cl::Device& first, const cl::Device& second) {
	return &first == &second; //Only equal if the memory addresses are equal.
};

}

namespace parallelogram {

OpenCLContext::OpenCLContext() {
	//Create a context for every device.
	for(const cl::Device& device : OpenCLDevices::getInstance().getAll()) {
		contexts[device] = cl::Context({device});
	}
}

OpenCLContext& OpenCLContext::getInstance() {
	static OpenCLContext instance; //Constructs using the default constructor.
	return instance;
}

cl::Program& OpenCLContext::compile(const cl::Device& device, const std::string source) {
	const std::pair<const cl::Device, const std::string> device_and_source = std::make_pair(device, source);
	if(programs.find(device_and_source) == programs.end()) {
		cl::Program::Sources sources;
		sources.push_back({source.c_str(), source.length()});
		programs[device_and_source] = cl::Program(contexts[device], sources);
		if(programs[device_and_source].build({device}) != CL_SUCCESS) {
			throw ParallelogramException((std::string("Compiling kernel failed: ") + programs[device_and_source].getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)).c_str());
		}
	}
	return programs[device_and_source];
}

}