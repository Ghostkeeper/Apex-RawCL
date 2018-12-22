/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef MOCKDEVICE_H
#define MOCKDEVICE_H

#include <cstddef> //For std::size_t, for the unique device ID.
#include <functional> //For adding specialisation to std::hash.

namespace apex {

/*
 * Mock for cl::Device.
 */
class MockDevice {
public:
	/*
	 * Constructor that assigns this mock device a unique ID so that it can be
	 * stored in a hash table.
	 */
	MockDevice();

	/*
	 * An identifier that is unique to this device, so that it can be
	 * disambiguated from other mock devices.
	 *
	 * Note that this device ID may get collisions with actual cl::Device
	 * instances.
	 */
	size_t device_id;

	/*
	 * Checks whether two mock device instances are the same device, by their
	 * device ID.
	 */
	bool operator ==(const MockDevice& other) const;

private:
	/*
	 * The device ID to assign to the mock device that is constructed next.
	 */
	static size_t next_device_id;
};

}

namespace std {

/*
 * Hashes MockDevices.
 *
 * Each ``MockDevice`` instance gets a unique ID by which it is hashed.
 */
template<> struct hash<apex::MockDevice> {
	size_t operator ()(const apex::MockDevice& device) const;
};

}

#endif //MOCKDEVICE_H