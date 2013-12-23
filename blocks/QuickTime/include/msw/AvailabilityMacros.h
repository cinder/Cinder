/*
     File:       AvailabilityMacros.h
 
     Copyright:  (c) 2001-2005 by Apple Computer, Inc., all rights reserved.

     More Info:  See TechNote 2064

     Contains:   Autoconfiguration of AVAILABLE_ macros for Mac OS X

                 This header enables a developer to specify build time
                 constraints on what Mac OS X versions the resulting
                 application will be run.  There are two bounds a developer
                 can specify:
                 
                      MAC_OS_X_VERSION_MIN_REQUIRED
                      MAC_OS_X_VERSION_MAX_ALLOWED
                      
                The lower bound controls which calls to OS functions will 
                be weak-importing (allowed to be unresolved at launch time).
                The upper bound controls which OS functionality, if used,
                will result in a compiler error because that functionality is
                not available on on any OS is the specifed range.
                
                For example, suppose an application is compiled with:
                
                      MAC_OS_X_VERSION_MIN_REQUIRED = MAC_OS_X_VERSION_10_2
                      MAC_OS_X_VERSION_MAX_ALLOWED  = MAC_OS_X_VERSION_10_3
                     
                and an OS header contains:
                
                     extern void funcA(void) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER;
                     extern void funcB(void) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2;
                     extern void funcC(void) AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3;
                     extern void funcD(void) AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER;
                     extern void funcE(void) AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER;
                     extern void funcF(void) AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;
                     extern void funcG(void) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
                     
                     typedef long TypeA DEPRECATED_IN_MAC_OS_X_VERSION_10_0_AND_LATER;
                     typedef long TypeB DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER;
                     typedef long TypeC DEPRECATED_IN_MAC_OS_X_VERSION_10_2_AND_LATER;
                     typedef long TypeD DEPRECATED_IN_MAC_OS_X_VERSION_10_3_AND_LATER;
                     typedef long TypeE DEPRECATED_IN_MAC_OS_X_VERSION_10_4_AND_LATER;

                Any application code which uses these declarations will get the following:
                
                                compile         link          run 
                                -------         ------        -------
                     funcA:     normal          normal        normal
                     funcB:     warning         normal        normal
                     funcC:     normal          normal        normal
                     funcD:     normal          normal        normal
                     funcE:     normal          normal        normal
                     funcF:     normal          weak          on 10.3 normal, on 10.2 (&funcF == NULL)
                     funcG:     error           error         n/a
                     typeA:     warning
                     typeB:     warning
                     typeC:     warning
                     typeD:     normal
                     typeE:     normal
                  
  
*/
#ifndef __AVAILABILITYMACROS__
#define __AVAILABILITYMACROS__


/*
 * Set up standard Mac OS X versions
 */
#define MAC_OS_X_VERSION_10_0 1000
#define MAC_OS_X_VERSION_10_1 1010
#define MAC_OS_X_VERSION_10_2 1020
#define MAC_OS_X_VERSION_10_3 1030
#define MAC_OS_X_VERSION_10_4 1040
#define MAC_OS_X_VERSION_10_5 1050


/* 
 * If min OS not specified, assume 10.1
 * Note: gcc driver may set _ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED_ based on MACOSX_DEPLOYMENT_TARGET environment variable
 */
#ifndef MAC_OS_X_VERSION_MIN_REQUIRED
    #ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
        #define MAC_OS_X_VERSION_MIN_REQUIRED __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
    #else
        #if __ppc64__ || __i386__ || __x86_64__
            #define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_4
        #else
            #define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_1
        #endif
    #endif
#endif

/*
 * if max OS not specified, assume largerof(10.5, min)
 */
#ifndef MAC_OS_X_VERSION_MAX_ALLOWED
    #if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_5
        #define MAC_OS_X_VERSION_MAX_ALLOWED MAC_OS_X_VERSION_MIN_REQUIRED
    #else
        #define MAC_OS_X_VERSION_MAX_ALLOWED MAC_OS_X_VERSION_10_5
    #endif
#endif

/*
 * Error on bad values
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_MIN_REQUIRED
    #error MAC_OS_X_VERSION_MAX_ALLOWED must be >= MAC_OS_X_VERSION_MIN_REQUIRED
#endif
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_0
    #error MAC_OS_X_VERSION_MIN_REQUIRED must be >= MAC_OS_X_VERSION_10_0
#endif

/*
 * only certain compilers support __attribute__((weak_import))
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))) && (MAC_OS_X_VERSION_MIN_REQUIRED >= 1020)
    #define WEAK_IMPORT_ATTRIBUTE __attribute__((weak_import))
#elif defined(__MWERKS__) && (__MWERKS__ >= 0x3205) && (MAC_OS_X_VERSION_MIN_REQUIRED >= 1020)
    #define WEAK_IMPORT_ATTRIBUTE __attribute__((weak_import))
#else
    #define WEAK_IMPORT_ATTRIBUTE
#endif

/*
 * only certain compilers support __attribute__((deprecated))
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#else
    #define DEPRECATED_ATTRIBUTE
#endif

/*
 * only certain compilers support __attribute__((unavailable))
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define UNAVAILABLE_ATTRIBUTE __attribute__((unavailable))
#else
    #define UNAVAILABLE_ATTRIBUTE
#endif



/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
 * 
 * Used on functions introduced in Mac OS X 10.0 
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED
 * 
 * Used on functions introduced in Mac OS X 10.0, 
 * and deprecated in Mac OS X 10.0
 */
