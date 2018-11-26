/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "BenchmarkData.h" //The class definition we're implementing.

namespace apex {
namespace benchmarks {

BenchmarkData& BenchmarkData::getInstance() {
	static BenchmarkData instance; //Constructs using the default constructor.
	return instance;
}

BenchmarkData::BenchmarkData() {
	#include "benchmarks/GeForceGTX560.h"
	#include "benchmarks/GeForceGTX660M.h"
	#include "benchmarks/IntelI72600K.h"
	#include "benchmarks/IntelI73610QM.h"
	#include "benchmarks/IntelIvyBridgeMGT2.h"
	#include "benchmarks/Predictor.h"
}

}
}
