/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/System.h"

// Reference Intel's Application Note #485, "Intel Processor Identification and the CPUID Instruction" for constants
// Reference AMD's "Processor and Core Enumeration Using CPUID" for physical processor determination

#if defined( CINDER_COCOA )
	#if defined( CINDER_COCOA_TOUCH )
		#import <CFNetwork/CFNetwork.h>
		#import <UIKit/UIDevice.h>
	#elif defined( CINDER_MAC )
		#import <objc/message.h>
	#endif
	#import <Foundation/Foundation.h>
	#import <netinet/in.h>
	#import <netdb.h>
	#import <ifaddrs.h>
	#import <arpa/inet.h>
	#import <net/ethernet.h>
	#import <net/if_dl.h>
	#include <sys/sysctl.h>
#elif defined( CINDER_MSW_DESKTOP )
	#include <windows.h>
	#include <windowsx.h>
	#include <iphlpapi.h>
	#pragma comment(lib, "IPHLPAPI.lib")
	namespace cinder {
		void cpuidwrap( int *p, unsigned int param );
	}
#elif defined( CINDER_UWP )
	#include <collection.h>
	#include "cinder/winrt/WinRTUtils.h"
	using namespace Windows::Devices::Input;
	using namespace Windows::Foundation::Collections;
	using namespace Windows::ApplicationModel;
	using namespace Windows::Networking;
	using namespace Windows::Networking::Connectivity;
	using namespace cinder::winrt;
#elif defined( CINDER_LINUX )
	#include <ifaddrs.h>
	#include <netdb.h>
#endif

#if defined( __clang__ ) || defined( __GNUC__ )
	#include <cxxabi.h>
#endif

#include <string>

using namespace std;

