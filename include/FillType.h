/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef FILLTYPE_H
#define FILLTYPE_H

namespace apex {

/*
 * Determines what parts of a self-intersecting polygon are considered inside
 * the polygon.
 */
enum FillType {
	/*
	 * Areas with an odd winding number are considered inside the polygon. Areas
	 * with an even winding number are considered outside the polygon.
	 *
	 * This means that if an area is surrounded multiple times by the same
	 * outline of a polygon, each loop inverts the area of whether it is
	 * considered inside the polygon or not.
	 */
	EVEN_ODD,

	/*
	 * Areas with a winding number that is not equal to 0 are considered inside
	 * the polygon. Areas with a winding number that is equal to 0 is considered
	 * outside the polygon.
	 *
	 * This means that regardless of how often the edge of a polygon loops
	 * around an area, any area that is surrounded at least once is considered
	 * to be inside the polygon.
	 */
	NONZERO
};

}

#endif //FILLTYPE_H