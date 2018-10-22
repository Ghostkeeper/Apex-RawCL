/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef SIMPLEPOLYGONBATCH_H
#define SIMPLEPOLYGONBATCH_H

#include <iterator> //To iterate over the contents of the batch.

namespace parallelogram {

class SimplePolygon;

/*
 * Class used to process operations on many simple polygons together.
 *
 * Performing the same instruction on many simple polygons could hide the
 * overhead associated with using some processing devices that have greater
 * performance otherwise.
 *
 * The methods of this class will act as if operating on all of the simple
 * polygons referred to by this batch separately. The results of these
 * operations will generally be stored in an output vector of the same size as
 * the input vector, where the result of each entry in the vector would be the
 * same result as if the method would be called separately on each polygon of
 * the input.
 *
 * The batch never stores a copy of the polygon data, except when it needs to
 * copy this data to another device for processing there.
 */
class SimplePolygonBatch {
public:
	/*
	 * Batches a bunch of simple polygons together to operate on separately.
	 *
	 * In order to batch a group of simple polygons, you need to indicate a
	 * range in an iterable object that is supposed to be batched.
	 *
	 * Note that the performance of this iterator is crucial to the performance
	 * of the algorithms on the batch. Supplying an iterator of an unordered set
	 * would induce multiple iterations over the set, which has a lot of unused
	 * entries and would have worse performance when compared to, for instance,
	 * an iterator of a vector.
	 * \param begin The first element of a range of simple polygons to batch.
	 * \param end The element after the last element of the range of simple
	 * polygons to batch.
	 */
	SimplePolygonBatch(const std::iterator<std::forward_iterator_tag, SimplePolygon> begin, const std::iterator<std::forward_iterator_tag, SimplePolygon> end);

private:
	/*
	 * The first element of a range of simple polygons to batch.
	 */
	const std::iterator<std::forward_iterator_tag, SimplePolygon> begin;

	/*
	 * The last element of a range of simple polygons to batch.
	 */
	const std::iterator<std::forward_iterator_tag, SimplePolygon> end;
};

}

#endif //SIMPLEPOLYGONBATCH_H