namespace cinder {

std::shared_ptr<System> System::sInstance;

std::shared_ptr<System> System::instance()
{
	if( ! sInstance ) {
		sInstance = std::shared_ptr<System>( new System );
	}
	
	return sInstance;
}

System::System()
{
	for( size_t b = 0; b < TOTAL_CACHE_TYPES; ++b )
		mCachedValues[b] = false;
		
#if defined( CINDER_MSW_DESKTOP ) && ! defined( _WIN64 )
	int p[4];
	cpuidwrap( p, 1 );
	mCPUID_EBX = p[1];
	mCPUID_ECX = p[2];
	mCPUID_EDX = p[3];
#endif
}

#if defined( CINDER_COCOA )

template<typename T>  
static T getSysCtlValue( const std::string &key )
{
	size_t len = 0;
	// determine the length of the value and make sure it's sizeof(T)
	int error = sysctlbyname( key.c_str(), NULL, &len, NULL, 0 );
	if( error )
		return T(0);
	else if( sizeof(T) != len )
		throw SystemExcFailedQuery();
	
	T val;
	error = sysctlbyname( key.c_str(), &val, &len, NULL, 0 );
	if( error )
		throw SystemExcFailedQuery();
	return val;
}
#endif

#if defined( CINDER_MSW_DESKTOP ) && ! defined( _WIN64 )
typedef struct _LOGICALPROCESSORDATA
{
   unsigned int nLargestStandardFunctionNumber;
   unsigned int nLargestExtendedFunctionNumber;
   int nLogicalProcessorCount;
   int nLocalApicId;
   int nCPUcore;
   int nProcessorId;
   int nApicIdCoreIdSize;
   int nNC;
   int nMNC;
   int nCPUCoresperProcessor;
   int nThreadsperCPUCore;
   int nProcId; 
   int nCoreId;
   bool CmpLegacy;
   bool HTT;
}  LOGICALPROCESSORDATA, *PLOGICALPROCESSORDATA;

void lockToLogicalProcessor( int n )
{
	DWORD_PTR ProcessAffinityMask, SystemAffinityMask;
	BOOL rc;
	DWORD pm, pmm;
	rc = ::GetProcessAffinityMask( GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask );
	pm = SystemAffinityMask;
	pmm = 1;
	while( n ) {
	  pmm = pmm << 1;
	  n--;
	}
	ProcessAffinityMask = pmm;
	rc = ::SetProcessAffinityMask( GetCurrentProcess(), ProcessAffinityMask );
}

void cpuidwrap( int * p, unsigned int param )
{
   __asm {
             mov    edi, p
             mov    eax, param
             cpuid
             mov    [edi+0d],  eax
             mov    [edi+4d],  ebx
             mov    [edi+8d],  ecx
             mov    [edi+12d], edx
         }
}

void cpuid( int whichlp, PLOGICALPROCESSORDATA p )
{
   unsigned int i, j, mask, numbits;
   int CPUInfo[4] = {0,0,0,0};
   lockToLogicalProcessor( whichlp );

   cpuidwrap( CPUInfo, 0 );
   p->nLargestStandardFunctionNumber = CPUInfo[0];

   // Get the information associated with each valid Id
   for (i=0; i <= p->nLargestStandardFunctionNumber; ++i) {
      cpuidwrap( CPUInfo, i );
      // Interpret CPU feature information.
      if(i == 1) {
         // Some of the bits of LocalApicId represent the CPU core 
         // within a processor and other bits represent the processor ID. 
         p->nLocalApicId = (CPUInfo[1] >> 24) & 0xff;
         p->HTT = (CPUInfo[3] >> 28) & 0x1; 
         // recalculate later after 0x80000008
         p->nLogicalProcessorCount = (CPUInfo[1] >> 16) & 0x0FF; 
      }
   }

   // Calling __cpuid with 0x80000000 as the InfoType argument
   // gets the number of valid extended IDs.
   cpuidwrap( CPUInfo, 0x80000000 );
   p->nLargestExtendedFunctionNumber = CPUInfo[0];
 
   // Get the information associated with each extended ID.
   for (i=0x80000000; i<=p->nLargestExtendedFunctionNumber; ++i)
   {
      cpuidwrap( CPUInfo, i );
      if  (i == 0x80000008)
      {
         p->nApicIdCoreIdSize = (CPUInfo[2] >> 12) & 0xF;
         p->nNC = (CPUInfo[2]) & 0x0FF;
      }
   }
   // MNC
   // A value of zero for ApicIdCoreIdSize indicates that MNC is derived by this
   // legacy formula: MNC = NC + 1
   // A non-zero value of ApicIdCoreIdSize means that MNC is 2^ApicIdCoreIdSize  

   if (p->nApicIdCoreIdSize)
   {
      p->nMNC = 2;
      for (j = p->nApicIdCoreIdSize-1; j>0; j--)
         p->nMNC = p->nMNC * 2;
   }
   else
   {
      p->nMNC = p->nNC + 1;
   }
   // If HTT==0, then LogicalProcessorCount is reserved, and the CPU contains 
   // one CPU core and the CPU core is single-threaded.
   // If HTT==1 and CmpLegacy==1, LogicalProcessorCount represents the number of
   // CPU cores per processor, where each CPU core is single-threaded.  If HTT==1
   // and CmpLegacy==0, then LogicalProcessorCount is the number of threads per
   // processor, which is the number of cores times the number of threads per core.
   // The number of cores is NC+1.
    
   p->nCPUCoresperProcessor = p->nNC + 1;
   p->nThreadsperCPUCore = ( p->HTT==0 ? 1 :
                              ( p->CmpLegacy==1 ? 1 : 
                                p->nLogicalProcessorCount / p->nCPUCoresperProcessor 
                              )
                           ); 

   // Calculate a mask for the core IDs
   mask = 1;
   numbits = 1;
   if (p->nApicIdCoreIdSize)
   {
      numbits = p->nApicIdCoreIdSize;
      for (j = p->nApicIdCoreIdSize; j>1; j--)
         mask = (mask << 1) + 1;
   }
   p->nProcId = p->nLocalApicId & ~mask;
   p->nProcId = p->nProcId >> (numbits);
   p->nCoreId = p->nLocalApicId & mask;
}

#endif // defined( CINDER_MSW_DESKTOP ) && ! defined( _WIN64 )

bool System::hasSse2()
{
	if( ! instance()->mCachedValues[HAS_SSE2] ) {
#if defined( CINDER_COCOA )	
		instance()->mHasSSE2 = ( getSysCtlValue<int>( "hw.optional.sse2" ) == 1 );
#elif defined( _WIN64 )
		instance()->mHasSSE2 = true;
#elif defined( CINDER_MSW_DESKTOP )
		instance()->mHasSSE2 = ( instance()->mCPUID_EDX & 0x04000000 ) != 0;
#elif defined( CINDER_UWP )
		instance()->mHasSSE2 = IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) != 0;
#else
	throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[HAS_SSE2] = true;
	}
	
