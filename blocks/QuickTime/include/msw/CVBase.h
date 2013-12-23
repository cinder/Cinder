/*
 *  CVBase.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVBase.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion Here you can find the type declarations for CoreVideo. CoreVideo uses a CVTimeStamp structure to store video display time stamps.
*/

 
#if !defined(__COREVIDEO_CVBASE_H__)
#define __COREVIDEO_CVBASE_H__ 1

#include <TargetConditionals.h>
#include <AvailabilityMacros.h>

#if TARGET_OS_MAC
#include <CoreFoundation/CFBase.h>
#else
#include <MacTypes.h>
#include <CFBase.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define CV_EXPORT CF_EXPORT 
#define CV_INLINE CF_INLINE

#if TARGET_OS_WIN32
#define CVDIRECT3DDEVICE LPDIRECT3DDEVICE9
#define CVDIRECT3DTEXTURE LPDIRECT3DTEXTURE9
#define CVDIRECT3DSURFACE LPDIRECT3DSURFACE9
#define CVDIRECT3D LPDIRECT3D9
#endif //TARGET_OS_WIN32

/*!
    @typedef	CVOptionFlags
    @abstract   Flags to be used for the display and render call back functions.
    @discussion ***Values to be defined***
*/
typedef uint64_t CVOptionFlags;

/*!
    @struct         CVSMPTETime
    @abstract       A structure for holding a SMPTE time.
    @field          subframes
                        The number of subframes in the full message.
    @field          subframeDivisor
                        The number of subframes per frame (typically 80).
    @field          counter
                        The total number of messages received.
    @field          type
                        The kind of SMPTE time using the SMPTE time type constants.
    @field          flags
                        A set of flags that indicate the SMPTE state.
    @field          hours
                        The number of hourse in the full message.
    @field          minutes
                        The number of minutes in the full message.
    @field          seconds
                        The number of seconds in the full message.
    @field          frames
                        The number of frames in the full message.
*/
struct CVSMPTETime
{
    SInt16  subframes;
    SInt16  subframeDivisor;
    UInt32  counter;
    UInt32  type;
    UInt32  flags;
    SInt16  hours;
    SInt16  minutes;
    SInt16  seconds;
    SInt16  frames;
};
typedef struct CVSMPTETime    CVSMPTETime;

/*!
    @enum           SMPTE Time Types
    @abstract       Constants that describe the type of SMPTE time.
    @constant       kCVSMPTETimeType24
                        24 Frame
    @constant       kCVSMPTETimeType25
                        25 Frame
    @constant       kCVSMPTETimeType30Drop
                        30 Drop Frame
    @constant       kCVSMPTETimeType30
                        30 Frame
    @constant       kCVSMPTETimeType2997
                        29.97 Frame
    @constant       kCVSMPTETimeType2997Drop
                        29.97 Drop Frame
    @constant       kCVSMPTETimeType60
                        60 Frame
    @constant       kCVSMPTETimeType5994
                        59.94 Frame
*/
enum
{
    kCVSMPTETimeType24        = 0,
    kCVSMPTETimeType25        = 1,
    kCVSMPTETimeType30Drop    = 2,
    kCVSMPTETimeType30        = 3,
    kCVSMPTETimeType2997      = 4,
    kCVSMPTETimeType2997Drop  = 5,
    kCVSMPTETimeType60        = 6,
    kCVSMPTETimeType5994      = 7
};

/*!
    @enum           SMPTE State Flags
    @abstract       Flags that describe the SMPTE time state.
    @constant       kCVSMPTETimeValid
                        The full time is valid.
    @constant       kCVSMPTETimeRunning
                        Time is running.
*/
enum
{
    kCVSMPTETimeValid     = (1L << 0),
    kCVSMPTETimeRunning   = (1L << 1)
};


enum {
    kCVTimeIsIndefinite = 1 << 0
};

typedef struct
{
    int64_t	    timeValue;
    int32_t	    timeScale;
    int32_t	    flags;
} CVTime;

/*!
    @struct CVTimeStamp
    @abstract CoreVideo uses a CVTimeStamp structure to store video display time stamps.
    @discussion This structure is purposely very similar to AudioTimeStamp defined in the CoreAudio framework. 
		Most of the CVTimeStamp struct should be fairly self-explanatory. However, it is probably worth pointing out that unlike the audio time stamps, floats are not used to represent the video equivalent of sample times. This was done partly to avoid precision issues, and partly because QuickTime still uses integers for time values and time scales. In the actual implementation it has turned out to be very convenient to use integers, and we can represent framerates like NTSC (30000/1001 fps) exactly. The mHostTime structure field uses the same Mach absolute time base that is used in CoreAudio, so that clients of the CoreVideo API can synchronize between the two subsystems.
    @field videoTimeScale The scale (in units per second) of the videoTime and videoPeriod values
    @field videoTime This represents the start of a frame (or field for interlaced)
    @field hostTime Host root timebase time
    @field rateScalar This is the current rate of the device as measured by the timestamps, divided by the nominal rate
    @field videoPeriod This is the nominal update period of the current output device
    @field smpteTime SMPTE time representation of the time stamp. 
    @field flags Possible values are:		
		kCVTimeStampVideoTimeValid
		kCVTimeStampHostTimeValid
		kCVTimeStampSMPTETimeValid
		kCVTimeStampVideoPeriodValid
		kCVTimeStampRateScalarValid
		There are flags for each field to make it easier to detect interlaced vs progressive output
		kCVTimeStampTopField
		kCVTimeStampBottomField
		Some commonly used combinations of timestamp flags
		kCVTimeStampVideoHostTimeValid
		kCVTimeStampIsInterlaced
    @field version The current CVTimeStamp is version 0.
    @field reserved Reserved. Do not use.

*/
typedef struct
{
    uint32_t			version;		// Currently will be 0.
    int32_t			videoTimeScale;     	// Video timescale (units per second)
    int64_t			videoTime;		// This represents the start of a frame (or field for interlaced) .. think vsync  - still not 100% sure on the name
    uint64_t			hostTime;		// Host root timebase time
    double			rateScalar;		// Current rate as measured by the timestamps divided by the nominal rate
    int64_t			videoRefreshPeriod;    	// Hint for nominal output rate
    CVSMPTETime			smpteTime;
    uint64_t			flags;
    uint64_t			reserved;
} CVTimeStamp; 

// Flags for the CVTimeStamp structure
enum
{
    kCVTimeStampVideoTimeValid              = (1L << 0),
    kCVTimeStampHostTimeValid               = (1L << 1),
    kCVTimeStampSMPTETimeValid              = (1L << 2),
    kCVTimeStampVideoRefreshPeriodValid     = (1L << 3),
    kCVTimeStampRateScalarValid             = (1L << 4),
    
    // There are flags for each field to make it easier to detect interlaced vs progressive output
    kCVTimeStampTopField                    = (1L << 16),
    kCVTimeStampBottomField                 = (1L << 17)
};

//	Some commonly used combinations of timestamp flags
enum
{
    kCVTimeStampVideoHostTimeValid   = (kCVTimeStampVideoTimeValid | kCVTimeStampHostTimeValid),
    kCVTimeStampIsInterlaced	     = (kCVTimeStampTopField | kCVTimeStampBottomField)
};

CV_EXPORT const CVTime kCVZeroTime;
CV_EXPORT const CVTime kCVIndefiniteTime;

#if defined(__cplusplus)
}
#endif

#endif
