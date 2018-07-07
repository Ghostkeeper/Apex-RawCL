/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef TESTSIMPLEPOLYGONAREA
#define TESTSIMPLEPOLYGONAREA

#include <cmath> //To construct a circle.
#include <gtest/gtest.h>
#include "OpenCLDevices.h" //To get the OpenCL devices.
#include "SimplePolygon.h" //To construct simple polygons to test on.
#include "SimplePolygonTestGroper.h" //To get access to SimplePolygon's private members.

#define PI 3.14159265358979

namespace parallelogram {

/*
 * Some fixtures for the TestSimplePolygonArea tests.
 */
class TestSimplePolygonArea : public testing::Test {
protected:
	/*
	 * A few polygons to test with.
	 */
	SimplePolygon square_1000;

	/*
	 * The devices to run tests on.
	 */
	std::vector<cl::Device> devices;

	/*
	 * Provides access to ``SimplePolygon``'s private members in order to test
	 * them.
	 */
	SimplePolygonTestGroper groper;

	virtual void SetUp() {
		square_1000.emplace_back(0, 0);
		square_1000.emplace_back(1000, 0);
		square_1000.emplace_back(1000, 1000);
		square_1000.emplace_back(0, 1000);

		devices = OpenCLDevices::getInstance().getAll();
	}
};

/*
 * Test for the area of an empty simple polygon being 0.
 */
TEST_F(TestSimplePolygonArea, InitialAreaIsZero) {
	SimplePolygon empty_polygon;
	groper.tested_simple_polygon = &empty_polygon;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(0, groper.area_opencl(device));
	}
	EXPECT_EQ(0, groper.area_host());
}

/*
 * Test the area of a 1000 by 1000 square.
 */
TEST_F(TestSimplePolygonArea, Square1000) {
	groper.tested_simple_polygon = &square_1000;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(1000 * 1000, groper.area_opencl(device));
	}
	EXPECT_EQ(1000 * 1000, groper.area_host());
}

/*
 * Test the area of a 1000 by 1000 square that's completely in the negative
 * coordinate area.
 */
TEST_F(TestSimplePolygonArea, Square1000NegativeCoordinates) {
	square_1000.translate(-1024, -1024);
	groper.tested_simple_polygon = &square_1000;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(1000 * 1000, groper.area_opencl(device));
	}
	EXPECT_EQ(1000 * 1000, groper.area_host());
}

/*
 * Test the area of a 1000 by 1000 square that's in the negative area of the Y
 * dimension but the positive area of the X dimension.
 */
TEST_F(TestSimplePolygonArea, Square1000NegativeY) {
	square_1000.translate(0, -1024);
	groper.tested_simple_polygon = &square_1000;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(1000 * 1000, groper.area_opencl(device));
	}
	EXPECT_EQ(1000 * 1000, groper.area_host());
}

/*
 * Test the area of a 1000 by 1000 square that's partially in the positive area
 * and partially in the negative area.
 */
TEST_F(TestSimplePolygonArea, Square1000AroundOrigin) {
	square_1000.translate(-512, -512);
	groper.tested_simple_polygon = &square_1000;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(1000 * 1000, groper.area_opencl(device));
	}
	EXPECT_EQ(1000 * 1000, groper.area_host());
}

/*
 * Test calculating the area of a triangle with a 1000-unit wide base.
 */
TEST_F(TestSimplePolygonArea, Triangle1000) {
	SimplePolygon triangle_1000;
	triangle_1000.emplace_back(24, 24);
	triangle_1000.emplace_back(1024, 24);
	triangle_1000.emplace_back(524, 1024);

	groper.tested_simple_polygon = &triangle_1000;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(1000 * 1000 / 2, groper.area_opencl(device));
	}
	EXPECT_EQ(1000 * 1000 / 2, groper.area_host());
}

/*
 * Tests the area of a long, slim rectangle.
 */
TEST_F(TestSimplePolygonArea, Rectangle) {
	SimplePolygon rectangle;
	rectangle.emplace_back(0, 0);
	rectangle.emplace_back(1000, 0);
	rectangle.emplace_back(1000, 1);
	rectangle.emplace_back(0, 1);

	groper.tested_simple_polygon = &rectangle;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(1000 * 1, groper.area_opencl(device));
	}
	EXPECT_EQ(1000 * 1, groper.area_host());
}