	return instance()->mHasSSE2;
}

bool System::hasSse3()
{
	if( ! instance()->mCachedValues[HAS_SSE3] ) {
#if defined( CINDER_COCOA )	
		instance()->mHasSSE3 = ( getSysCtlValue<int>( "hw.optional.sse3" ) == 1 );
#elif defined( _WIN64 )
		instance()->mHasSSE3 = true;
#elif defined( CINDER_MSW_DESKTOP )
		instance()->mHasSSE3 = ( instance()->mCPUID_ECX & 0x00000001 ) != 0;
#elif defined( CINDER_UWP )
		instance()->mHasSSE3 = IsProcessorFeaturePresent(PF_SSE3_INSTRUCTIONS_AVAILABLE) != 0;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[HAS_SSE3] = true;
	}
	
	return instance()->mHasSSE3;
}

bool System::hasSse4_1()
{
	if( ! instance()->mCachedValues[HAS_SSE4_1] ) {
#if defined( CINDER_COCOA )	
		instance()->mHasSSE4_1 = ( getSysCtlValue<int>( "hw.optional.sse4_1" ) == 1 );
#elif defined( _WIN64 )
		instance()->mHasSSE4_1 = true; // TODO: this is not being tested
#elif defined( CINDER_MSW_DESKTOP )
		instance()->mHasSSE4_1 = ( instance()->mCPUID_ECX & ( 1 << 19 ) ) != 0;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[HAS_SSE4_1] = true;
	}
	
	return instance()->mHasSSE4_1;
}

bool System::hasSse4_2()
{
	if( ! instance()->mCachedValues[HAS_SSE4_2] ) {
#if defined( CINDER_COCOA )	
		instance()->mHasSSE4_2 = ( getSysCtlValue<int>( "hw.optional.sse4_2" ) == 1 );
#elif defined( _WIN64 )
		instance()->mHasSSE4_2 = true; // TODO: this is not being tested
#elif defined( CINDER_MSW_DESKTOP )
		instance()->mHasSSE4_2 = ( instance()->mCPUID_ECX & ( 1 << 20 ) ) != 0;
#else
		throw Exception( "Not implemented" );
#endif		
		instance()->mCachedValues[HAS_SSE4_2] = true;
	}
	
	return instance()->mHasSSE4_2;
}

bool System::hasArm()
{
	if( ! instance()->mCachedValues[HAS_ARM] ) {
#if defined( CINDER_UWP )	
		SYSTEM_INFO info;
		::GetNativeSystemInfo(&info);
		instance()->mHasArm = info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM;
#elif defined( CINDER_COCOA_TOUCH )
		instance()->mHasArm = true;
#else
		instance()->mHasArm = false;
#endif
		instance()->mCachedValues[HAS_ARM] = true;
	}
	return instance()->mHasArm;
}

bool System::hasX86_64()
{
	if( ! instance()->mCachedValues[HAS_X86_64] ) {
#if defined( CINDER_COCOA )	
		instance()->mHasX86_64 = ( getSysCtlValue<int>( "hw.optional.x86_64" ) == 1 );
#elif defined( _WIN64 )
		instance()->mHasX86_64 = true;
#elif defined( CINDER_MSW_DESKTOP )
		instance()->mHasX86_64 = ( instance()->mCPUID_EDX & ( 1 << 29 ) ) != 0;
#elif defined( CINDER_UWP )
		SYSTEM_INFO info;
		::GetNativeSystemInfo(&info);
		instance()->mHasX86_64 = info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
#else
		throw Exception( "Not implemented" );
#endif		

		instance()->mCachedValues[HAS_X86_64] = true;
	}
	
	return instance()->mHasX86_64;
}

