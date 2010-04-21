#include "flint/app/AppBasic.h"
#include "flint/System.h"
#include "flint/qtime/QuickTime.h"
using namespace fli;

using namespace fli::app;

class SystemTestApp : public AppBasic {
 public:
	void	setup();
};

void SystemTestApp::setup()
{
	console() << "System information:" << std::endl;
#if defined( FLI_MAC )
	console() << " OS Version " << System::getOSMajorVersion() << "." << System::getOSMinorVersion() << "." << System::getOSBugFixVersion() << std::endl;
#else
	console() << " OS Version " << System::getOSMajorVersion() << "." << System::getOSMinorVersion() << " Service Pack " << System::getOSBugFixVersion() << std::endl;
#endif
	console() << " has SSE2:" << System::hasSSE2() << std::endl;
	console() << " has SSE3:" << System::hasSSE3() << std::endl;
	console() << " has SSE4.1:" << System::hasSSE4_1() << std::endl;
	console() << " has SSE4.2:" << System::hasSSE4_2() << std::endl;
	console() << " has 64 bit:" << System::hasX86_64() << std::endl;
	console() << " CPUs:" << System::getNumCPUs() << std::endl;
	console() << " Cores:" << System::getNumCores() << std::endl;
	console() << " QuickTime version: " << std::hex << qtime::getQuickTimeVersion() << std::dec << " (" << qtime::getQuickTimeVersionString() << ")" << std::endl;	
}

FLI_APP_BASIC( SystemTestApp, RendererGL )
