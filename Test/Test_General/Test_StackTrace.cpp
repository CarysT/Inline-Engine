#include "Test.hpp"

#include "BaseLibrary/StackTrace.hpp"

#include <iostream>


using namespace std;


//------------------------------------------------------------------------------
// Test class
//------------------------------------------------------------------------------


class TestStackTrace : public AutoRegisterTest<TestStackTrace> {
public:
	TestStackTrace() {}

	static std::string Name() {
		return "Stack Trace";
	}
	int Run() override;

private:
	static int a;
};


//------------------------------------------------------------------------------
// Test definition
//------------------------------------------------------------------------------


int TestStackTrace::Run() {
	auto frames = GetStackTrace();

	for (auto frame : frames) {
		cout << frame << endl;
	}


	return 0;
}