int System::getNumCpus()
{
	if( ! instance()->mCachedValues[PHYSICAL_CPUS] ) {
#if defined( CINDER_COCOA )	
		instance()->mPhysicalCPUs = getSysCtlValue<int>( "hw.packages" );
#elif defined( CINDER_UWP ) || defined( _WIN64 )
		SYSTEM_INFO info;
		::GetNativeSystemInfo(&info);
		instance()->mPhysicalCPUs = info.dwNumberOfProcessors;
#elif defined( CINDER_MSW_DESKTOP )
		const int MAX_NUMBER_OF_LOGICAL_PROCESSORS = 96;
		const int MAX_NUMBER_OF_PHYSICAL_PROCESSORS = 8;
		const int MAX_NUMBER_OF_IOAPICS = 16;
		int PhysProcIds[MAX_NUMBER_OF_PHYSICAL_PROCESSORS+MAX_NUMBER_OF_IOAPICS];
		LOGICALPROCESSORDATA LogicalProcessorMap[MAX_NUMBER_OF_LOGICAL_PROCESSORS]; 
		memset( (void *) &LogicalProcessorMap, 0, sizeof( LogicalProcessorMap ) );
		memset( (void *) &PhysProcIds, 0, sizeof( PhysProcIds ) );

		// save the process affinity mask so we can reset it
		DWORD_PTR processAffinityMask, systemAffinityMask;
		::GetProcessAffinityMask( GetCurrentProcess(), &processAffinityMask, &systemAffinityMask );

		// walk the cores and fill in our structures
		int numCores = getNumCores();
		for( int i = 0; i < numCores; i++ )
			cpuid( i, &LogicalProcessorMap[i] );
		// lock to one logical processor
		lockToLogicalProcessor( ( instance()->mCPUID_EBX >> 24 ) & 0xFF );
		for( int i = 0; i < numCores; i++ )
			PhysProcIds[LogicalProcessorMap[i].nProcId]++;
		instance()->mPhysicalCPUs = 0;
		for( int i = 0; i < (MAX_NUMBER_OF_PHYSICAL_PROCESSORS+MAX_NUMBER_OF_IOAPICS); i++ )
			if( PhysProcIds[i] ) 
				instance()->mPhysicalCPUs++;  
		
		// unlock from a particular logical processor
		::SetProcessAffinityMask( GetCurrentProcess(), processAffinityMask );
#else
		throw Exception( "Not implemented" );
#endif		

		instance()->mCachedValues[PHYSICAL_CPUS] = true;
	}
	
	return instance()->mPhysicalCPUs;
}

int System::getNumCores()
{
	if( ! instance()->mCachedValues[LOGICAL_CPUS] ) {
#if defined( CINDER_COCOA )	
		instance()->mLogicalCPUs = getSysCtlValue<int>( "hw.logicalcpu" );
#elif defined( CINDER_UWP ) || defined( _WIN64 )
		SYSTEM_INFO info;
		::GetNativeSystemInfo(&info);
		// no way to check the actual number of cores, so return dwNumberOfProcessors
		instance()->mLogicalCPUs = info.dwNumberOfProcessors;
#elif defined( CINDER_MSW_DESKTOP )
		::SYSTEM_INFO sys;
		::GetSystemInfo( &sys );
		instance()->mLogicalCPUs = sys.dwNumberOfProcessors;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[LOGICAL_CPUS] = true;
	}
	
	return instance()->mLogicalCPUs;
}

#if defined( CINDER_COCOA )
typedef struct {
	NSInteger majorVersion;
	NSInteger minorVersion;
	NSInteger patchVersion;
} ShadowOSVersion;
#endif

int System::getOsMajorVersion()
{
	if( ! instance()->mCachedValues[OS_MAJOR] ) {
#if defined( CINDER_COCOA_TOUCH )
		NSArray *sysVerComponents = [[[UIDevice currentDevice] systemVersion] componentsSeparatedByString:@"."];
		instance()->mOSMajorVersion = [[sysVerComponents firstObject] intValue];
#elif defined( CINDER_MAC )
		if( [[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)] ) {
			ShadowOSVersion version = ((ShadowOSVersion(*)(id, SEL))objc_msgSend_stret)([NSProcessInfo processInfo], @selector(operatingSystemVersion));
			instance()->mOSMajorVersion = (int32_t)version.majorVersion;
		} else {
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			if( Gestalt(gestaltSystemVersionMajor, reinterpret_cast<SInt32*>( &(instance()->mOSMajorVersion) ) ) != noErr )
				throw SystemExcFailedQuery();
	#pragma clang diagnostic pop
		}
#elif defined( CINDER_MSW_DESKTOP )
		::OSVERSIONINFOEX info;
		::ZeroMemory( &info, sizeof( OSVERSIONINFOEX ) );
		info.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
		::GetVersionEx( (OSVERSIONINFO *)&info );
		instance()->mOSMajorVersion = info.dwMajorVersion;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[OS_MAJOR] = true;
	}
	
	return instance()->mOSMajorVersion;
}

