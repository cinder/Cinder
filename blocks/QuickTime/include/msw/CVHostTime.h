/*
 *  CVHostTime.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVHostTime.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion Utility functions for retrieving and working with the host time.
*/

#if !defined(__COREVIDEO_CVHOSTTIME_H__)
#define __COREVIDEO_CVHOSTTIME_H__ 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <QuartzCore/CVBase.h>
#else
#include <CVBase.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*!
    @function   CVGetCurrentHostTime
    @abstract   Retrieve the current value of the host time base.
    @discussion On Mac OS X, the host time base for CoreVideo and CoreAudio are identical, and the values returned from either API
                may be used interchangeably.
    @result     The current host time.
*/
extern uint64_t CVGetCurrentHostTime() AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVGetHostClockFrequency
    @abstract   Retrieve the frequency of the host time base.
    @discussion On Mac OS X, the host time base for CoreVideo and CoreAudio are identical, and the values returned from either API
                may be used interchangeably.
    @result     The current host frequency.
*/
extern double   CVGetHostClockFrequency() AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVGetHostClockMinimumTimeDelta
    @abstract   Retrieve the smallest possible increment in the host time base.
    @result     The smallest valid increment in the host time base.
*/
extern uint32_t CVGetHostClockMinimumTimeDelta() AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#if defined(__cplusplus)
}
#endif

#endif
