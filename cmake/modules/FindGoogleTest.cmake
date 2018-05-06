#Library for performing massively parallel computations on polygons.
#Copyright (C) 2018 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

#First try finding the library using CMake's built-in find script.
find_package(GTest)

if(NOT GTEST_FOUND)
	#Give the option to build Google Test from source.
	option(BUILD_GTEST "Build Google Test from source." ON)
	if(BUILD_GTEST)
		message(STATUS "Building Google Test from source.")
		include(ExternalProject)
		ExternalProject_Add(GTest
			GIT_REPOSITORY https://github.com/google/googletest
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
		)
		set(GTEST_FOUND TRUE)
		set(GTEST_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include/gtest)
		set(GTEST_LIBRARIES ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX})
		set(GTEST_MAIN_LIBRARIES ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/libgtest-main${CMAKE_STATIC_LIBRARY_SUFFIX})
		set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
		mark_as_advanced(GTEST_INCLUDE_DIRS)
		mark_as_advanced(GTEST_LIBRARIES)
		mark_as_advanced(GTEST_MAIN_LIBRARIES)
		mark_as_advanced(GTEST_BOTH_LIBRARIES)
	else()
		if(GoogleTest_FIND_REQUIRED)
			message(FATAL_ERROR "Could NOT find Google Test.")
		else()
			message(WARNING "Could NOT find Google Test.")
		endif()
	endif()
endif()