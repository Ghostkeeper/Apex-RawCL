/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef TESTSIMPLEPOLYGONBATCH
#define TESTSIMPLEPOLYGONBATCH

#include <gtest/gtest.h>
#include "SimplePolygon.h" //Example polygons.
#include "SimplePolygonBatch.h"
#include "helpers/SimplePolygonBatchGroper.h" //The class under test.

namespace apex {

/*
 * Some fixtures for the TestSimplePolygonBatch tests.
 */
class TestSimplePolygonBatch : public testing::Test {
protected:
	/*
	 * A vector containing ten triangles.
	 */
	std::vector<SimplePolygon> ten_triangles;

	/*
	 * Provides access to ``SimplePolygonBatch``'s private members in order to
	 * test them.
	 */
	SimplePolygonBatchGroper<std::vector<SimplePolygon>::const_iterator> const_groper;

	virtual void SetUp() {
		SimplePolygon triangle; //Triangle with area of 100.
		triangle.emplace_back(0, 0);
		triangle.emplace_back(20, 0);
		triangle.emplace_back(10, 20);
		for(size_t i = 0; i < 10; i++) {
			ten_triangles.push_back(triangle);
		}
	}
};

/*
 * Test the case where all data fits in a single work group.
 *
 * This is the simplest case.
 */
TEST_F(TestSimplePolygonBatch, LimitedByWorkGroupSize) {
	SimplePolygonBatch<std::vector<SimplePolygon>::const_iterator> batch(ten_triangles.cbegin(), ten_triangles.cend());
	const_groper.tested_batch = &batch;

	//Construct a computer with enough memory for anything, and just enough workers in a group so that everything goes into one group.
	DeviceStatistics limits(nullptr);
	limits.global_memory = 9999999999999; //Shouldn't be a limit.
	limits.local_memory = 9999999999999; //Shouldn't be a limit.
	limits.items_per_compute_unit = 9999999999999; //Not a limit yet.

	std::vector<std::vector<size_t>> start_positions;
	std::vector<size_t> work_groups_per_pass;

	const_groper.divide_edges(limits, 0, start_positions, work_groups_per_pass);
	ASSERT_EQ(1, start_positions.size()); //There should be only one work group.
	EXPECT_EQ(ten_triangles.size(), start_positions[0].size()); //And it should contain 10 start positions, one for every polygon.
	for(size_t i = 0; i < ten_triangles.size(); i++) {
		EXPECT_EQ(i * 4, start_positions[0][i]); //Each triangle stores 4 vertices (of which one pivot).
	}

	limits.items_per_compute_unit = 30; //Still not a limit yet.
	const_groper.divide_edges(limits, 0, start_positions, work_groups_per_pass);
	ASSERT_EQ(1, start_positions.size()); //There should still be only one work group.
	EXPECT_EQ(ten_triangles.size(), start_positions[0].size());

	limits.items_per_compute_unit = 29; //Now we can't fit everything in one work group. Need a second one!
	const_groper.divide_edges(limits, 0, start_positions, work_groups_per_pass);
	ASSERT_EQ(2, start_positions.size()); //Now we need 2 work groups.
	EXPECT_EQ(10, start_positions[0].size()); //All 10 triangles will still start in the first work group.
	EXPECT_EQ(0, start_positions[1].size()); //None of them start in the second group, but it is there.
}

}

#endif //TESTSIMPLEPOLYGONBATCH