/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGONBATCHGROPER_H
#define SIMPLEPOLYGONBATCHGROPER_H

#include "SimplePolygonBatch.h" //The class we're exposing the private members of.

namespace apex {

/*
 * Class that allows access to private members of ``SimplePolygonBatch`` in
 * order to test them.
 *
 * This breaks the secrecy of ``SimplePolygonBatch`` for the purpose of testing.
 * \tparam Iterator The type of iterators to use in the batch.
 */
template<typename Iterator,
	typename Device = cl::Device> //Templated cl::Device to allow mocking it in tests.
class SimplePolygonBatchGroper {
public:
	/*
	 * The batch that is being tested right now.
	 */
	SimplePolygonBatch<Iterator, Device>* tested_batch;

	size_t count() const {
		return tested_batch->count;
	}

	bool ensure_fit(cl_ulong maximum_memory) {
		return tested_batch->ensure_fit(maximum_memory);
	}

	template<typename OpenCLContext = OpenCLContext,
		typename Buffer = cl::Buffer,
		typename Context = cl::Context,
		typename CommandQueue = cl::CommandQueue>
	bool load(const Device& device, const cl_ulong overhead) {
		return tested_batch->template load<OpenCLContext, Buffer, Context, CommandQueue>(device, overhead);
	}

	std::unordered_map<const Device*, cl_ulong>& loaded_in_memory() const {
		return tested_batch->loaded_in_memory;
	}

	std::vector<SimplePolygonBatch<Iterator, Device>> subbatches() const {
		return tested_batch->subbatches;
	}

	size_t total_vertices() const {
		return tested_batch->total_vertices;
	}
};

}

#endif //SIMPLEPOLYGONBATCHGROPER_H