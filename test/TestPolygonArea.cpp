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

//Test for the area of an empty polygon being 0.
TEST(TestPolygonArea, InitialAreaIsZero) {
	Polygon empty_polygon;
	EXPECT_EQ(0, empty_polygon.area());
}

//Test the area of a 1000 by 1000 square.
TEST(TestPolygonArea, Square1000) {
	Polygon square_1000;
	square_1000.emplace_back(); //Contains one simple polygon.
	square_1000[0].emplace_back(0, 0);
	square_1000[0].emplace_back(0, 1000);
	square_1000[0].emplace_back(1000, 1000);
	square_1000[0].emplace_back(1000, 0);

	EXPECT_EQ(1000000, square_1000.area());
}

//Test the area of a 1000 by 1000 square that's completely in the negative coordinate area.
TEST(TestPolygonArea, Square1000NegativeCoordinates) {
	Polygon square_1000;
	square_1000.emplace_back(); //Contains one simple polygon.
	square_1000[0].emplace_back(-1024, -1024);
	square_1000[0].emplace_back(-1024, -24);
	square_1000[0].emplace_back(-24, -24);
	square_1000[0].emplace_back(-24, -1024);

	EXPECT_EQ(1000000, square_1000.area());
}

}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif //TESTPOLYGONAREA

