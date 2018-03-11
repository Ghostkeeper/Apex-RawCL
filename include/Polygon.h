/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef POLYGON_H
#define POLYGON_H

#include <vector> //To list the simple polygons.
#include "SimplePolygon.h" //The simple polygons that this complex polygon consists of.

namespace parallelogram {

/*
 * A complex polygon.
 */
class Polygon {
    /*
     * Initialise an empty complex polygon.
     *
     * The polygon will have no area at all.
     */
    Polygon();

private:
    /*
     * The simple polygons that this polygon consists of.
     */
    std::vector<SimplePolygon> simple_polygons;
};

}

#endif //POLYGON_H