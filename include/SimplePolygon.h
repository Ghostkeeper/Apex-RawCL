/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGON_H
#define SIMPLEPOLYGON_H

#include <vector> //To store the vertices.
#include "Point2.h" //The type to represent a vertex.

namespace parallelogram {

/*
 * Represents a simple polygon.
 *
 * Simple polygons consist of a single boundary. This is not necessarily a
 * simple polygon in the mathematical sense since there is no checking whether
 * the boundary intersects itself.
 */
class SimplePolygon : std::vector<Point2> {
    /*
     * Constructs an empty simple polygon.
     *
     * The polygon will have no vertices or edges and no area.
     */
    SimplePolygon();
};

}

#endif //SIMPLEPOLYGON_H

