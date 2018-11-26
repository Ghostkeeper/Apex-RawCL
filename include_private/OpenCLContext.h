/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef OPENCLCONTEXT_H
#define OPENCLCONTEXT_H

#include <unordered_map> //The caches of OpenCL programs and contexts.
#include "OpenCL.h" //To store the context and programs.

namespace std {

/*
 * Hashes cl::Device instances.
 *
 * Devices are hashed by their cl_device_id, which should be unique per device.
 */
template<> struct hash<cl::Device> {
	size_t operator ()(const cl::Device& device) const;
};

/*
 * Generic template for hashing const objects.
 *
 * This refers to its non-const implementation. Hashing a const object is
 * exactly the same as hashing a non-const object.
 */
template<typename T> struct hash<const T> {
	size_t operator ()(const T& obj) const;
};

/*
 * Hashes any arbitrary std::pair.
 *
 * This refers through to the hash of both elements and combines them with some
 * bitwise operators.
 */
template<typename F, typename S> struct hash<pair<F, S>> {
	size_t operator ()(const pair<F, S>& the_pair) const;
};

/*
 * Compares two devices for equality.
 *
 * Devices are compared by their cl_device_id, which should be unique per
 * device.
 */
bool operator ==(const cl::Device& first, const cl::Device& second);

}

namespace apex {

/*
 * This holds the OpenCL contexts and queues so that we don't have to recreate
 * them for every function call.
 *
 * One context is created for every available device.
 *
 * It also holds a cache for the compiled OpenCL programs to prevent having to
 * build them over and over again. All OpenCL kernels are compiled for every
 * available device.
 */
class OpenCLContext {
public:
	/*
	 * For each OpenCL device its context, where all kernels should be run.
	 */
	std::unordered_map<cl::Device, cl::Context> contexts;

	/*
	 * For each OpenCL device its command queue.
	 */
	std::unordered_map<cl::Device, cl::CommandQueue> queues;

	/*
	 * Statically gets the instance of this class.
	 *
	 * Since this class is a singleton, there can be only one instance of this
	 * class.
	 */
	static OpenCLContext& getInstance();

	/*
	 * Compiles the given source code and returns a program that can be executed
	 * on the specified device.
	 *
	 * If the given source code has been compiled before, a cached program will
	 * be returned. It only needs to compile once.
	 * \param device The OpenCL device to compile the source code for.
	 */
	cl::Program& compile(const cl::Device& device, const std::string source);

	/*
	 * Since this is a singleton, the copy constructor should not be
	 * implemented.
	 */
	OpenCLContext(const OpenCLContext& original) = delete;

	/*
	 * Since this is a singleton, the copy assignment operator should not be
	 * implemented.
	 */
	void operator =(const OpenCLContext& original) = delete;
protected:
	/*
	 * The cache storing programs after compiling them from source code.
	 */
	std::unordered_map<std::pair<const cl::Device, const std::string>, cl::Program> programs;

	/*
	 * Creates a new instance of the OpenCL program cache.
	 *
	 * This constructor also creates the OpenCL context and puts all available
	 * devices in the context.
	 */
	OpenCLContext();
};

}

#endif //OPENCLKERNELS_H