#include "cinder/app/AppBasic.h"
#include "cinder/System.h"
#include "cinder/qtime/QuickTime.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SystemTestApp : public AppBasic {
 public:
	void	setup();
};

void SystemTestApp::setup()
{
	console() << "System information:" << std::endl;
#if ! defined( CINDER_MSW )
	console() << " OS Version " << System::getOsMajorVersion() << "." << System::getOsMinorVersion() << "." << System::getOsBugFixVersion() << std::endl;
#else
	console() << " OS Version " << System::getOsMajorVersion() << "." << System::getOsMinorVersion() << " Service Pack " << System::getOsBugFixVersion() << std::endl;
#endif
	console() << " has SSE2:" << System::hasSse2() << std::endl;
	console() << " has SSE3:" << System::hasSse3() << std::endl;
	console() << " has SSE4.1:" << System::hasSse4_1() << std::endl;
	console() << " has SSE4.2:" << System::hasSse4_2() << std::endl;
	console() << " has 64 bit:" << System::hasX86_64() << std::endl;
	console() << " CPUs:" << System::getNumCpus() << std::endl;
	console() << " Cores:" << System::getNumCores() << std::endl;
	console() << " QuickTime version: " << std::hex << qtime::getQuickTimeVersion() << std::dec << " (" << qtime::getQuickTimeVersionString() << ")" << std::endl;	
	
	console() << " IP Addresses: " << std::endl;
	vector<string> ipAddresses = System::getIpAddresses();
	for( vector<string>::const_iterator addrIt = ipAddresses.begin(); addrIt != ipAddresses.end(); ++addrIt )
		console() << "   " << *addrIt << std::endl;

	console() << " Hardware Addresses: " << std::endl;
	vector<string> hwAddresses = System::getHardwareAddresses();
	for( vector<string>::const_iterator addrIt = hwAddresses.begin(); addrIt != hwAddresses.end(); ++addrIt )
		console() << "   " << *addrIt << std::endl;

}

CINDER_APP_BASIC( SystemTestApp, RendererGl )
