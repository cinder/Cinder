
///
/// @file Main test file compiles Catch unit test framework.
/// Write your own tests in separate source files and build
/// them and link them with the main project target.
///
/// These unit tests are useful for non-visual testing of Cinder.
///


#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#ifdef _MSC_VER
/**
* FIXME:
* Currently (2020-09-03), when compiled from cmake on windows,
* the text executable is compiled as a WIN32 (GUI) application
* instead of a simple console app.
* For that reason we need the WinMain entry point.
*
* However, as the the unit tests can also be built from a
* vc project file that does compile it as a console app.
* For that reason, we provide WinMain in addition and not
* instead of the regular main
*
* TODO: check if this is still necessary and/or fix the
* Unit test creation under cmake
*
*/

#include <windows.h> // Provide typedefs for WinMain declaration

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow) {

	return Catch::Session().run(0, &lpCmdLine);
}

#endif



