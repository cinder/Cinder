//  ---------------------------------------------------------------------------
//
//  @file       AntPerfTimer.h
//  @brief      A performance (precision) timer for benchs
//  @author     Philippe Decaudin
//  @license    This file is part of the AntTweakBar library.
//              For conditions of distribution and use, see License.txt
//
//  note:       No cpp file is needed, everything is defined in this header
//
//  ---------------------------------------------------------------------------

#if !defined ANT_PERF_TIMER_INCLUDED
#define ANT_PERF_TIMER_INCLUDED

#ifndef  __cplusplus
#   error This is a C++ header
#endif  // __cplusplus


#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

    #include <windows.h>
    #include <tchar.h>

#if defined( CINDER_WINRT )
	#include "cinder/WinRTUtils.h"
#endif

    struct PerfTimer
    {
        inline        PerfTimer()
		{
			if( !QueryPerformanceFrequency(&Freq) )
#if defined( CINDER_MSW )
				MessageBox(NULL, _T("Precision timer not supported"), _T("Problem"), MB_ICONEXCLAMATION);
#elif defined( CINDER_WINRT )
				cinder::winrt::WinRTMessageBox("Precision timer not supported", "OK");
#endif
			Reset();
		}
        inline void   Reset()       { QueryPerformanceCounter(&Start); }
        inline double GetTime()     { if( QueryPerformanceCounter(&End) ) return ((double)End.QuadPart - (double)Start.QuadPart)/((double)Freq.QuadPart); else return 0; }
    protected:
        LARGE_INTEGER Start, End, Freq;
    };

#else // !_WIN (-> LINUX)

    #include <sys/time.h>
    #include <unistd.h>

    struct PerfTimer
    {
        inline        PerfTimer()   { Reset(); }
        inline void   Reset()       { gettimeofday(&Start, &TZ); }
        inline double GetTime()     { gettimeofday(&End,&TZ); 
                                      double t1 = (double)Start.tv_sec + (double)Start.tv_usec/(1000*1000);
                                      double t2 = (double)End.tv_sec + (double)End.tv_usec/(1000*1000);
                                      return t2-t1; }
    protected:
        struct timeval Start, End;
        struct timezone TZ;
    };

#endif // _WIN


#endif // ANT_PERF_TIMER_INCLUDED
