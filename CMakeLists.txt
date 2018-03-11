#Library for performing massively parallel computations on polygons.
#Copyright (C) 2018 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8.12)
project(parallelogram)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#Compiler settings.
if(NOT ${CMAKE_VERSION} VERSION_LESS 3.1)
	set(CMAKE_CXX_STANDARD 11)
else()
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

#Dependencies.
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

#Source files.
set(sources
	src/Point2.cpp
	src/Polygon.cpp
	src/SimplePolygon.cpp
)

#Compilation.
add_library(parallelogram SHARED ${sources})
target_include_directories(parallelogram PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Tests.
option(BUILD_TESTS "Build automated tests to verify correctness of the library." OFF)
if(BUILD_TESTS)
	enable_testing()
	find_package(GoogleTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})

	#Names of all tests. Each must have a file called "test/<name>.cpp" as source file.
	set(test_names
		TestPolygonArea
		TestPolygonTranslate
	)
	#To make sure that the tests are built before running them, add the building of these tests as an additional test.
	add_custom_target(build_all_tests)
	add_test(BuildTests "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target build_all_tests)
	foreach(test_case ${test_names})
		add_executable(${test_case} test/${test_case}.cpp)
		target_link_libraries(${test_case} ${GTEST_BOTH_LIBRARIES} parallelogram) #Link with GTest and the main Parallelogram library.
		add_dependencies(build_all_tests ${test_case}) #Make sure that this gets built as part of the build_all_tests target.
		add_test(${test_case} ${test_case})
		set_tests_properties(${test_case} PROPERTIES DEPENDS BuildTests)
	endforeach()
endif()

#Installation.
#TODO