int System::getOsMinorVersion()
{
	if( ! instance()->mCachedValues[OS_MINOR] ) {
#if defined( CINDER_COCOA_TOUCH )
		NSArray *sysVerComponents = [[[UIDevice currentDevice] systemVersion] componentsSeparatedByString:@"."];
		instance()->mOSMinorVersion = [[sysVerComponents objectAtIndex:1] intValue];
#elif defined( CINDER_MAC )
		if( [[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)] ) {
			ShadowOSVersion version = ((ShadowOSVersion(*)(id, SEL))objc_msgSend_stret)([NSProcessInfo processInfo], @selector(operatingSystemVersion));
			instance()->mOSMinorVersion = (int32_t)version.minorVersion;
		} else {
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			if( Gestalt(gestaltSystemVersionMinor, reinterpret_cast<SInt32*>( &(instance()->mOSMinorVersion) ) ) != noErr )
				throw SystemExcFailedQuery();
	#pragma clang diagnostic pop
		}
#elif defined( CINDER_MSW_DESKTOP )
		::OSVERSIONINFOEX info;
		::ZeroMemory( &info, sizeof( OSVERSIONINFOEX ) );
		info.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
		::GetVersionEx( reinterpret_cast<LPOSVERSIONINFO>( &info ) );
		instance()->mOSMinorVersion = info.dwMinorVersion;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[OS_MINOR] = true;
	}
	
	return instance()->mOSMinorVersion;
}

int System::getOsBugFixVersion()
{
	if( ! instance()->mCachedValues[OS_BUGFIX] ) {
#if defined( CINDER_COCOA_TOUCH )
		NSArray *sysVerComponents = [[[UIDevice currentDevice] systemVersion] componentsSeparatedByString:@"."];
		if( [sysVerComponents count] > 2 )
			instance()->mOSBugFixVersion = [[sysVerComponents objectAtIndex:2] intValue];
		else
			instance()->mOSBugFixVersion = 0;
#elif defined( CINDER_MAC )
		if( [[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)] ) {
			ShadowOSVersion version = ((ShadowOSVersion(*)(id, SEL))objc_msgSend_stret)([NSProcessInfo processInfo], @selector(operatingSystemVersion));
			instance()->mOSBugFixVersion = (int32_t)version.patchVersion;
		} else {
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			if( Gestalt(gestaltSystemVersionBugFix, reinterpret_cast<SInt32*>( &(instance()->mOSBugFixVersion) ) ) != noErr )
				throw SystemExcFailedQuery();
	#pragma clang diagnostic pop
		}
#elif defined( CINDER_MSW_DESKTOP )
		::OSVERSIONINFOEX info;
		::ZeroMemory( &info, sizeof( OSVERSIONINFOEX ) );
		info.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
		::GetVersionEx( reinterpret_cast<LPOSVERSIONINFO>( &info ) );
		instance()->mOSBugFixVersion = info.wServicePackMajor;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[OS_BUGFIX] = true;
	}
	
	return instance()->mOSBugFixVersion;
}

