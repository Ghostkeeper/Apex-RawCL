/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef TESTPOLYGONAREA
#define TESTPOLYGONAREA

#include <gtest/gtest.h>
#include "Polygon.h"

namespace parallelogram {

class TestPolygonArea : public testing::Test {
protected:
	//A few polygons to test with.
	Polygon square_1000;

	virtual void SetUp() {
		square_1000.emplace_back(); //Contains one simple polygon.
		square_1000[0].emplace_back(0, 0);
		square_1000[0].emplace_back(0, 1000);
		square_1000[0].emplace_back(1000, 1000);
		square_1000[0].emplace_back(1000, 0);
	}
};

/*
 * Test for the area of an empty polygon being 0.
 */
TEST_F(TestPolygonArea, InitialAreaIsZero) {
	Polygon empty_polygon;
	EXPECT_EQ(0, empty_polygon.area());
}

/*
 * Test the area of a 1000 by 1000 square.
 */
TEST_F(TestPolygonArea, Square1000) {
	EXPECT_EQ(1000000, square_1000.area());
}

/*
 * Test the area of a 1000 by 1000 square that's completely in the negative
 * coordinate area.
 */
TEST_F(TestPolygonArea, Square1000NegativeCoordinates) {
	square_1000.translate(-1024, -1024);
	EXPECT_EQ(1000000, square_1000.area());
}

/*
 * Test the area of a 1000 by 1000 square that's in the negative area of the Y
 * dimension but the positive area of the X dimension.
 */
TEST_F(TestPolygonArea, Square1000NegativeY) {
	square_1000.translate(0, -1024);
	EXPECT_EQ(1000000, square_1000.area());
}

/*
 * Test the area of a 1000 by 1000 square that's partially in the positive area
 * and partially in the negative area.
 */
TEST_F(TestPolygonArea, Square1000AroundOrigin) {
	square_1000.translate(-512, -512);
	EXPECT_EQ(1000000, square_1000.area());
}

}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif //TESTPOLYGONAREA

