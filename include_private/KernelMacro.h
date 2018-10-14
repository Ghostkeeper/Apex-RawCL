/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef KERNELMACRO_H
#define KERNELMACRO_H

/*
 * A macro to wrap your kernel definition.
 *
 * The macro will turn your kernel definition into a string, but your IDE will
 * not recognise it as a string if it doesn't evaluate the macro. So for the
 * compiler it'll be a proper string, but your IDE should still be using C++'s
 * syntax highlighting. Handy!
 */
#define PARALLELOGRAM_KERNEL(...) #__VA_ARGS__

#endif //KERNELMACRO_H