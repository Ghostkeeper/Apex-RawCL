/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef TESTPOLYGONTRANSLATE
#define TESTPOLYGONTRANSLATE

#include <gtest/gtest.h>
#include "Polygon.h"

namespace parallelogram {

/*
 * Tests moving a simple square in a positive direction.
 */
TEST(TestPolygonTranslate, SquareMovePositive) {
	Polygon square_1000;
	square_1000.emplace_back(); //Contains one simple polygon.
	square_1000[0].emplace_back(0, 0);
	square_1000[0].emplace_back(1000, 0);
	square_1000[0].emplace_back(1000, 1000);
	square_1000[0].emplace_back(0, 1000);

	square_1000.translate(10, 25);

	EXPECT_EQ(10, square_1000[0][0].x);
	EXPECT_EQ(25, square_1000[0][0].y);
	EXPECT_EQ(1010, square_1000[0][1].x);
	EXPECT_EQ(25, square_1000[0][1].y);
	EXPECT_EQ(1010, square_1000[0][2].x);
	EXPECT_EQ(1025, square_1000[0][2].y);
	EXPECT_EQ(10, square_1000[0][3].x);
	EXPECT_EQ(1025, square_1000[0][3].y);
}

}

#endif //TESTPOLYGONTRANSLATE