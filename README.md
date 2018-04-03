
[license]: # (Library for performing massively parallel computations on polygons.)
[license]: # (Copyright C 2018 Ghostkeeper)
[license]: # (This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or, at your option, any later version.)
[license]: # (This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.)
[license]: # (You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.)

Parallelogram
=============
The current state of this library can be considered to be completely defunct. I am basically experimenting here. You will find nothing of actual use here in your application, but it may serve as a crude example of how to make OpenCL work well on all platforms.

This is intended to be a library for polygon and polyhedron operations. It provides a bunch of geometrical algorithms that operate on finite-resolution shapes in 2D and 3D. These algorithms have multiple implementations. Some scale well, using more extensive data structures or highly parallel computing. Some don't scale well, but are more efficient on smaller data sets. The library chooses an implementation automatically based on predictions of how long an algorithm will take to compute its operation on the input data.

Goals for this library include, in order from important to unimportant:
- Teach me the ways of OpenCL.
- Become an alternative to [ClipperLib](http://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/_Body.htm) using more modern algorithms to achieve better performance.
- Teach others the ways of OpenCL in practice, to show how to make it work on a wide variety of platforms at the same time.
- Encourage open source development of slicers.