bool System::hasMultiTouch()
{
	if( ! instance()->mCachedValues[MULTI_TOUCH] ) {
#if defined( CINDER_MAC ) // Mac OS X doesn't really support touch yet (well, we don't yet)	
	#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
		instance()->mHasMultiTouch = getOsMajorVersion() > 10 || getOsMinorVersion() >= 6;
	#else
		return false;
	#endif
#elif defined( CINDER_COCOA_TOUCH ) // all incarnations of the iPhone OS support multiTouch
		instance()->mHasMultiTouch = true;
#elif defined( CINDER_MSW_DESKTOP )
		int value = ::GetSystemMetrics( 94/*SM_DIGITIZER*/ );
		instance()->mHasMultiTouch = (value & 0x00000080/*NID_READY*/ ) && 
				( (value & 0x00000040/*NID_MULTI_INPUT*/ ) || (value & 0x00000001/*NID_INTEGRATED_TOUCH*/ ) );
#elif defined( CINDER_UWP )
		auto pointerDevices = PointerDevice::GetPointerDevices();
		std::for_each(begin(pointerDevices), end(pointerDevices), [&](PointerDevice^ p) {
			if(p->PointerDeviceType == PointerDeviceType::Touch) {
				instance()->mHasMultiTouch  = true;
			}
		});
#elif defined( CINDER_ANDROID )
		// @TODO Check to make sure all variants of Android device support multiTouch 
		instance()->mHasMultiTouch = true;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[MULTI_TOUCH] = true;
	}
	
	return instance()->mHasMultiTouch;
}

int32_t System::getMaxMultiTouchPoints()
{
	if( ! instance()->mCachedValues[MAX_MULTI_TOUCH_POINTS] ) {
#if defined( CINDER_MAC ) // We don't have a good way of determining this yet
		instance()->mMaxMultiTouchPoints = 10;
#elif defined( CINDER_COCOA_TOUCH ) // all incarnations of the iPhone OS support multiTouch
		instance()->mMaxMultiTouchPoints = 6; // we don't seem to be able to query this at runtime; should be hardcoded based on the device
#elif defined( CINDER_MSW_DESKTOP )
		instance()->mMaxMultiTouchPoints = ::GetSystemMetrics( 95/*SM_MAXIMUMTOUCHES*/ );
#elif defined( CINDER_UWP )
		auto pointerDevices = PointerDevice::GetPointerDevices();
		unsigned int maxContacts = 0;
		std::for_each(begin(pointerDevices), end(pointerDevices), [&](PointerDevice^ p) {
			if(p->MaxContacts > maxContacts) {
				maxContacts  = p->MaxContacts;
			}
		});
		instance()->mMaxMultiTouchPoints = maxContacts;

#elif defined( CINDER_ANDROID )
		instance()->mMaxMultiTouchPoints = 10;
#else
		throw Exception( "Not implemented" );
#endif
		instance()->mCachedValues[MAX_MULTI_TOUCH_POINTS] = true;
	}
	
	return instance()->mMaxMultiTouchPoints;
}

string System::demangleTypeName( const char *mangledName )
{
	// not supported on MSW or UWP
#if defined( CINDER_MSW )
	return mangledName;
#else
	int status = 0;
	std::unique_ptr<char, void(*)(void *)> result { abi::__cxa_demangle( mangledName, NULL, NULL, &status ), free };

	return ( status == 0 ) ? result.get() : mangledName;
#endif
}

