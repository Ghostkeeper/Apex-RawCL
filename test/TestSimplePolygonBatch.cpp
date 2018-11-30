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
TEST_F(TestSimplePolygonBatch, Unlimited) {
	SimplePolygonBatch<std::vector<SimplePolygon>::const_iterator> batch(ten_triangles.cbegin(), ten_triangles.cend());
	const_groper.tested_batch = &batch;

	//Construct a computer with enough memory for anything and unlimited work items per group.
	DeviceStatistics limits(nullptr);
	limits.global_memory = 9999999999999; //Shouldn't be a limit.
	limits.local_memory = 9999999999999;
	limits.items_per_compute_unit = 9999999999999;

	std::vector<std::vector<size_t>> start_positions;
	std::vector<size_t> work_groups_per_pass;

	const_groper.divide_edges(limits, 0, start_positions, work_groups_per_pass);
	ASSERT_EQ(1, start_positions.size()); //There should be only one work group.
	EXPECT_EQ(ten_triangles.size(), start_positions[0].size()); //And it should contain 10 start positions, one for every polygon.
	for(size_t i = 0; i < ten_triangles.size(); i++) {
		EXPECT_EQ(i * 4, start_positions[0][i]); //Each triangle stores 4 vertices (of which one pivot).
	}
}

TEST_F(TestSimplePolygonBatch, JustEnoughWorkItems) {
	SimplePolygonBatch<std::vector<SimplePolygon>::const_iterator> batch(ten_triangles.cbegin(), ten_triangles.cend());
	const_groper.tested_batch = &batch;

	//Construct a computer with enough memory for everything but just barely enough work items per group.
	DeviceStatistics limits(nullptr);
	limits.global_memory = 9999999999999; //Shouldn't be a limit.
	limits.local_memory = 9999999999999; //Shouldn't be a limit.
	limits.items_per_compute_unit = 30; //Just enough to process all 30 vertices at once.

	std::vector<std::vector<size_t>> start_positions;
	std::vector<size_t> work_groups_per_pass;

	const_groper.divide_edges(limits, 0, start_positions, work_groups_per_pass);
	ASSERT_EQ(1, start_positions.size()); //There should still be only one work group.
	EXPECT_EQ(ten_triangles.size(), start_positions[0].size()); //And it should contain all 10 start positions.
	for(size_t i = 0; i < ten_triangles.size(); i++) {
		EXPECT_EQ(i * 4, start_positions[0][i]); //Each triangle stores 4 vertices (of which one pivot).
	}
}

/*
 * Starts running the tests.
 *
 * This calls upon GoogleTest to start testing.
 * \param argc The number of arguments to read.
 * \param argv The arguments provided to this application via the command.
 * \return ``0`` if all tests ran successfully, or something else if any test
 * failed.
 */
int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

}

#endif //TESTSIMPLEPOLYGONBATCH