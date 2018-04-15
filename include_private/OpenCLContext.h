/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef OPENCLCONTEXT_H
#define OPENCLCONTEXT_H

#include <unordered_map> //The cache of OpenCL programs.
#include "OpenCL.h" //To store the context and programs.

namespace parallelogram {

/*
 * This holds the OpenCL context so that we don't have to recreate it for every
 * function call.
 *
 * It also holds a cache for the compiled OpenCL programs to prevent having to
 * build them over and over again. All OpenCL kernels are compiled for every
 * available device.
 */
class OpenCLContext {
public:
	/*
	 * The OpenCL context that all kernels should be run in.
	 */
	cl::Context context;

	/*
	 * Statically gets the instance of this class.
	 *
	 * Since this class is a singleton, there can be only one instance of this
	 * class.
	 */
	static OpenCLContext& getInstance();

	/*
	 * Compiles the given source code and returns a program that can be executed
	 * on any available OpenCL device.
	 *
	 * If the given source code has been compiled before, a cached program will
	 * be returned. It only needs to compile once.
	 */
	cl::Program& compile(std::string source);

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
	std::unordered_map<std::string, cl::Program> programs;

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

