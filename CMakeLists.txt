#Library for performing massively parallel computations on polygons.
#Copyright (C) 2018 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.9)
project(apex VERSION 0.0.1 DESCRIPTION "Library for performing massively parallel computations on polygons.")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#Compiler settings.
if(NOT ${CMAKE_VERSION} VERSION_LESS 3.1)
	set(CMAKE_CXX_STANDARD 17)
else()
	set(CMAKE_CXX_FLAGS "-std=c++17")
endif()

#Source files.
set(sources
	src/ApexException.cpp
	src/Point2.cpp
	src/Polygon.cpp
	src/SimplePolygon.cpp

	src/polygon_algorithms/Area.cpp
	src/polygon_algorithms/Contains.cpp

	src/private/BenchmarkData.cpp
	src/private/Benchmarks.cpp
	src/private/OpenCLContext.cpp
	src/private/OpenCLDevices.cpp
)

#Compilation.
add_library(apex SHARED ${sources})
target_include_directories(apex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(apex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include_private)
target_include_directories(apex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/benchmark) #To make the benchmarker a friend class.

#Verify some compile features that we need so we can give an early error if it's not.
target_compile_features(apex PUBLIC cxx_std_11)
target_compile_features(apex PUBLIC cxx_constexpr)
target_compile_features(apex PUBLIC cxx_defaulted_functions)
target_compile_features(apex PUBLIC cxx_lambdas)
#target_compile_features(apex PUBLIC cxx_noexcept) #Reminder for the future.
target_compile_features(apex PUBLIC cxx_nonstatic_member_init)
target_compile_features(apex PUBLIC cxx_range_for)
target_compile_features(apex PUBLIC cxx_right_angle_brackets)
target_compile_features(apex PUBLIC cxx_rvalue_references)
target_compile_features(apex PUBLIC cxx_variadic_macros)
target_compile_features(apex PRIVATE cxx_generalized_initializers)
target_compile_features(apex PRIVATE cxx_static_assert)
target_compile_features(apex PRIVATE cxx_thread_local)

#Dependencies.
find_package(OpenCL REQUIRED)
target_include_directories(apex PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(apex ${OpenCL_LIBRARIES})
if(OpenCL_VERSION_MAJOR EQUAL 1)
	set(_opencl_hpp_file cl.hpp)
elseif(OpenCL_VERSION_MAJOR EQUAL 2)
	set(_opencl_hpp_file cl2.hpp)
else()
	set(_opencl_hpp_file cl.hpp)
	message(WARNING "You are using an unknown version of OpenCL: ${OpenCL_VERSION_STRING}. Assuming it's OpenCL 1.")
endif()
if(APPLE)
	set(_opencl_hpp_directory OpenCL)
else()
	set(_opencl_hpp_directory CL)
endif()
add_definitions(-DOPENCL_CPP_BINDINGS="${_opencl_hpp_directory}/${_opencl_hpp_file}")

#Tests.
option(BUILD_TESTS "Build automated tests to verify correctness of the library." OFF)
if(BUILD_TESTS)
	enable_testing()

	find_package(GoogleTest REQUIRED)

	#Build a library of helpers for the tests. We only need to compile this once.
	set(test_helpers_sources
		test/helpers/SimplePolygonTestGroper.cpp
		test/helpers/MockDevice.cpp
		test/helpers/MockOpenCLContext.cpp
	)
	add_library(test_helpers ${test_helpers_sources})
	target_link_libraries(test_helpers apex)
	target_include_directories(test_helpers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test/helpers)
	target_include_directories(test_helpers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include_private) #This test library has access to private headers.

	#Names of all tests. Each must have a file called "test/<name>.cpp" as source file.
	#Instead of slashes, use periods.
	set(test_names
		TestPolygonArea
		TestPolygonTranslate
		TestSimplePolygonArea
		TestSimplePolygonBatch
		TestSimplePolygonContains
		private.TestOpenCLDevices
	)

	#To make sure that the tests are built before running them, add the building of these tests as an additional test.
	add_custom_target(build_all_tests)
	add_test(BuildTests "${CMAKE_COMMAND}" --build ${CMAKE_CURRENT_BINARY_DIR} --target build_all_tests)
	foreach(test_case ${test_names})
		#Expand periods to slashes for the path to the test case, since CMake can't handle slashes in target names.
		string(REPLACE . / test_path ${test_case})
		#Make sure that the directory exists.
		get_filename_component(test_dirname ${test_path} DIRECTORY)
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_dirname})

		find_package(Threads REQUIRED)

		add_executable(${test_case} test/${test_path}.cpp ${test_helpers})
		target_link_libraries(${test_case} ${GOOGLETEST_BOTH_LIBRARIES} ${OpenCL_LIBRARIES} Threads::Threads apex test_helpers) #Link with GoogleTest, OpenCL and the main Apex library.
		target_include_directories(${test_case} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test/helpers)
		target_include_directories(${test_case} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
		target_include_directories(${test_case} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include_private) #This test has access to private headers.
		target_include_directories(${test_case} PRIVATE ${GOOGLETEST_INCLUDE_DIRS})
		target_include_directories(${test_case} PRIVATE ${OpenCL_INCLUDE_DIRS})
		add_dependencies(${test_case} apex test_helpers GoogleTest)
		add_dependencies(build_all_tests ${test_case}) #Make sure that this gets built as part of the build_all_tests target.
		add_test(${test_case} ${test_case})
		set_tests_properties(${test_case} PROPERTIES DEPENDS BuildTests)
	endforeach()
endif()

#Benchmarking application.
option(BUILD_BENCHMARKER "Build tool to compute benchmarks for your current set-up." ON)
if(BUILD_BENCHMARKER)
	find_package(Eigen REQUIRED)
	set(benchmark_sources
		benchmark/Benchmarker.cpp
		benchmark/SimplePolygonBenchmark.cpp
	)
	add_executable(benchmarker ${benchmark_sources})
	target_include_directories(benchmarker PRIVATE ${EIGEN_INCLUDE_DIRS})
	target_include_directories(benchmarker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
	target_include_directories(benchmarker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_include_directories(benchmarker PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include_private) #The benchmarker has access to private headers to call specific implementations.
	target_include_directories(benchmarker PRIVATE ${OpenCL_INCLUDE_DIRS})
	target_link_libraries(benchmarker apex)
	target_link_libraries(benchmarker ${OpenCL_LIBRARIES})
	if(BUILD_EIGEN)
		add_dependencies(benchmarker Eigen)
	endif()
endif()

#Installation.
include(GNUInstallDirs)
#TODO: Install header files.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/apex.pc.in ${CMAKE_CURRENT_BINARY_DIR}/apex.pc @ONLY) #Have to use @ONLY since ${} has special meaning for .pc files.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/apex.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)