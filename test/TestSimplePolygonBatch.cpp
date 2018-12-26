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
#include <limits> //To simulate infinite memory and for the marker flag.
#include "SimplePolygon.h" //Example polygons.
#include "SimplePolygonBatch.h"
#include "helpers/MockDevice.h" //To mock out cl::Device.
#include "helpers/SimplePolygonBatchGroper.h" //The class under test.
#include "helpers/MockBuffer.h" //To prevent having to allocate on actual OpenCL devices.
#include "helpers/MockOpenCLContext.h" //To prevent working with actual OpenCL devices.

namespace apex {

/*
 * Some fixtures for the TestSimplePolygonBatch tests.
 */
class TestSimplePolygonBatch : public testing::Test {
protected:
	/*
	 * A polygon to test with. Put it in a batch, for instance.
	 *
	 * It's a triangle, so it'll have 3 vertices. It's also fairly fast to copy.
	 * This is a triangle of 20 by 10 units (so an area of 100).
	 */
	SimplePolygon triangle;

	/*
	 * A vector containing ten triangles.
	 */
	std::vector<SimplePolygon> ten_triangles;

	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> ten_triangles_batch;

	/*
	 * Provides access to ``SimplePolygonBatch``'s private members in order to
	 * test them.
	 */
	SimplePolygonBatchGroper<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> groper;

	TestSimplePolygonBatch() :
		ten_triangles_batch(ten_triangles.begin(), ten_triangles.end()) {
	}

