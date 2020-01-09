#include "Test.hpp"

#include <BaseLibrary/Memory/RingAllocationEngine.hpp>

#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using namespace std::string_literals;

static void TestAssertFunc(bool val, const char* expression) {
	if (!val) {
		throw std::runtime_error("Assertion failed while evaluating the following expression:\n"s + expression);
	}
}

#define TestAssert(x) TestAssertFunc(x, #x)


static void ExpectAllocationFail(inl::RingAllocationEngine& allocator) {
	try {
		allocator.Allocate();
		throw std::runtime_error("Expected allocation error!");
	}
	catch (std::bad_alloc&) {
	} // OK!
}


static int RandomAllocSize(size_t poolSize) {
	size_t divisor = poolSize / 4 - 1;
	assert(divisor > 0);
	return (rand() % divisor) + 1;
}


class Test_RingAllocatorEngine : public AutoRegisterTest<Test_RingAllocatorEngine> {
public:
	static std::string Name() {
		return "RingAllocatorEngine";
	}

	virtual int Run() override {
		try {
			{
				inl::RingAllocationEngine allocator(5);
				allocator.Allocate(1);
				try {
					allocator.Allocate(5);
					throw std::runtime_error("New allocation has overriden an existing allocation");
				}
				catch (const std::bad_alloc&) {
				}
			}
			{
				inl::RingAllocationEngine allocator(50);
				for (int i = 0; i < 2000; i++) {
					size_t pos = allocator.Allocate(1);
					allocator.Deallocate(pos);
				}
				allocator.Reset();
				for (int i = 0; i < 50; i++) {
					allocator.Allocate(1);
				}
				allocator.Reset();
				for (int i = 0; i < 2000; i++) {
					size_t pos1 = allocator.Allocate(1);
					size_t pos2 = allocator.Allocate(3);
					allocator.Deallocate(pos1);
					allocator.Deallocate(pos2);
				}
				allocator.Reset();
				for (int i = 0; i < 2000; i++) {
					size_t pos1 = allocator.Allocate(1);
					size_t pos2 = allocator.Allocate(1);
					allocator.Deallocate(pos1);
					allocator.Deallocate(pos2);
				}
			}

			constexpr int size = 53;
			inl::RingAllocationEngine allocator(size);

			TestAssert(allocator.Size() == size);


			{
				std::vector<size_t> allocations;
				for (int i = 0; i < size; i++) {
					allocations.push_back(allocator.Allocate());
				}

				ExpectAllocationFail(allocator);

				size_t prev = allocations.at(0);
				for (int i = 1; i < allocations.size(); i++) {
					size_t curr = allocations.at(i);
					TestAssert(curr == prev + 1);
					prev = curr;
				}
			}

			allocator.Reset();

			{
				size_t half = size / 2;
				size_t first = allocator.Allocate(half);
				size_t second = allocator.Allocate(size - half);

				ExpectAllocationFail(allocator);
				allocator.Deallocate(second);
				ExpectAllocationFail(allocator);
				allocator.Deallocate(first);
				allocator.Allocate(size);
			}

			allocator.Reset();

			// Random sized allocations
			{
				// Repeat it 10 times to maximize safety
				for (int currentTestRepeatIndex = 0; currentTestRepeatIndex < 10; currentTestRepeatIndex++) {
					std::vector<size_t> allocations;
					// offset the allocation from start
					{
						size_t offsetter = allocator.Allocate(size / 6);
						allocator.Deallocate(offsetter);
					}

					//allocate while there is no more room for a new allocation
					bool finished = false;
					while (!finished) {
						size_t newSize = RandomAllocSize(size);
						bool allocated = false;
						while (!allocated && !finished) {
							try {
								allocations.push_back(allocator.Allocate(newSize));
								allocated = true;
							}
							catch (std::bad_alloc&) {
								if (newSize == 1) {
									finished = true;
								}
								else {
									newSize -= 1;
								}
							}
						}
					}

					//remove all the allocations except for the first in a random order
					size_t firstPos = allocations.front();
					while (allocations.size() > 1) {
						const size_t min = 1;
						const size_t max = allocations.size() - 1;
						const auto diff = max - min;
						const size_t target = (rand() % (diff + 1)) + min;

						auto iter = allocations.begin() + target;
						allocator.Deallocate(*iter);
						allocations.erase(iter);
						ExpectAllocationFail(allocator);
					}

					allocator.Deallocate(allocations.front());
					allocator.Allocate(size);

					allocator.Reset();
				}
			}


			std::cout << "Test finished correctly" << std::endl;
		}
		catch (std::exception& ex) {
			std::cout << "Test failed with exception: " << ex.what() << std::endl;
			return 1;
		}
		catch (...) {
			std::cout << "Test failed with unknown exception" << std::endl;
			return 1;
		}

		return 0;
	}
};