/*
 * Test computing the area of a concave simple polygon.
 */
TEST_F(TestSimplePolygonArea, Concave) {
	//This concave shape is a triangle with base length 100 and height 100, with a triangle subtracted from it at the base.
	//The subtracted triangle has base length 100 and height 50.
	//The area of this concave shape is then (100 * 100) / 2 - (100 * 50) / 2.
	SimplePolygon concave;
	concave.emplace_back(10, 10);
	concave.emplace_back(60, 60);
	concave.emplace_back(110, 10);
	concave.emplace_back(60, 110);

	groper.tested_simple_polygon = &concave;
	for(const cl::Device& device : devices) {
		EXPECT_EQ(100 * 100 / 2 - 100 * 50 / 2, groper.area_opencl(device));
	}
	EXPECT_EQ(100 * 100 / 2 - 100 * 50 / 2, groper.area_host());
}

/*
 * Test the area of a negative square, where the vertices are going the other
 * way around.
 *
 * The area should then be negative.
 */
TEST_F(TestSimplePolygonArea, NegativeSquare) {
	SimplePolygon negative_square_1000;
	negative_square_1000.emplace_back(0, 0);
	negative_square_1000.emplace_back(0, 1000);
	negative_square_1000.emplace_back(1000, 1000);
	negative_square_1000.emplace_back(1000, 0);

	EXPECT_EQ(-square_1000.area(), negative_square_1000.area());
}

/*
 * Tests the area of a self-intersecting simple polygon.
 */
TEST_F(TestSimplePolygonArea, SelfIntersecting) {
	SimplePolygon hourglass; //An hourglass figure where two of the edges of the polygon intersect.
	hourglass.emplace_back(0, 0);
	hourglass.emplace_back(100, 0);
	hourglass.emplace_back(25, 75); //The top of the hourglass is half as wide as the bottom, resulting in a quarter of the area!
	hourglass.emplace_back(75, 75);

	EXPECT_EQ(100 * 50 / 2 - 50 * 25 / 2, hourglass.area());
}

/*
 * Test the area of a line.
 */
TEST_F(TestSimplePolygonArea, Line) {
	SimplePolygon line; //A diagonal line.
	line.emplace_back(0, 0);
	line.emplace_back(100, 100);

	EXPECT_EQ(0, line.area()); //Lines have no area.
}

/*
 * Tests computing the area of a simple polygon that consists of a single
 * vertex.
 */
TEST_F(TestSimplePolygonArea, Point) {
	SimplePolygon point;
	point.emplace_back(25, 25);

	EXPECT_EQ(0, point.area());
}

/*
 * Tests computing the area of a regular simple polygon that consists of many
 * vertices.
 *
 * This is tested with a regular polygon that approaches a circle. The ground
 * truth is calculated with the formula for the area of a regular polygon:
 * 1/2 * n * r^2 * sin(2*pi / n)
 */
TEST_F(TestSimplePolygonArea, Circle) {
	SimplePolygon circle;
	constexpr size_t num_vertices = 1000000;
	constexpr coord_t radius = 1000000;
	for(size_t vertex = 0; vertex < num_vertices; vertex++) { //Construct a circle with lots of vertices.
		const coord_t x = std::lround(std::cos(PI * 2 / num_vertices * vertex) * radius); //This rounding naturally introduces error, so we must allow some lenience in the output.
		const coord_t y = std::lround(std::sin(PI * 2 / num_vertices * vertex) * radius);
		circle.emplace_back(x, y);
	}

	constexpr area_t ground_truth = num_vertices * radius * radius * std::sin(PI * 2 / num_vertices) / 2; //Formula for area of regular polygon.
	EXPECT_NEAR(ground_truth, circle.area(), std::sqrt(num_vertices) / num_vertices / 6 * (PI * radius * radius - PI * (radius - 1) * (radius - 1))); //Allow some error due to rounding of input coordinates.
}

}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif //TESTPOLYGONAREA

