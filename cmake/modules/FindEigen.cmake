#Library for performing massively parallel computations on polygons.
#Copyright (C) 2018 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

#Find module for the Eigen linear algebra library.
#
#This module exports the following variables:
# - EIGEN_FOUND: True if the Eigen library was found, or False if it wasn't.
# - EIGEN_INCLUDE_DIRS: The location of the Eigen headers, if found.
# - EIGEN_VERSION: The version of Eigen that was found.
# - EIGEN_MAJOR_VERSION: The major version number of Eigen (TODO: Only matches for version 3).
# - EIGEN_MINOR_VERSION: The minor version number of Eigen.
# - EIGEN_PATCH_VERSION: The patch version number of Eigen.
# - EIGEN_DEFINITIONS: Compiler switches required for using Eigen.

find_package(PkgConfig QUIET)
pkg_check_modules(PC_EIGEN QUIET eigen)
set(EIGEN_DEFINITIONS ${PC_EIGEN_CFLAGS_OTHER})

find_path(EIGEN_INCLUDE_DIRS Eigen/Core
	HINTS ${PC_EIGEN_INCLUDEDIR} ${PC_EIGEN_INCLUDE_DIRS} "${EIGEN_ROOT}" "$ENV{EIGEN_ROOT}"
	PATHS "$ENV{PROGRAMFILES}/Eigen" "$ENV{PROGRAMW6432}/Eigen"
	PATH_SUFFIXES eigen2 eigen3 include/eigen2 include/eigen3 include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(eigen3 DEFAULT_MSG EIGEN_INCLUDE_DIRS)

mark_as_advanced(EIGEN_INCLUDE_DIRS)

if(EIGEN_FOUND) #Found an existing installation.
	#Getting Eigen's version number from their source code!
	#Take the macro file that contains the version number and append a program to it that outputs the values of the macro.
	file(READ "${CMAKE_CURRENT_BINARY_DIR}/eigen-git-mirror-master/Eigen/src/Core/util/Macros.h" _eigen_macros_file)
	file(READ "${CMAKE_MODULE_PATH}/GetEigenVersion/GetEigenVersion.cpp" _get_eigen_version_original)
	file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/GetEigenVersionPatched.cpp" "#define EIGEN_DEVICE_FUNC\n\n${_eigen_macros_file}\n\n${_get_eigen_version_original}") #Append a simple program to one of the macro files.
	#Compile this program, get its result.
	try_run(RUN_RESULT COMPILE_RESULT
		"${CMAKE_CURRENT_BINARY_DIR}"
		"${CMAKE_CURRENT_BINARY_DIR}/GetEigenVersionPatched.cpp"
		RUN_OUTPUT_VARIABLE _run_output #The output of this application is #;#;#, so due to the semicolons it gets interpreted as a list in CMake.
	)
	list(GET _run_output 0 EIGEN_MAJOR_VERSION)
	list(GET _run_output 1 EIGEN_MINOR_VERSION)
	list(GET _run_output 2 EIGEN_PATCH_VERSION)
	set(EIGEN_VERSION ${EIGEN_MAJOR_VERSION}.${EIGEN_MINOR_VERSION}.${EIGEN_PATCH_VERSION})
	message(STATUS "Found Eigen version: ${EIGEN_VERSION} at ${EIGEN_INCLUDE_DIRS}.")
else()
	#Then optionally clone Eigen ourselves.
	option(BUILD_EIGEN "Build Eigen from source." ON) #This is a lie actually, since Eigen is header-only and doesn't need any compiling.
	if(BUILD_EIGEN)
		message(STATUS "Building Eigen from source.")
		#include(ExternalProject)
		#ExternalProject_Add(Eigen
		#	GIT_REPOSITORY https://github.com/eigenteam/eigen-git-mirror #We're using the mirror so that we don't need to depend on Mercurial as well as Git. Just Git now.
		#	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
		#	INSTALL_COMMAND "" #Don't immediately install it. We just need the headers.
		#)
		#set(EIGEN_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include/eigen")
		#set(EIGEN_FOUND TRUE)

		#Getting Eigen's version number from their source code!
		#First download and extract the source code.
		file(DOWNLOAD https://github.com/eigenteam/eigen-git-mirror/archive/master.zip "${CMAKE_CURRENT_BINARY_DIR}/eigen-git-mirror-master.zip")
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar x "${CMAKE_CURRENT_BINARY_DIR}/eigen-git-mirror-master.zip"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		)
		set(EIGEN_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/eigen-git-mirror-master")
		set(EIGEN_FOUND TRUE)
		#Take the macro file that contains the version number and append a program to it that outputs the values of the macro.
		file(READ "${EIGEN_INCLUDE_DIRS}/Eigen/src/Core/util/Macros.h" _eigen_macros_file)
		file(READ "${CMAKE_MODULE_PATH}/GetEigenVersion/GetEigenVersion.cpp" _get_eigen_version_original)
		file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/GetEigenVersionPatched.cpp" "#define EIGEN_DEVICE_FUNC\n\n${_eigen_macros_file}\n\n${_get_eigen_version_original}") #Append a simple program to one of the macro files.
		#Compile this program, get its result.
		try_run(RUN_RESULT COMPILE_RESULT
			"${CMAKE_CURRENT_BINARY_DIR}"
			"${CMAKE_CURRENT_BINARY_DIR}/GetEigenVersionPatched.cpp"
			RUN_OUTPUT_VARIABLE _run_output #The output of this application is #;#;#, so due to the semicolons it gets interpreted as a list in CMake.
		)
		list(GET _run_output 0 EIGEN_MAJOR_VERSION)
		list(GET _run_output 1 EIGEN_MINOR_VERSION)
		list(GET _run_output 2 EIGEN_PATCH_VERSION)
		set(EIGEN_VERSION ${EIGEN_MAJOR_VERSION}.${EIGEN_MINOR_VERSION}.${EIGEN_PATCH_VERSION})
		message(STATUS "Found Eigen version: ${EIGEN_VERSION}")
	endif()
endif()

set(EIGEN_VERSION ${EIGEN_MAJOR_VERSION}.${EIGEN_MINOR_VERSION}.${EIGEN_PATCH_VERSION})