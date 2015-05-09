Cinder for WinRT currently requires Boost 1.53.0. If you want to use a higher version, you will need to make the following changes until Boost properly supports WinRT.

Specify the version of Boost as a C++ Preprocessor definition

WINRT_BOOST_VERSION=105300

In order to use Boost::Signals2 on WinRT platforms, you must replace the use of InitializeCriticalSection with InitializeCriticalSectionEx in the following two files:

cinder\boost\boost\signals2\detail\lwm_win32_cs.hpp
cinder\boost\boost\smart_ptr\detail\lwm_win32_cs.hpp
cinder\boost\boost\detail\endian.hpp

We have copied these files into:

cinder\include\winrt\boost\boost\signals2\detail\lwm_win32_cs.hpp
cinder\include\winrt\boost\boost\smart_ptr\detail\lwm_win32_cs.hpp
cinder\include\winrt\boost\boost\detail\endian.hpp

The followings changes were made to the copied files:

extern "C" __declspec(dllimport) void __stdcall InitializeCriticalSection(critical_section *);
extern "C" __declspec(dllimport) void __stdcall InitializeCriticalSectionEx(critical_section *, unsigned long spinCount, unsigned long flags);

public:

    mutex()
    {
        InitializeCriticalSectionEx(&cs_, 0, 0 );
    }

Modify your C++ Additional Include Directories paths so ../

..\include\winrt\boost;

appears before

..\boost;

Changes made to cinder\boost\boost\detail\endian.hpp

#elif defined(__i386__) || defined(__alpha__) \
   || defined(__ia64) || defined(__ia64__) \
   || defined(_M_IX86) || defined(_M_IA64) \
   || defined(_M_ALPHA) || defined(__amd64) \
   || defined(__amd64__) || defined(_M_AMD64) \
   || defined(__x86_64) || defined(__x86_64__) \
   || defined(_M_X64) || defined(__bfin__) \
   || defined(_M_ARM)

_M_ARM was added to check for ARM compiler in Visual Studio