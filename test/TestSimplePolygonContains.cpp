/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef TESTSIMPLEPOLYGONCONTAINS
#define TESTSIMPLEPOLYGONCONTAINS

#include <gtest/gtest.h>
#include "SimplePolygon.h" //To construct simple polygons to test on.

namespace parallelogram {

/*
 * Some fixtures of polygons to test with.
 */
class TestSimplePolygonContains : public testing::Test {
protected:
	/*
	 * A square of 1000 by 1000 units, with one corner at 0,0.
	 */
	SimplePolygon square_1000;

	/*
	 * A rotated square with diagonals of length 1000.
	 *
	 * It is centred around 0,0 and rotated 45 degrees.
	 */
	SimplePolygon diamond_1000;

	/*
	 * Prepares for running a test.
	 *
	 * Before every test, a new instance of this class is created and this test
	 * is ran.
	 */
	virtual void SetUp() {
		square_1000.emplace_back(0, 0);
		square_1000.emplace_back(1000, 0);
		square_1000.emplace_back(1000, 1000);
		square_1000.emplace_back(0, 1000);

		diamond_1000.emplace_back(-500, 0);
		diamond_1000.emplace_back(0, -500);
		diamond_1000.emplace_back(500, 0);
		diamond_1000.emplace_back(0, 500);
	}
};

/*
 * Test whether a point is inside a square.
 */
TEST_F(TestSimplePolygonContains, InsideSquare) {
	EXPECT_TRUE(square_1000.contains(Point2(500, 500)));
}

/*
 * Test whether a point is outside a square.
 */
TEST_F(TestSimplePolygonContains, OutsideSquare) {
	EXPECT_FALSE(square_1000.contains(Point2(-500, 500)));
}

/*
 * Test whether a point is inside a diamond.
 *
 * This tests for the case where the polygon has diagonal edges.
 */
TEST_F(TestSimplePolygonContains, InsideDiamondNonCenter) {
	EXPECT_TRUE(diamond_1000.contains(Point2(50, 50)));
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

#endif //TESTSIMPLEPOLYGONCONTAINS