/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef COORDINATE_H
#define COORDINATE_H

#include <stdint.h>

namespace parallelogram {

/*
 * The type to use to store coordinates in space.
 *
 * This type is an integer-type rather than a floating point type, so no partial
 * unit coordinates are possible. This is intended to prevent inaccuracies due
 * to rounding errors.
 *
 * It must be exactly 32 bits long to cast properly to cl_int.
 */
typedef int32_t coord_t;

/*
 * The type to use to store a 2-dimensional area.
 *
 * In order to be able to store all areas that can be defined with all possible
 * coordinates, this type needs to be twice as long as coord_t, except the sign.
 * This requires 31 * 2 = 62 bits, plus an extra bit for the sign.
 *
 * This must also be a signed integer because polygons can have a negative area.
 */
typedef int64_t area_t;

}

#endif //COORDINATE_H

