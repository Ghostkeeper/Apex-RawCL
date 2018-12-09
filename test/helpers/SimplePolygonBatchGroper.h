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
template<typename Iterator>
class SimplePolygonBatchGroper {
public:
	/*
	 * The batch that is being tested right now.
	 */
	SimplePolygonBatch<Iterator>* tested_batch;

	void divide_edges(const DeviceStatistics& statistics, const cl_ulong global_overhead_per_polygon, std::vector<std::vector<size_t>>& start_positions, std::vector<size_t>& work_groups_per_pass) const {
		return tested_batch->divide_edges(statistics, global_overhead_per_polygon, start_positions, work_groups_per_pass);
	}
};

}

#endif //SIMPLEPOLYGONBATCHGROPER_H