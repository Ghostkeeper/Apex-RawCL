#Library for performing massively parallel computations on polygons.
#Copyright (C) 2018 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

#First try finding the library using CMake's built-in find script.
find_package(GTest QUIET)

if(GTEST_FOUND)
	message(STATUS "Found GoogleTest.")
	set(GOOGLETEST_FOUND TRUE)
	set(GOOGLETEST_INCLUDE_DIRS "${GTEST_INCLUDE_DIRS}")
	set(GOOGLETEST_LIBRARIES "${GTEST_LIBRARIES}")
	set(GOOGLETEST_MAIN_LIBRARIES "${GTEST_MAIN_LIBRARIES}")
	set(GOOGLETEST_BOTH_LIBRARIES "${GTEST_BOTH_LIBRARIES}")
else() #GTest was not found.
	#Give the option to build Google Test from source.
	option(BUILD_GOOGLETEST "Build Google Test from source." ON)
	if(BUILD_GOOGLETEST)
		message(STATUS "Building Google Test from source.")
		include(ExternalProject)
		ExternalProject_Add(GoogleTest
			GIT_REPOSITORY https://github.com/google/googletest
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
			INSTALL_COMMAND "" #If we want to build it just for this project, no need to install it.
		)
		set(GOOGLETEST_FOUND TRUE)
		set(GOOGLETEST_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/GoogleTest-prefix/src/GoogleTest/googletest/include")
		set(GOOGLETEST_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/GoogleTest-prefix/src/GoogleTest-build/googlemock/gtest/libgtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(GOOGLETEST_MAIN_LIBRARIES "${CMAKE_CURRENT_BINARY_DIR}/GoogleTest-prefix/src/GoogleTest-build/googlemock/gtest/libgtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(GOOGLETEST_BOTH_LIBRARIES "${GOOGLETEST_LIBRARIES};${GOOGLETEST_MAIN_LIBRARIES}")
	else()
		if(GoogleTest_FIND_REQUIRED)
			message(FATAL_ERROR "Could NOT find Google Test.")
		else()
			message(WARNING "Could NOT find Google Test.")
		endif()
	endif()
endif()

mark_as_advanced(GOOGLETEST_INCLUDE_DIRS)
mark_as_advanced(GOOGLETEST_LIBRARIES)
mark_as_advanced(GOOGLETEST_MAIN_LIBRARIES)
mark_as_advanced(GOOGLETEST_BOTH_LIBRARIES)