	virtual void SetUp() {
		triangle.emplace_back(0, 0);
		triangle.emplace_back(20, 0);
		triangle.emplace_back(10, 20);
		for(size_t i = 0; i < 10; i++) {
			ten_triangles.push_back(triangle);
		}
		ten_triangles_batch = SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer>(ten_triangles.begin(), ten_triangles.end());
	}
};

/*
 * Tests the count and total vertices as generated by the constructor, with an
 * empty batch.
 */
TEST_F(TestSimplePolygonBatch, CountEmpty) {
	std::vector<SimplePolygon> empty;
	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> batch(empty.begin(), empty.end());
	groper.tested_batch = &batch;
	EXPECT_EQ(0, groper.count()) << "Batch is empty.";
	EXPECT_EQ(0, groper.total_vertices()) << "Batch is empty.";
}

/*
 * Tests the count and total vertices as generated by the constructor, with a
 * batch containing one triangle.
 */
TEST_F(TestSimplePolygonBatch, CountOne) {
	std::vector<SimplePolygon> one_triangle;
	one_triangle.push_back(triangle);
	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> batch(one_triangle.begin(), one_triangle.end());
	groper.tested_batch = &batch;

	EXPECT_EQ(1, groper.count()) << "1 triangle in this batch.";
	EXPECT_EQ(3, groper.total_vertices()) << "3 vertices per triangle, 1 triangle.";
}

/*
 * Tests the count and total vertices as generated by the constructor, with a
 * batch containing ten triangles.
 */
TEST_F(TestSimplePolygonBatch, CountTen) {
	groper.tested_batch = &ten_triangles_batch;
	EXPECT_EQ(10, groper.count()) << "10 triangles in this batch.";
	EXPECT_EQ(30, groper.total_vertices()) << "3 vertices per triangle, 10 triangles.";
}

/*
 * Tests ensure_fit on an empty batch.
 *
 * Whatever size you use, it should never do anything with the batch.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitEmpty) {
	std::vector<SimplePolygon> empty;
	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> batch(empty.begin(), empty.end());
	groper.tested_batch = &batch;

	bool result = groper.ensure_fit(100);
	EXPECT_TRUE(result) << "Empty, so it just fits.";
	EXPECT_TRUE(groper.subbatches().empty()) << "It should not create any subbatches since empty just fits.";

	result = groper.ensure_fit(0);
	EXPECT_TRUE(result) << "Empty, so it just fits even if there is no room.";
	EXPECT_TRUE(groper.subbatches().empty()) << "It should not create any subbatches since empty just fits.";
}

/*
 * Tests ensure_fit on a batch that is smaller than the maximum memory.
 *
 * The batch already fits, so it shouldn't create subbatches.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitAlreadyFits) {
	groper.tested_batch = &ten_triangles_batch;

	constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;
	const cl_ulong expected_memory_usage = 40 * vertex_size; //10 triangles, with one extra vertex_size per polygon.
	bool result = groper.ensure_fit(expected_memory_usage + 100); //Fits comfortably.
	EXPECT_TRUE(result) << "It's expected to fit easily in global memory.";
	EXPECT_TRUE(groper.subbatches().empty()) << "Since it fits in global memory, no subbatches are necessary.";

	result = groper.ensure_fit(expected_memory_usage); //Fits exactly.
	EXPECT_TRUE(result) << "It's expected to fit exactly in global memory.";
	EXPECT_TRUE(groper.subbatches().empty()) << "Since it fits in global memory, no subbatches are necessary.";
}

/*
 * Tests ensure_fit on a batch that is too big for the maximum memory, and needs
 * to be split in two.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitSplitInTwo) {
	groper.tested_batch = &ten_triangles_batch;

	constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;
	const bool result = groper.ensure_fit(30 * vertex_size); //Requires 40 vertex_sizes to fit, so this is too little memory.
	EXPECT_TRUE(result) << "None of the triangles have more than 29 vertices, because they're triangles.";
	ASSERT_EQ(2, groper.subbatches().size()) << "The batch got split up in 2 subbatches of 7 and 3 triangles.";
	SimplePolygonBatchGroper<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> subbatch_groper;
	subbatch_groper.tested_batch = &groper.subbatches()[0];
	EXPECT_EQ(7, subbatch_groper.count()) << "Each triangle requires 4 vertex_sizes. 7 * 4 = 28, which is the maximum that fits.";
	EXPECT_EQ(21, subbatch_groper.total_vertices()) << "Each triangle requires 4 vertex sizes. 7 * 4 = 28, which is the maximum that fits.";
	subbatch_groper.tested_batch = &groper.subbatches()[1];
	EXPECT_EQ(3, subbatch_groper.count()) << "Remaining 3 triangles.";
	EXPECT_EQ(9, subbatch_groper.total_vertices()) << "Remaining 3 triangles.";
}

/*
 * Tests ensure_fit on a batch that is too big for the maximum memory, and needs
 * to be split into many.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitSplitInFive) {
	groper.tested_batch = &ten_triangles_batch;

	constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;
	const bool result = groper.ensure_fit(8 * vertex_size); //Fits 2 triangles per batch, exactly.
	EXPECT_TRUE(result) << "Triangles take 4 vertex sizes, and there's room for 8, so it should fit.";
	EXPECT_EQ(5, groper.subbatches().size()) << "The batch got split into 5 groups of 2.";
	SimplePolygonBatchGroper<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> subbatch_groper;

	for(SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer>& subbatch : groper.subbatches()) {
		subbatch_groper.tested_batch = &subbatch;
		EXPECT_EQ(2, subbatch_groper.count()) << "The batch got split into 5 groups of 2.";
	}
}

/*
 * Tests ensure_fit on a batch that contains polygons with different sizes.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitUnevenSizes) {
	std::vector<SimplePolygon> uneven_sizes;
	uneven_sizes.emplace_back();
	for(size_t i = 0; i < 10; i++) { //First polygon gets 10 vertices.
		uneven_sizes.back().emplace_back(0, 0);
	}
	uneven_sizes.push_back(triangle); //Second polygon gets 3 vertices.
	uneven_sizes.push_back(triangle); //Third polygon gets 3 vertices too.
	uneven_sizes.emplace_back();
	for(size_t i = 0; i < 13; i++) { //Fourth polygon gets 13 vertices.
		uneven_sizes.back().emplace_back(0, 0);
	}
	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> batch(uneven_sizes.begin(), uneven_sizes.end());
	groper.tested_batch = &batch;

	constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;
	const bool result = groper.ensure_fit(14 * vertex_size); //Fits one large polygon or multiple triangles.
	EXPECT_TRUE(result) << "None of the polygons are more than 13 vertices.";
	ASSERT_EQ(3, groper.subbatches().size()) << "The batch got broken into 3.";
	SimplePolygonBatchGroper<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> subbatch_groper;
	subbatch_groper.tested_batch = &groper.subbatches()[0];
	EXPECT_EQ(1, subbatch_groper.count()) << "This batch contains just the first polygon. The second doesn't fit any more.";
	EXPECT_EQ(10, subbatch_groper.total_vertices()) << "This batch contains just the first polygon. The second doesn't fit any more.";
	subbatch_groper.tested_batch = &groper.subbatches()[1];
	EXPECT_EQ(2, subbatch_groper.count()) << "This batch contains the two triangles.";
	EXPECT_EQ(6, subbatch_groper.total_vertices()) << "This batch contains the two triangles.";
	subbatch_groper.tested_batch = &groper.subbatches()[2];
	EXPECT_EQ(1, subbatch_groper.count()) << "This batch contains just the last polygon.";
	EXPECT_EQ(13, subbatch_groper.total_vertices()) << "This batch contains just the last polygon.";
}

/*
 * Tests the behaviour when ensure_fit is called multiple times with
 * consecutively smaller maximum memory.
 *
 * The second time, it has to re-batch everything, discarding the original
 * subbatches.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitRebatch) {
	groper.tested_batch = &ten_triangles_batch;

	constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;
	bool result = groper.ensure_fit(20 * vertex_size); //Fits 5 triangles per batch initially.
	EXPECT_TRUE(result);
	ASSERT_EQ(2, groper.subbatches().size()) << "The 10 triangles have to be divided over 2 groups of 5.";
	SimplePolygonBatchGroper<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> subbatch_groper;
	for(SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer>& subbatch : groper.subbatches()) {
		subbatch_groper.tested_batch = &subbatch;
		EXPECT_EQ(5, subbatch_groper.count()) << "All 2 subbatches have 5 triangles.";
	}

	result = groper.ensure_fit(8 * vertex_size); //Fits only 2 triangles per batch now!
	EXPECT_TRUE(result);
	ASSERT_EQ(5, groper.subbatches().size()) << "The 10 triangles have to be divided over 5 groups of 2.";
	for(SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer>& subbatch : groper.subbatches()) {
		subbatch_groper.tested_batch = &subbatch;
		EXPECT_EQ(2, subbatch_groper.count()) << "All 5 subbatches have 2 triangles.";
	}
}

/*
 * Tests ensure_fit when it can't find a way to fit the polygons in memory
 * because one polygon is too big.
 */
TEST_F(TestSimplePolygonBatch, EnsureFitTooBig) {
	std::vector<SimplePolygon> polygons;
	polygons.emplace_back();
	for(size_t i = 0; i < 10; i++) {
		polygons.back().emplace_back(0, 0);
	}
	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> batch(polygons.begin(), polygons.end());
	groper.tested_batch = &batch;

	//First polygon doesn't fit.
	constexpr cl_ulong vertex_size = sizeof(cl_ulong) * 2;
	bool result = groper.ensure_fit(10 * vertex_size); //Needs 11 vertex_sizes (one for end marker), so it won't fit.
	EXPECT_FALSE(result) << "The first polygon needs 11 vertices, but there's space for 10.";
	EXPECT_TRUE(groper.subbatches().empty()) << "If it doesn't fit, it must clear any subbatches created.";

	//Last polygon doesn't fit.
	polygons.emplace_back();
	for(size_t i = 0; i < 20; i++) {
		polygons.back().emplace_back(0, 0);
	}
	batch = SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer>(polygons.begin(), polygons.end());
	groper.tested_batch = &batch;
	result = groper.ensure_fit(15 * vertex_size);
	EXPECT_FALSE(result) << "The second polygon needs 21 vertices, but there's space for 15.";
	EXPECT_TRUE(groper.subbatches().empty()) << "If it doesn't fit, it must clear any subbatches created.";

	//Second polygon doesn't fit (but the last one does).
	polygons.push_back(triangle);
	batch = SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer>(polygons.begin(), polygons.end());
	groper.tested_batch = &batch;
	result = groper.ensure_fit(15 * vertex_size);
	EXPECT_FALSE(result) << "The second polygon needs 21 vertices, but there's space for 15.";
	EXPECT_TRUE(groper.subbatches().empty()) << "If it doesn't fit, it must clear any subbatches created.";
}

/*
 * Tests loading an empty batch into device memory.
 */
TEST_F(TestSimplePolygonBatch, LoadEmpty) {
	std::vector<SimplePolygon> empty;
	SimplePolygonBatch<std::vector<SimplePolygon>::iterator, MockDevice, MockBuffer> batch(empty.begin(), empty.end());
	groper.tested_batch = &batch;

	MockDevice device;
	MockOpenCLContext& opencl_context = MockOpenCLContext::getInstance();
	opencl_context.addTestDevice(device);

	const bool result = groper.load<MockOpenCLContext, MockContext, MockCommandQueue>(device, 0);
	EXPECT_TRUE(result);

	typename std::unordered_map<const MockDevice*, MockBuffer>::iterator buffer = groper.loaded_in_memory().find(&device);
	ASSERT_NE(buffer, groper.loaded_in_memory().end()) << "Batch must now be marked as loaded in memory.";

	size_t memory_size;
	const cl_int success = buffer->second.getInfo(CL_MEM_SIZE, &memory_size);
	EXPECT_EQ(success, CL_SUCCESS);
	EXPECT_EQ(memory_size, 0) << "Memory requirement must be 0 since the batch was empty.";
}

/*
 * Tests loading a simple batch into memory, with just ten triangles.
 *
 * This is the happy path, the base case.
 */
TEST_F(TestSimplePolygonBatch, LoadTenTriangles) {
	groper.tested_batch = &ten_triangles_batch;

	MockDevice device;
	MockOpenCLContext& opencl_context = MockOpenCLContext::getInstance();
	opencl_context.addTestDevice(device);

	const bool result = groper.load<MockOpenCLContext, MockContext, MockCommandQueue>(device, 0);
	EXPECT_TRUE(result);

	typename std::unordered_map<const MockDevice*, MockBuffer>::iterator entry = groper.loaded_in_memory().find(&device);
	ASSERT_NE(entry, groper.loaded_in_memory().end()) << "Batch must now be marked as loaded in memory.";
	MockBuffer& buffer = entry->second;

	size_t memory_size;
	const cl_int success = buffer.getInfo(CL_MEM_SIZE, &memory_size);
	EXPECT_EQ(success, CL_SUCCESS);
	constexpr cl_ulong vertex_size = 2 * sizeof(cl_ulong);
	EXPECT_EQ(memory_size, 4 * vertex_size * 10) << "Needs 4 vertex sizes per triangle, 10 triangles.";
	for(size_t triangle = 0; triangle < ten_triangles.size(); triangle++) {
		const size_t triangle_offset = 4 * vertex_size * triangle;
		for(size_t vertex = 0; vertex < ten_triangles[triangle].size(); vertex++) {
			const size_t vertex_offset = triangle_offset + vertex_size * vertex;
			//Reconstruct coordinates from buffer's data.
			coord_t buffer_x = 0;
			memcpy(&buffer_x, &buffer.data[vertex_offset], sizeof(coord_t));
			coord_t buffer_y = 0;
			memcpy(&buffer_y, &buffer.data[vertex_offset + sizeof(coord_t)], sizeof(coord_t));
			EXPECT_EQ(buffer_x, ten_triangles[triangle][vertex].x) << "X coordinate of data read back from buffer.";
			EXPECT_EQ(buffer_y, ten_triangles[triangle][vertex].y) << "Y coordinate of data read back from buffer.";
		}

		//Validate marker flag.
		cl_ulong marker = 0;
		memcpy(&marker, &buffer.data[triangle_offset + vertex_size * ten_triangles[triangle].size()], sizeof(cl_ulong));
		EXPECT_EQ(marker, std::numeric_limits<cl_ulong>::max()) << "Marker flag is max value of a cl_ulong.";

		//Validate looping link.
		cl_ulong loop = 0;
		memcpy(&loop, &buffer.data[triangle_offset + vertex_size * ten_triangles[triangle].size() + sizeof(cl_ulong)], sizeof(cl_ulong));
		EXPECT_EQ(loop, triangle_offset) << "Looper flag must point to the beginning of the triangle.";
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