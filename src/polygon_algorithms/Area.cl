/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

R"kernel(

/*
 * Computes the area of (a part of) a polygon.
 * \param input_data_points The coordinates of the vertices of the polygon.
 * \param output_areas For each work group, the output area of their part of the
 * polygon.
 * \param sums Some scratch space to store the computed areas before summing
 * them all up within one work group.
 */
void kernel area(global const int2* input_data_points, global long* output_areas, local long* sums) {
	//Compute the area contributed by one line segment.
	const int global_id = get_global_id(0);
	const int2 previous = input_data_points[global_id];
	const int2 next = input_data_points[global_id + 1];
	const int local_id = get_local_id(0);
	sums[local_id] = previous.x * next.y - previous.y * next.x;

	//Aggregate sum on the memory in this work group.
	const int local_size = get_local_size(0);
	int current_size = local_size;
	for(int offset = (current_size + 1) / 2; offset > 1; offset = (current_size + 1) / 2) {
		barrier(CLK_LOCAL_MEM_FENCE);
		if(local_id < offset && local_id + offset < current_size) {
			sums[local_id] += sums[local_id + offset];
		}
		current_size = offset;
	}

	//Copy the resulting sum to the output.
	barrier(CLK_LOCAL_MEM_FENCE);
	if(local_id == 0) {
		const int workgroup_id = global_id / local_size;
		output_areas[workgroup_id] = sums[local_id];
	}
}

)kernel"