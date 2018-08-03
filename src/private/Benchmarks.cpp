/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "BenchmarkData.h" //File that groups the actual benchmark data in compile time.
#include "Benchmarks.h"
#include "DeviceStatistics.h" //To predict the performance of devices based on their specs.
#include "OpenCLDevices.h" //Get the devices that we can choose to run algorithms on.
#include "ParallelogramException.h" //When something goes wrong.

namespace parallelogram {
namespace benchmarks {

std::pair<std::string, const cl::Device*> choose(const std::vector<std::string> options, const std::vector<size_t> problem_size) {
	if(options.empty()) {
		throw ParallelogramException("Strategy choice has no algorithms to choose from.");
	}
	const OpenCLDevices& device_manager = OpenCLDevices::getInstance();
	const std::vector<cl::Device>& all_devices = device_manager.getAll();
	std::vector<const cl::Device*> available_devices;
	for(std::vector<cl::Device>::const_iterator device = all_devices.begin(); device != all_devices.end(); device++) { //Convert to pointers and add the host as available device.
		available_devices.push_back(&*device);
	}
	available_devices.push_back(nullptr);

	//Try each option on each device. Predict how long it would take to execute.
	double best_time = std::numeric_limits<double>::infinity();
	std::string best_option = "";
	const cl::Device* best_device = nullptr;
	const BenchmarkData& data = BenchmarkData::getInstance();
	for(std::string option : options) {
		for(const cl::Device* device : available_devices) {
			DeviceStatistics statistics = device_manager.getStatistics(device);
			if(data.predictor.find(std::pair<std::string, std::string>(option, "constant")) == data.predictor.end()) {
				continue; //No data on this algorithm.
			}
			double prediction = data.predictor.find(std::pair<std::string, std::string>(option, "constant"))->second;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "compute_units"))->second * statistics.compute_units;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "compute_units^2"))->second * statistics.compute_units * statistics.compute_units;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "items_per_compute_unit"))->second * statistics.items_per_compute_unit;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "items_per_compute_unit^2"))->second * statistics.items_per_compute_unit * statistics.items_per_compute_unit;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "clock_frequency"))->second * statistics.clock_frequency;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "global_memory"))->second * statistics.global_memory;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "global_memory^2"))->second * statistics.global_memory * statistics.global_memory;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "local_memory"))->second * statistics.local_memory;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "local_memory^2"))->second * statistics.local_memory * statistics.local_memory;
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "size"))->second * problem_size[0]; //TODO: Allow for problem sizes of higher dimensions.
			prediction += data.predictor.find(std::pair<std::string, std::string>(option, "size^2"))->second * problem_size[0] * problem_size[0];
			if(prediction < best_time) {
				best_time = prediction;
				best_option = option;
				best_device = device;
			}
		}
	}

	return std::pair<std::string, const cl::Device*>(best_option, best_device);
}

}
}