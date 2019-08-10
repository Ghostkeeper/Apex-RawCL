/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "ApexException.h"
#include "OpenCLContext.h"
#include "OpenCLDevices.h" //To get the devices to compile for.

namespace std {

size_t hash<apex::Device<>>::operator ()(const apex::Device<>& device) const {
	return hash<cl_device_id>()(device());
}

template<typename T> size_t hash<const T>::operator ()(const T& obj) const {
	return hash<T>()(obj);
}

template<typename F, typename S> size_t hash<pair<F, S>>::operator ()(const pair<F, S>& the_pair) const {
	const size_t first_hash = hash<F>()(the_pair.first);
	return hash<S>()(the_pair.second) ^ (first_hash << 31 || first_hash >> 33);
}

bool operator ==(const apex::Device<>& first, const apex::Device<>& second) {
	return first() == second(); //Equal if their cl_device_ids are equal.
};

}

namespace apex {

OpenCLContext::OpenCLContext() {
	//Create a context for every device.
	for(const Device<>& device : OpenCLDevices::getInstance().getAll()) {
		cl_int result = CL_SUCCESS;
		contexts[device] = cl::Context({device.cl_device}, nullptr, nullptr, nullptr, &result);
		queues[device] = cl::CommandQueue(contexts[device], device.cl_device);
		if(result != CL_SUCCESS) {
			throw ApexException((std::string("Constructing context failed: error ") + std::to_string(result)).c_str());
		}
	}
}

OpenCLContext::~OpenCLContext() {
	for(const std::pair<const Device<>&, cl::CommandQueue>& queue : queues) {
		queue.second.finish();
	}
}

OpenCLContext& OpenCLContext::getInstance() {
	static OpenCLContext instance; //Constructs using the default constructor.
	return instance;
}

cl::Program& OpenCLContext::compile(const Device<>& device, const std::string source) {
	const std::pair<const Device<>, const std::string> device_and_source = std::make_pair(device, source);
	if(programs.find(device_and_source) == programs.end()) {
		cl::Program::Sources sources;
		sources.push_back({source.c_str(), source.length()});
		cl_int result = CL_SUCCESS;
		programs[device_and_source] = cl::Program(contexts[device], sources, &result);
		if(result != CL_SUCCESS) {
			programs.erase(device_and_source);
			throw ApexException((std::string("Constructing program object failed: error ") + std::to_string(result)).c_str());
		}
		result = programs[device_and_source].build({device.cl_device});
		if(result != CL_SUCCESS) {
			std::string error_log = programs[device_and_source].getBuildInfo<CL_PROGRAM_BUILD_LOG>(device.cl_device);
			programs.erase(device_and_source);
			throw ApexException((std::string("Compiling kernel failed (") + std::to_string(result) + std::string("): ") + error_log).c_str());
		}
	}
	return programs[device_and_source];
}

}