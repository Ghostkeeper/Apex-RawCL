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
#include <cmath> //To construct a star.
#include "SimplePolygon.h" //To construct simple polygons to test on.

#define TAU 6.28318530717959

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
	 * This shape is basically a square with one edge flipped so that it becomes
	 * the shape of an hourglass.
	 *
	 * The top half of the hourglass has negative area. The bottom half is
	 * positive. The self-intersection happens at coordinate 500,500.
	 */
	SimplePolygon hourglass;

	/*
	 * A star with five points.
	 *
	 * This shape has self-intersections and winds multiple times around the
	 * centre. It behaves differently with the even-odd fill rule vs. the
	 * nonzero fill rule.
	 *
	 * The star is centred around 0,0 and has a radius of 500.
	 */
	SimplePolygon five_pointed_star;

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

		hourglass.emplace_back(0, 0);
		hourglass.emplace_back(1000, 0);
		hourglass.emplace_back(0, 1000);
		hourglass.emplace_back(1000, 1000);

		five_pointed_star.emplace_back(0, 500);
		five_pointed_star.emplace_back(-std::sin(TAU / 5 * 2) * 500, std::cos(TAU / 5 * 2) * 500);
		five_pointed_star.emplace_back(-std::sin(TAU / 5 * 4) * 500, std::cos(TAU / 5 * 4) * 500);
		five_pointed_star.emplace_back(-std::sin(TAU / 5 * 1) * 500, std::cos(TAU / 5 * 1) * 500);
		five_pointed_star.emplace_back(-std::sin(TAU / 5 * 3) * 500, std::cos(TAU / 5 * 3) * 500);
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
TEST_F(TestSimplePolygonContains, InsideDiamondNonCentre) {
	EXPECT_TRUE(diamond_1000.contains(Point2(50, 50)));
}

/*
 * Test whether a point is inside a diamond when it's exactly in the centre.
 *
 * This is an edge case because a ray cast exactly sideways to the right would
 * hit two of the endpoints of edges (one vertex of the diamond).
 */
TEST_F(TestSimplePolygonContains, InsideDiamondCentre) {
	EXPECT_TRUE(diamond_1000.contains(Point2(0, 0)));
}

/*
 * Test whether a point is inside a diamond when it's left of the centre.
 *
 * To a ray casting algorithm that casts purely horizontal rays, this shouldn't
 * make any difference from the InsideDiamondCentre test.
 */
TEST_F(TestSimplePolygonContains, InsideDiamondLeftOfCentre) {
	EXPECT_TRUE(diamond_1000.contains(Point2(-50, 0)));
}

/*
 * Test whether a point is outside a diamond when it's next to the tip of the
 * diamond.
 *
 * This is an edge case because the ray going towards the right from the point
 * rakes the tip of the diamond.
 */
TEST_F(TestSimplePolygonContains, OutsideDiamondLeftOfTip) {
	EXPECT_FALSE(diamond_1000.contains(Point2(-50, 500)));
}

/*
 * Test whether a point is outside a diamond when it's next to the bottom of the
 * diamond.
 *
 * This is an edge case because the ray going towards the right from the point
 * rakes the bottom of the diamond. It could be different from
 * OutsideDiamondLeftOfTip because of how the symmetry is broken when the ray
 * crosses the tips of two edges.
 */
TEST_F(TestSimplePolygonContains, OutsideDiamondLeftOfBottom) {
	EXPECT_FALSE(diamond_1000.contains(Point2(-50, -500)));
}

/*
 * Test whether a point is outside a square when it's next to the top of the
 * square.
 *
 * This is a literal edge case because there is now an edge of the square on top
 * of the ray that is being shot outside of the point in question.
 */
TEST_F(TestSimplePolygonContains, OutsideSquareLeftOfTop) {
	EXPECT_FALSE(square_1000.contains(Point2(-50, 1000)));
}

/*
 * Test whether a point is outside a square when it's next to the bottom of the
 * square.
 *
 * This is a literal edge case because there is now an edge of the square on top
 * of the ray that is being shot outside of the point in question. It could be
 * different from OutsideSquareLeftOfTop because of how the symmetry is broken
 * when the ray crosses the tips of two edges.
 */
TEST_F(TestSimplePolygonContains, OutsideSquareLeftOfBottom) {
	EXPECT_FALSE(square_1000.contains(Point2(-50, 0)));
}

/*
 * Test whether a point is inside the bottom (positive) half of an hourglass.
 */
TEST_F(TestSimplePolygonContains, InsideHourglassPositive) {
	EXPECT_TRUE(hourglass.contains(Point2(500, 250)));
}

/*
 * Test whether a point is inside the top (negative) half of an hourglass.
 *
 * According to the even-odd rule, which is the default, a winding order of -1
 * is odd meaning it should be considered inside.
 */
TEST_F(TestSimplePolygonContains, InsideHourglassNegative) {
	EXPECT_TRUE(hourglass.contains(Point2(500, 750)));
}

/*
 * Test whether a point whose ray is cast through a negative area is considered
 * outside the polygon.
 */
TEST_F(TestSimplePolygonContains, OutsideHourglassNextToNegative) {
	EXPECT_FALSE(hourglass.contains(Point2(0, 750)));
}

/*
 * Test whether a point whose ray goes through a self-intersection of a polygon
 * is considered outside the polygon.
 */
TEST_F(TestSimplePolygonContains, OutsideHourglassNextToIntersection) {
	EXPECT_FALSE(hourglass.contains(Point2(0, 500)));
}

/*
 * Test whether the centre of a self-intersecting five pointed star is indeed
 * considered outside the polygon.
 */
TEST_F(TestSimplePolygonContains, InsideStarCentreEvenOdd) {
	EXPECT_FALSE(five_pointed_star.contains(Point2(0, 0)));
}

/*
 * Test whether the centre of a self-intersecting five pointed star is indeed
 * considered inside the polygon if the fill type "nonzero" is used.
 */
TEST_F(TestSimplePolygonContains, OutsideStarCentreNonzero) {
	EXPECT_TRUE(five_pointed_star.contains(Point2(0, 0), FillType::NONZERO));
}

/*
 * Test whether the point of a self-intersecting five pointed star is indeed
 * considered inside the polygon regardless of the fill type.
 */
TEST_F(TestSimplePolygonContains, InsideStarPointEvenOdd) {
	EXPECT_TRUE(five_pointed_star.contains(Point2(-std::sin(TAU / 5) * 460, std::cos(TAU / 5) * 460)));
	EXPECT_TRUE(five_pointed_star.contains(Point2(-std::sin(TAU / 5) * 460, std::cos(TAU / 5) * 460), FillType::NONZERO));
}

TEST_F(TestSimplePolygonContains, LeftEdgeOfSquare) {
	EXPECT_TRUE(square_1000.contains(Point2(0, 500), FillType::EVEN_ODD, EdgeInclusion::INSIDE));
	EXPECT_FALSE(square_1000.contains(Point2(0, 500), FillType::EVEN_ODD, EdgeInclusion::OUTSIDE));
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