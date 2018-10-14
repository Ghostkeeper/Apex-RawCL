/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

R"kernel(

/*
 * Computes the winding number of (a part of) a polygon around a certain point.
 *
 * Adding all the winding numbers of every edge together gives us the actual
 * winding number of the polygon around the point, which allows us to determine
 * if the point should be considered inside the polygon.
 * \param input_data_points The coordinates of the vertices of the polygon.
 * \param total_vertices The total number of vertices in the entire polygon (not
 * just this work group).
 * \param point The point to wind around.
 * \param include_edges Whether to include edges inside the polygon or not.
 * \param output_windings For each work group, the winding number for their part
 * of the polygon.
 * \param sums Some scratch space to store the computed winding numbers before
 * summing them all up within one work group.
 */
void kernel contains(global const int2* input_data_points, const long total_vertices, const int2 point, const int include_edges, global int* output_windings, local int* sums) {
	//Compute the winding number contributed by one line segment.
	const uint global_id = get_global_id(0);
	const int2 previous = input_data_points[global_id];
	const int2 next = input_data_points[global_id + 1];
	const uint local_id = get_local_id(0);

	//The winding number algorithm outlined in Contains.cpp.
	const long point_is_left = (next.x - previous.x) * (point.y - previous.y) - (next.y - previous.y) * (point.x - previous.x);
	char winding_number = 0;
	if(previous.y < next.y || previous.y > next.y) { //Rising or falling edge.
		const bool inversed = previous.y > next.y; //When the edge is falling instead of rising, many things are inversed.
		const char invert = (!inversed) * 2 - 1; //Either -1 or 1. Multiply by this to invert the sign if inversed.
		//For the edge case of the ray hitting a vertex exactly, count rays hitting the lower vertices along with this edge.
		if((point.y >= previous.y) ^ inversed && (point.y < next.y) ^ inversed) { //Crosses height of point.
			if(point_is_left * invert > 0 || (point_is_left == 0 && include_edges != inversed)) {
				winding_number = invert;
			}
		}
	} else if(previous.y == point.y) { //Horizontal line at exactly the height of the point.
		if(previous.x < next.x && point.x >= previous.x && point.x <= next.x) { //Going to the left.
			if(include_edges == 0) {
				winding_number = -1;
			}
		} else if(previous.x >= next.x && point.x <= previous.x && point.x >= next.x) { //Going to the right.
			if(include_edges == 1) {
				winding_number = 1;
			}
		}
	}
	sums[local_id] = winding_number;

	//Aggregate sum on the memory in this work group.
	const uint local_size = get_local_size(0);
	const uint workgroup_id = global_id / local_size;
	uint current_size = local_size;
	if((workgroup_id + 1) * local_size > total_vertices) {
		if(workgroup_id * local_size < total_vertices) {
			current_size = total_vertices % local_size;
		} else {
			current_size = 0;
		}
	}
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
		if(current_size > 1) { //Last iteration of the aggregate sum.
			sums[local_id] += sums[local_id + 1];
		} else if(current_size == 0) { //Edge case for work groups whose work was indivisible such that they got 0 work.
			sums[local_id] = 0;
		}
		output_windings[workgroup_id] = sums[local_id];
	}
}

)kernel"