vector<System::NetworkAdapter> System::getNetworkAdapters()
{
	vector<System::NetworkAdapter> adapters;

#if defined( CINDER_COCOA ) || defined( CINDER_LINUX )

	struct ifaddrs *interfaces = nullptr;

	if( getifaddrs( &interfaces ) == 0 ) {
		for( auto currentInterface = interfaces; currentInterface; currentInterface = currentInterface->ifa_next ) {
			if( currentInterface->ifa_addr->sa_family == AF_INET ) {
				char host[NI_MAXHOST], subnetMask[NI_MAXHOST];
				int nameResult = getnameinfo( currentInterface->ifa_addr,
					(currentInterface->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
					host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
				int maskResult = getnameinfo( currentInterface->ifa_netmask,
					(currentInterface->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
					subnetMask, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );
				if( nameResult == 0 && maskResult == 0 ) {
					adapters.push_back( System::NetworkAdapter( currentInterface->ifa_name, host, subnetMask ) );
				}
			}
		}
	}
	freeifaddrs( interfaces );
#elif defined( CINDER_MSW_DESKTOP )
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)::HeapAlloc( ::GetProcessHeap(), 0, sizeof(IP_ADAPTER_INFO) );
    if( pAdapterInfo == NULL ) {
        return adapters;
    }
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
    if( ::GetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW) {
        ::HeapFree( ::GetProcessHeap(), 0, pAdapterInfo );
        pAdapterInfo = (IP_ADAPTER_INFO *) ::HeapAlloc( ::GetProcessHeap(), 0, ulOutBufLen );
        if( ! pAdapterInfo ) {
            return adapters;
        }
    }

    if( (dwRetVal = ::GetAdaptersInfo( pAdapterInfo, &ulOutBufLen )) == NO_ERROR ) {
        pAdapter = pAdapterInfo;
        while( pAdapter ) {
			adapters.push_back( System::NetworkAdapter( pAdapter->Description, pAdapter->IpAddressList.IpAddress.String, pAdapter->IpAddressList.IpMask.String ) );
            pAdapter = pAdapter->Next;
        }
    }

    if( pAdapterInfo )
        ::HeapFree( ::GetProcessHeap(), 0, pAdapterInfo );
#elif defined( CINDER_UWP )
	auto hosts = NetworkInformation::GetHostNames();
	std::for_each( begin(hosts), end(hosts), [&](HostName^ n) {
		std::string subnetMask;
		if( n->IPInformation && n->IPInformation->PrefixLength )
			subnetMask = PlatformStringToString( n->IPInformation->PrefixLength->ToString() );
		
		adapters.push_back( System::NetworkAdapter( PlatformStringToString(n->CanonicalName), PlatformStringToString(n->DisplayName), subnetMask ) );
	});
#else
		throw Exception( "Not implemented" );
#endif

	return adapters;

}

std::string System::getSubnetMask() {
	auto preferredIpAddress = System::getIpAddress();
	vector<System::NetworkAdapter> adapters = getNetworkAdapters();
	for (vector<System::NetworkAdapter>::const_iterator adaptIt = adapters.begin(); adaptIt != adapters.end(); ++adaptIt) {
		if (adaptIt->getIpAddress() == preferredIpAddress) {
			return adaptIt->getSubnetMask();
		}
	}
	return "0.0.0.0";
}

#if defined( CINDER_UWP )
std::string System::getIpAddress()
{
    auto icp = NetworkInformation::GetInternetConnectionProfile();
	std::string ip("");
	if (icp != nullptr && icp->NetworkAdapter != nullptr)
    {
		auto hosts = NetworkInformation::GetHostNames();
		std::for_each(begin(hosts), end(hosts), [&](HostName^ n) {
			if(n->IPInformation && n->IPInformation->NetworkAdapter)
			{
				if(n->IPInformation->NetworkAdapter->NetworkAdapterId == icp->NetworkAdapter->NetworkAdapterId) {
					ip = PlatformStringToString(n->DisplayName);
				}
				//OutputDebugString(std::wstring(n->DisplayName->Data()).c_str());
			}
		});
    }

    return ip;
}
#else
std::string System::getIpAddress()
{
	vector<System::NetworkAdapter> adapters = getNetworkAdapters();

#if defined( CINDER_COCOA )
	for( vector<System::NetworkAdapter>::const_iterator adaptIt = adapters.begin(); adaptIt != adapters.end(); ++adaptIt ) {
		if( adaptIt->getName() == "en0" )
			return adaptIt->getIpAddress();
	}
#endif

	std::string result = "127.0.0.1";	
	for( vector<System::NetworkAdapter>::const_iterator adaptIt = adapters.begin(); adaptIt != adapters.end(); ++adaptIt ) {
		if( (adaptIt->getIpAddress() != "127.0.0.1") && (adaptIt->getIpAddress() != "0.0.0.0") )
			result = adaptIt->getIpAddress();
	}
	
	return result;
}
#endif

#if defined( CINDER_COCOA_TOUCH )
bool System::isDeviceIphone()
{
	if( ! instance()->mCachedValues[IS_IPHONE] ) {
		instance()->mCachedValues[IS_IPHONE] = [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone;
	}
	
	return instance()->mCachedValues[IS_IPHONE];
}

bool System::isDeviceIpad()
{
	if( ! instance()->mCachedValues[IS_IPAD] ) {
		instance()->mCachedValues[IS_IPAD] = [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad;
	}
	
	return instance()->mCachedValues[IS_IPAD]; 
}
#endif

} // namespace cinder
