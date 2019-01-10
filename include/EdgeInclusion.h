/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2019 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef APEX_EDGEINCLUSION_H
#define APEX_EDGEINCLUSION_H

namespace apex {

/*
 * Determines whether edges are included in the coverage of a polygon.
 *
 * If a point lies exactly on the edge of a polygon, is that considered inside
 * the polygon or outside?
 */
enum EdgeInclusion {
	/*
	 * Points on the edge of a polygon are considered to be inside the polygon.
	 */
	INSIDE,

	/*
	 * Points on the edge of a polygon are considered to be outside the polygon.
	 */
	OUTSIDE
};

}

#endif //APEX_EDGEINCLUSION_H