/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef BENCHMARKER_H
#define BENCHMARKER_H

#include <string> //To return the device identifiers.
#include <vector> //To store multiple benchmarks.
#include "SimplePolygonBenchmark.h" //The benchmark runners.

namespace cl {

class Device; //Forward declaration of device to associate benchmarker with.

}

namespace apex {
namespace benchmarks {

/*
 * Performs benchmarks to determine the relative performance of various
 * algorithms on the current hardware.
 *
 * These benchmarks will then assist in making a better choice of strategy when
 * the algorithms are executed.
 *
 * The CPP file for this class also contains an entry point in order to perform
 * the benchmarks separately.
 */
class Benchmarker {
public:
	/*
	 * Each of these benchmarks will be run on all devices.
	 *
	 * We initialize these once and reuse them for every device.
	 */
	static const std::vector<SimplePolygonBenchmark> device_benchmarks;

	/*
	 * Each of these benchmarks will be run on the host.
	 */
	static const std::vector<SimplePolygonBenchmark> host_benchmarks;

	/*
	 * The device that this benchmarker is measuring.
	 *
	 * If measuring the host, this should be ``nullptr``.
	 */
	const cl::Device* device;

	/*
	 * Creates a new benchmarker for benchmarking the performance of a specific
	 * device.
	 * \param device The device to benchmark. If benchmarking the host, use
	 * ``nullptr``.
	 */
	Benchmarker(const cl::Device* device);

	/*
	 * Prints the statistics of the device that can be detected via OpenGL.
	 *
	 * This will output computational statistics such as clock speed and number
	 * of cores. This is then used to approximate the performance for devices
	 * that are unknown but have similar statistics.
	 */
	void device_statistics() const;

	/*
	 * Runs all benchmarks for the current device.
	 */
	void run() const;
};

}
}

#endif //BENCHMARKER_H