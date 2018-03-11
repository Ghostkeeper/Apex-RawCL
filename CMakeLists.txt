#Library for performing massively parallel computations on polygons.
#Copyright (C) 2018 Ghostkeeper
#This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
#You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8.12)
project(parallelogram)

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

#Installation.
#TODO