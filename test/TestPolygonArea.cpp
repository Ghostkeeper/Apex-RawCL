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
		square_1000[0].emplace_back(1000, 0);
		square_1000[0].emplace_back(1000, 1000);
		square_1000[0].emplace_back(0, 1000);
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
	EXPECT_EQ(1000 * 1000, square_1000.area());
}

/*
 * Test the area of a 1000 by 1000 square that's completely in the negative
 * coordinate area.
 */
TEST_F(TestPolygonArea, Square1000NegativeCoordinates) {
	square_1000.translate(-1024, -1024);
	EXPECT_EQ(1000 * 1000, square_1000.area());
}

/*
 * Test the area of a 1000 by 1000 square that's in the negative area of the Y
 * dimension but the positive area of the X dimension.
 */
TEST_F(TestPolygonArea, Square1000NegativeY) {
	square_1000.translate(0, -1024);
	EXPECT_EQ(1000 * 1000, square_1000.area());
}

/*
 * Test the area of a 1000 by 1000 square that's partially in the positive area
 * and partially in the negative area.
 */
TEST_F(TestPolygonArea, Square1000AroundOrigin) {
	square_1000.translate(-512, -512);
	EXPECT_EQ(1000 * 1000, square_1000.area());
}

/*
 * Test calculating the area of a triangle with a 1000-unit wide base.
 */
TEST_F(TestPolygonArea, Triangle1000) {
	Polygon triangle_1000;
	triangle_1000.emplace_back(); //Contains one simple polygon.
	triangle_1000[0].emplace_back(24, 24);
	triangle_1000[0].emplace_back(1024, 24);
	triangle_1000[0].emplace_back(524, 1024);

	EXPECT_EQ(1000 * 1000 / 2, triangle_1000.area());
}

/*
 * Test computing the area of a concave polygon.
 */
TEST_F(TestPolygonArea, Concave) {
	//This concave shape is a triangle with base length 100 and height 100, with a triangle subtracted from it at the base.
	//The subtracted triangle has base length 100 and height 50.
	//The area of this concave shape is then (100 * 100) / 2 - (100 * 50) / 2.
	Polygon concave;
	concave.emplace_back(); //Contains one simple polygon.
	concave[0].emplace_back(10, 10);
	concave[0].emplace_back(60, 60);
	concave[0].emplace_back(110, 10);
	concave[0].emplace_back(60, 110);

	EXPECT_EQ(100 * 100 / 2 - 100 * 50 / 2, concave.area());
}

/*
 * Test the area of a negative square, where the vertices are going the other
 * way around.
 *
 * The area should then be negative.
 */
TEST_F(TestPolygonArea, NegativeSquare) {
	Polygon negative_square_1000;
	negative_square_1000.emplace_back();
	negative_square_1000[0].emplace_back(0, 0);
	negative_square_1000[0].emplace_back(0, 1000);
	negative_square_1000[0].emplace_back(1000, 1000);
	negative_square_1000[0].emplace_back(1000, 0);

	EXPECT_EQ(-square_1000.area(), negative_square_1000.area());
}

/*
 * Tests the area of a complex polygon: A square with a hole in the middle.
 */
TEST_F(TestPolygonArea, SquareWithHole) {
	Polygon square_with_hole; //100 by 100 square with a 20 by 20 hole in the middle.
	square_with_hole.emplace_back();
	square_with_hole[0].emplace_back(0, 0);
	square_with_hole[0].emplace_back(100, 0);
	square_with_hole[0].emplace_back(100, 100);
	square_with_hole[0].emplace_back(0, 100);
	square_with_hole.emplace_back(); //A 20x20 hole in the negative.
	square_with_hole[1].emplace_back(30, 30);
	square_with_hole[1].emplace_back(30, 50);
	square_with_hole[1].emplace_back(50, 50);
	square_with_hole[1].emplace_back(50, 30);

	EXPECT_EQ(100 * 100 - 20 * 20, square_with_hole.area());
}

}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif //TESTPOLYGONAREA

