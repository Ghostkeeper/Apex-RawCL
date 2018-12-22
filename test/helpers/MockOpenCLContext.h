/*
 * Library for performing massively parallel computations on polygons.
 * Copyright (C) 2018 Ghostkeeper
 * This library is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for details.
 * You should have received a copy of the GNU Affero General Public License along with this library. If not, see <https://gnu.org/licenses/>.
 */

#ifndef MOCKOPENCLCONTEXT_H
#define MOCKOPENCLCONTEXT_H

#include <unordered_map> //Storing the contexts and queues per device.
#include "MockCommandQueue.h"
#include "MockContext.h"
#include "MockDevice.h"

namespace apex {

/*
 * Mocks the OpenCLContext class, in order to prevent making the tests depend on
 * the current OpenCL context.
 *
 * This mock only works with ``MockDevice``, ``MockContext`` and
 * ``MockCommandQueue``. This is done in order to prevent this class from being
 * used in live code, and to prevent real devices from being used in unit tests.
 * The mock is not useful for live code since it doesn't detect devices. Real
 * devices should not be used in unit tests since the outcome of the test will
 * then depend on the devices that are available on the test runner.
 *
 * This class is not to be used for algorithm tests. Those tests should be run
 * on actual devices since you'll want to test how the algorithm works on OpenCL
 * then. If you use this class, you will not use OpenCL.
 */
class MockOpenCLContext {
public:
	/*
	 * The getInstance() method is actually implemented and gets a singleton
	 * instance in order to make this mock behave the same as the real deal.
	 */
	static MockOpenCLContext& getInstance();

	/*
	 * Adds a device that can be used in a test.
	 *
	 * This creates a context and command queue for the device as well.
	 */
	void addTestDevice(MockDevice& device);

	std::unordered_map<MockDevice, MockContext> contexts;
	std::unordered_map<MockDevice, MockCommandQueue> queues;
};

}

#endif //MOCKOPENCLCONTEXT_H