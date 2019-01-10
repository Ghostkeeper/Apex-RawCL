/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGONTESTGROPER_H
#define SIMPLEPOLYGONTESTGROPER_H

#include "Device.h"
#include "SimplePolygon.h" //The polygon we're exposing the private members of.

namespace apex {

/*
 * Class that allows access to private members of ``SimplePolygon`` in order to
 * test them.
 *
 * This breaks the secrecy of ``SimplePolygon`` for the purpose of testing.
 */
class SimplePolygonTestGroper {
public:
	/*
	 * The polygon that is being tested right now.
	 */
	SimplePolygon* tested_simple_polygon;

	/*
	 * Calls the private function "area_host" on the tested simple polygon and
	 * returns the result.
	 */
	area_t area_host() const;

	/*
	 * Calls the private function "area_opencl" on the tested simple polygon and
	 * returns the result.
	 */
	area_t area_opencl(const Device<>& device) const;

	/*
	 * Calls the private function "contains_host" on the tested simple polygon
	 * and returns the result.
	 */
	bool contains_host(const Point2& point, const EdgeInclusion& include_edge = EdgeInclusion::INSIDE, const FillType& fill_type = FillType::NONZERO) const;

	/*
	 * Calls the private function "contains_opencl" on the tested simple polygon
	 * and returns the result.
	 */
	bool contains_opencl(const Device<>& device, const Point2& point, const EdgeInclusion& include_edge = EdgeInclusion::INSIDE, const FillType& fill_type = FillType::NONZERO) const;
};

}

#endif //SIMPLEPOLYGONTESTGROPER_H