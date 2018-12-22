/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#include "MockDevice.h"

namespace apex {

size_t MockDevice::next_device_id = 0;

MockDevice::MockDevice() {
	device_id = next_device_id++;
}

bool MockDevice::operator ==(const MockDevice& other) const {
	return device_id == other.device_id;
}

}

namespace std {

size_t hash<apex::MockDevice>::operator ()(const apex::MockDevice& device) const {
	return device.device_id;
}

}