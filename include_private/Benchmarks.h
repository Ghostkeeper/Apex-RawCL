/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef APEX_BENCHMARKS_H
#define APEX_BENCHMARKS_H

#include "OpenCL.h" //For cl::Device.

namespace apex {
namespace benchmarks { //Contain all benchmark data and the ``choose`` function in a sub-namespace.

/*
 * Uses available benchmark data to choose between implementations of an
 * algorithm.
 *
 * The intent of this class is to choose between different implementations of an
 * algorithm based on the input size. The benchmarks know nothing about the
 * actual content of the implementations though, so it could just as well be
 * used to choose between different types of algorithms for their time
 * complexity.
 *
 * Each benchmark contains a unique identifier which is a string denoting the
 * name of the function (by convention). It also contains the time that it took
 * to execute. To make an educated guess at which function should be used, it
 * also contains metadata information about the most salient device that the
 * algorithm depends on, as well as the size of the input (in any number of
 * dimensions). Few functions will depend solely on one device. All
 * OpenCL-accelerated functions will for instance depend both on the selected
 * OpenCL device as well as on the host that calls upon OpenCL. However in such
 * a case the OpenCL device is probably the most salient for the final execution
 * time, so this must then be logged with the benchmark.
 *
 * This benchmarks class, when called upon, can then choose between the
 * available functions based on the available devices in the current environment
 * and the size of the input that must currently be processed.
 *
 * A precise match will rarely be found among the benchmarks. First, the choice
 * algorithm will choose which benchmarks to use based on the available devices
 * in the current machines, choosing from among the devices that benchmarks are
 * available for the devices that most closely match the specifications. Then
 * the choice of which device to use will be made depending on the input size.
 *
 * A special optional device is the host, which may vary greatly in available
 * performance. The host is intended to be used in case there is no OpenCL
 * support available in the current environment, so it is advisable to provide
 * at least one function as a fall-back that doesn't call OpenCL and marks the
 * most salient device type as "host". In the benchmarks, the host device has no
 * performance data since OpenCL cannot provide that to us and it will vary too
 * much to be useful anyway.
 *
 * The result of the choice is a tuple containing the chosen function's unique
 * identifier, and the chosen most salient OpenCL device it needs to run on.
 * \param options The names of the algorithms that we can choose from.
 * \param problem_size A multi-dimensional problem size. Some problems may just
 * have a fixed size (for example the size of the polygon that the algorithm is
 * applied to), but some may have multiple sizes that the algorithm depends on,
 * such as when multiple polygons are involved. The number of dimensions in the
 * problem size must be the same for all algorithms to choose among, and must be
 * the same for those algorithms every time. If an algorithm accepts an
 * arbitrary number of polygons, for instance, the problem size may not be the
 * individual vertex counts of each of the polygons, but must be either the
 * total count of vertices in all of them or the number of polygons.
 * \return A pair consisting of:
 * * The name of the most efficient algorithm to run in this case.
 * * The device to run this algorithm on, or nullptr if it should be the host.
 */
std::pair<std::string, const cl::Device*> choose(const std::vector<std::string> options, const std::vector<size_t> problem_size);

}
}

#endif //APEX_BENCHMARKS_H