#define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED    DEPRECATED_ATTRIBUTE

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_0_AND_LATER
 * 
 * Used on types deprecated in Mac OS X 10.0 
 */
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_0_AND_LATER     DEPRECATED_ATTRIBUTE






/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.1 
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_1
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER     UNAVAILABLE_ATTRIBUTE
#elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_1
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER     WEAK_IMPORT_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED
 * 
 * Used on declarations introduced in Mac OS X 10.1, 
 * and deprecated in Mac OS X 10.1
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_1
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED    AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_1
 * 
 * Used on declarations introduced in Mac OS X 10.0, 
 * but later deprecated in Mac OS X 10.1
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_1
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_1    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_1    AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#endif

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER
 * 
 * Used on types deprecated in Mac OS X 10.1 
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_1
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER    DEPRECATED_ATTRIBUTE
#else
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER
#endif







/*
 * AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.2 
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_2
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER     UNAVAILABLE_ATTRIBUTE
#elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_2
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER     WEAK_IMPORT_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED
 * 
 * Used on declarations introduced in Mac OS X 10.2, 
 * and deprecated in Mac OS X 10.2
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED    AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2
 * 
 * Used on declarations introduced in Mac OS X 10.0, 
 * but later deprecated in Mac OS X 10.2
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2    AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2
 * 
 * Used on declarations introduced in Mac OS X 10.1, 
 * but later deprecated in Mac OS X 10.2
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_2    AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_2_AND_LATER
 * 
 * Used on types deprecated in Mac OS X 10.2 
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_2_AND_LATER    DEPRECATED_ATTRIBUTE
#else
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_2_AND_LATER
#endif





/*
 * AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.3 
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_3
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER     UNAVAILABLE_ATTRIBUTE
#elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER     WEAK_IMPORT_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED
 * 
 * Used on declarations introduced in Mac OS X 10.3, 
 * and deprecated in Mac OS X 10.3
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED    AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3
 * 
 * Used on declarations introduced in Mac OS X 10.0, 
 * but later deprecated in Mac OS X 10.3
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3    AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3
 * 
 * Used on declarations introduced in Mac OS X 10.1, 
 * but later deprecated in Mac OS X 10.3
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3    AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3
 * 
 * Used on declarations introduced in Mac OS X 10.2, 
 * but later deprecated in Mac OS X 10.3
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_3    AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
#endif

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_3_AND_LATER
 * 
 * Used on types deprecated in Mac OS X 10.3 
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_3_AND_LATER    DEPRECATED_ATTRIBUTE
#else
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_3_AND_LATER
#endif






/*
 * AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.4 
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER     UNAVAILABLE_ATTRIBUTE
#elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER     WEAK_IMPORT_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED
 * 
 * Used on declarations introduced in Mac OS X 10.4, 
 * and deprecated in Mac OS X 10.4
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED    AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4
 * 
 * Used on declarations introduced in Mac OS X 10.0, 
 * but later deprecated in Mac OS X 10.4
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4
 * 
 * Used on declarations introduced in Mac OS X 10.1, 
 * but later deprecated in Mac OS X 10.4
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4
 * 
 * Used on declarations introduced in Mac OS X 10.2, 
 * but later deprecated in Mac OS X 10.4
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4
 * 
 * Used on declarations introduced in Mac OS X 10.3, 
 * but later deprecated in Mac OS X 10.4
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_4    AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
#endif

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_4_AND_LATER
 * 
 * Used on types deprecated in Mac OS X 10.4 
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_4_AND_LATER    DEPRECATED_ATTRIBUTE
#else
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_4_AND_LATER
#endif





/*
 * AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
 * 
 * Used on declarations introduced in Mac OS X 10.5 
 */
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER     UNAVAILABLE_ATTRIBUTE
#elif MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER     WEAK_IMPORT_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER_BUT_DEPRECATED
 * 
 * Used on declarations introduced in Mac OS X 10.5, 
 * and deprecated in Mac OS X 10.5
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER_BUT_DEPRECATED    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER_BUT_DEPRECATED    AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
 * 
 * Used on declarations introduced in Mac OS X 10.0, 
 * but later deprecated in Mac OS X 10.5
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
 * 
 * Used on declarations introduced in Mac OS X 10.1, 
 * but later deprecated in Mac OS X 10.5
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
 * 
 * Used on declarations introduced in Mac OS X 10.2, 
 * but later deprecated in Mac OS X 10.5
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
 * 
 * Used on declarations introduced in Mac OS X 10.3, 
 * but later deprecated in Mac OS X 10.5
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
#endif

/*
 * AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
 * 
 * Used on declarations introduced in Mac OS X 10.4, 
 * but later deprecated in Mac OS X 10.5
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    DEPRECATED_ATTRIBUTE
#else
    #define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5    AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
#endif

/*
 * DEPRECATED_IN_MAC_OS_X_VERSION_10_5_AND_LATER
 * 
 * Used on types deprecated in Mac OS X 10.5 
 */
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_5_AND_LATER    DEPRECATED_ATTRIBUTE
#else
    #define DEPRECATED_IN_MAC_OS_X_VERSION_10_5_AND_LATER
#endif

#endif  /* __AVAILABILITYMACROS__ */


