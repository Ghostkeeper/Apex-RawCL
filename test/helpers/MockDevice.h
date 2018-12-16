/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef MOCKDEVICE_H
#define MOCKDEVICE_H

namespace apex {

/*
 * Mock for cl::Device.
 *
 * The mock tracks for certain methods how often it is called and with what
 * parameters.
 */
class MockDevice {
	//This device is empty. It is only used as placeholder for functionality that gets mocked away as well.
};

}

#endif //MOCKDEVICE_H