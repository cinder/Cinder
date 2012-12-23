/*==================================================================================================
     File:       CoreAudio/CoreAudioTypes.h

     Contains:   Definitions types common to the Core Audio APIs

     Version:    Technology: Mac OS X
                 Release:    Mac OS X

     Copyright:  (c) 1985-2007 by Apple Inc., all rights reserved.

     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:

                     http://developer.apple.com/bugreporter/

==================================================================================================*/
#if !defined(__CoreAudioTypes_h__)
#define __CoreAudioTypes_h__

/*!
    @header CoreAudioTypes
    This header defines the types and constants that all the CoreAudio APIs have in common.
*/

//==================================================================================================

#define COREAUDIOTYPES_VERSION 1050

#include <TargetConditionals.h>
#if !defined(__COREAUDIO_USE_FLAT_INCLUDES__)
    #include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
#else
    #include <MacTypes.h>
#endif

#if defined(__cplusplus)
    #include <string.h>
#endif

//==================================================================================================

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint off
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

//==================================================================================================

/*!
    @struct         AudioValueRange
    @abstract       This structure holds a pair of numbers that represent a continuous range of
                    values.
    @field          mMinimum
                        The minimum value.
    @field          mMaximum
                        The maximum value.
*/
struct AudioValueRange
{
    Float64 mMinimum;
    Float64 mMaximum;
};
typedef struct AudioValueRange  AudioValueRange;

/*!
    @struct         AudioValueTranslation
    @abstract       This stucture holds the buffers necessary for translation operations.
    @field          mInputData
                        The buffer containing the data to be translated.
    @field          mInputDataSize
                        The number of bytes in the buffer pointed at by mInputData.
    @field          mOutputData
                        The buffer to hold the result of the translation.
    @field          mOutputDataSize
                        The number of bytes in the buffer pointed at by mOutputData.
*/
struct AudioValueTranslation
{
    void*   mInputData;
    UInt32  mInputDataSize;
    void*   mOutputData;
    UInt32  mOutputDataSize;
};
typedef struct AudioValueTranslation    AudioValueTranslation;

/*!
    @struct         AudioBuffer
    @abstract       A structure to hold a buffer of audio data.
    @field          mNumberChannels
                        The number of interleaved channels in the buffer.
    @field          mDataByteSize
                        The number of bytes in the buffer pointed at by mData.
    @field          mData
                        A pointer to the buffer of audio data.
*/
struct AudioBuffer
{
    UInt32  mNumberChannels;
    UInt32  mDataByteSize;
    void*   mData;
};
typedef struct AudioBuffer  AudioBuffer;

/*!
    @struct         AudioBufferList
    @abstract       A variable length array of AudioBuffer structures.
    @field          mNumberBuffers
                        The number of AudioBuffers in the mBuffers array.
    @field          mBuffers
                        A variable length array of AudioBuffers.
*/
struct AudioBufferList
{
    UInt32      mNumberBuffers;
    AudioBuffer mBuffers[kVariableLengthArray];
};
typedef struct AudioBufferList  AudioBufferList;

/*!
	@typedef		AudioSampleType
	@abstract		The canonical audio sample type used by the various CoreAudio APIs
*/
typedef Float32		AudioSampleType;

/*!
    @struct         AudioStreamBasicDescription
    @abstract       This structure encapsulates all the information for describing the basic
                    format properties of a stream of audio data.
    @discussion     This structure is sufficient to describe any constant bit rate format that  has
                    channels that are the same size. Extensions are required for variable bit rate
                    data and for constant bit rate data where the channels have unequal sizes.
                    However, where applicable, the appropriate fields will be filled out correctly
                    for these kinds of formats (the extra data is provided via separate properties).
                    In all fields, a value of 0 indicates that the field is either unknown, not
                    applicable or otherwise is inapproprate for the format and should be ignored.
                    Note that 0 is still a valid value for most formats in the mFormatFlags field.

                    In audio data a frame is one sample across all channels. In non-interleaved
                    audio, the per frame fields identify one channel. In interleaved audio, the per
                    frame fields identify the set of n channels. In uncompressed audio, a Packet is
                    one frame, (mFramesPerPacket == 1). In compressed audio, a Packet is an
                    indivisible chunk of compressed data, for example an AAC packet will contain
                    1024 sample frames.
    @field          mSampleRate
                        The number of sample frames per second of the data in the stream.
    @field          mFormatID
                        A four char code indicating the general kind of data in the stream.
    @field          mFormatFlags
                        Flags specific to each format.
    @field          mBytesPerPacket
                        The number of bytes in a packet of data.
    @field          mFramesPerPacket
                        The number of sample frames in each packet of data.
    @field          mBytesPerFrame
                        The number of bytes in a single sample frame of data.
    @field          mChannelsPerFrame
                        The number of channels in each frame of data.
    @field          mBitsPerChannel
                        The number of bits of sample data for each channel in a frame of data.
    @field          mReserved
                        Pads the structure out to force an even 8 byte alignment.
*/
struct AudioStreamBasicDescription
{
    Float64 mSampleRate;
    UInt32  mFormatID;
    UInt32  mFormatFlags;
    UInt32  mBytesPerPacket;
    UInt32  mFramesPerPacket;
    UInt32  mBytesPerFrame;
    UInt32  mChannelsPerFrame;
    UInt32  mBitsPerChannel;
    UInt32  mReserved;
};
typedef struct AudioStreamBasicDescription  AudioStreamBasicDescription;

/*!
    @enum           AudioStreamBasicDescription Constants
    @abstract       Constants for use with AudioStreamBasicDescription
    @constant       kAudioStreamAnyRate
                        The format can use any sample rate. Note that this constant can only appear
                        in listings of supported formats. It will never appear in a current format.
*/
enum
{
    kAudioStreamAnyRate = 0
};

/*!
    @enum           Format IDs
    @abstract       The four char code IDs used to identify individual formats of audio data.
    @constant       kAudioFormatLinearPCM
                        Linear PCM, uses the standard flags.
    @constant       kAudioFormatAC3
                        AC-3, has no flags.
    @constant       kAudioFormat60958AC3
                        AC-3 packaged for transport over an IEC 60958 compliant digital audio
                        interface. Uses the standard flags.
    @constant       kAudioFormatAppleIMA4
                        Apples implementation of IMA 4:1 ADPCM, has no flags.
    @constant       kAudioFormatMPEG4AAC
                        MPEG-4 Low Complexity AAC audio object, has no flags.
    @constant       kAudioFormatMPEG4CELP
                        MPEG-4 CELP audio object, has no flags.
    @constant       kAudioFormatMPEG4HVXC
                        MPEG-4 HVXC audio object, has no flags.
    @constant       kAudioFormatMPEG4TwinVQ
                        MPEG-4 TwinVQ audio object type, has no flags.
    @constant       kAudioFormatMACE3
                        MACE 3:1, has no flags.
    @constant       kAudioFormatMACE6
                        MACE 6:1, has no flags.
    @constant       kAudioFormatULaw
                        µLaw 2:1, has no flags.
    @constant       kAudioFormatALaw
                        aLaw 2:1, has no flags.
    @constant       kAudioFormatQDesign
                        QDesign music, has no flags
    @constant       kAudioFormatQDesign2
                        QDesign2 music, has no flags
    @constant       kAudioFormatQUALCOMM
                        QUALCOMM PureVoice, has no flags
    @constant       kAudioFormatMPEGLayer1
                        MPEG-1/2, Layer 1 audio, has no flags
    @constant       kAudioFormatMPEGLayer2
                        MPEG-1/2, Layer 2 audio, has no flags
    @constant       kAudioFormatMPEGLayer3
                        MPEG-1/2, Layer 3 audio, has no flags
    @constant       kAudioFormatTimeCode
                        A stream of IOAudioTimeStamps, uses the IOAudioTimeStamp flags (see
                        IOKit/audio/IOAudioTypes.h).
    @constant       kAudioFormatMIDIStream
                        A stream of MIDIPacketLists where the time stamps in the MIDIPacketList are
                        sample offsets in the stream. The mSampleRate field is used to describe how
                        time is passed in this kind of stream and an AudioUnit that receives or
                        generates this stream can use this sample rate, the number of frames it is
                        rendering and the sample offsets within the MIDIPacketList to define the
                        time for any MIDI event within this list. It has no flags.
    @constant       kAudioFormatParameterValueStream
                        A "side-chain" of Float32 data that can be fed or generated by an AudioUnit
                        and is used to send a high density of parameter value control information.
                        An AU will typically run a ParameterValueStream at either the sample rate of
                        the AudioUnit's audio data, or some integer divisor of this (say a half or a
                        third of the sample rate of the audio). The Sample Rate of the ASBD
                        describes this relationship. It has no flags.
    @constant       kAudioFormatAppleLossless
                        Apple Lossless, the flags indicate the bit depth of the source material.
	@constant		kAudioFormatMPEG4AAC_HE
						MPEG-4 High Efficiency AAC audio object, has no flags.
    @constant       kAudioFormatMPEG4AAC_LD
                        MPEG-4 AAC Low Delay audio object, has no flags.
    @constant		kAudioFormatMPEG4AAC_HE_V2
						MPEG-4 High Efficiency AAC Version 2 audio object, has no flags. 
	@constant		kAudioFormatMPEG4AAC_Spatial
						MPEG-4 Spatial Audio audio object, has no flags.
	@constant		kAudioFormatAMR
						The AMR Narrow Band speech codec.
*/
enum
{
    kAudioFormatLinearPCM               = 'lpcm',
    kAudioFormatAC3                     = 'ac-3',
    kAudioFormat60958AC3                = 'cac3',
    kAudioFormatAppleIMA4               = 'ima4',
    kAudioFormatMPEG4AAC                = 'aac ',
    kAudioFormatMPEG4CELP               = 'celp',
    kAudioFormatMPEG4HVXC               = 'hvxc',
    kAudioFormatMPEG4TwinVQ             = 'twvq',
    kAudioFormatMACE3                   = 'MAC3',
    kAudioFormatMACE6                   = 'MAC6',
    kAudioFormatULaw                    = 'ulaw',
    kAudioFormatALaw                    = 'alaw',
    kAudioFormatQDesign                 = 'QDMC',
    kAudioFormatQDesign2                = 'QDM2',
    kAudioFormatQUALCOMM                = 'Qclp',
    kAudioFormatMPEGLayer1              = '.mp1',
    kAudioFormatMPEGLayer2              = '.mp2',
    kAudioFormatMPEGLayer3              = '.mp3',
    kAudioFormatTimeCode                = 'time',
    kAudioFormatMIDIStream              = 'midi',
    kAudioFormatParameterValueStream    = 'apvs',
    kAudioFormatAppleLossless           = 'alac',
    kAudioFormatMPEG4AAC_HE				= 'aach',
    kAudioFormatMPEG4AAC_LD				= 'aacl',
    kAudioFormatMPEG4AAC_HE_V2			= 'aacp',
    kAudioFormatMPEG4AAC_Spatial		= 'aacs',
    kAudioFormatAMR						= 'samr'
};

/*!
    @enum           Standard Flag Values for AudioStreamBasicDescription
    @abstract       These are the standard flags for use in the mFormatFlags field of the
                    AudioStreamBasicDescription structure.
    @discussion     Typically, when an ASBD is being used, the fields describe the complete layout
                    of the sample data in the buffers that are represented by this description -
                    where typically those buffers are represented by an AudioBuffer that is
                    contained in an AudioBufferList.

                    However, when an ASBD has the kAudioFormatFlagIsNonInterleaved flag, the
                    AudioBufferList has a different structure and semantic. In this case, the ASBD
                    fields will describe the format of ONE of the AudioBuffers that are contained in
                    the list, AND each AudioBuffer in the list is determined to have a single (mono)
                    channel of audio data. Then, the ASBD's mChannelsPerFrame will indicate the
                    total number of AudioBuffers that are contained within the AudioBufferList -
                    where each buffer contains one channel. This is used primarily with the
                    AudioUnit (and AudioConverter) representation of this list - and won't be found
                    in the AudioHardware usage of this structure.
    @constant       kAudioFormatFlagIsFloat
                        Set for floating point, clear for integer.
    @constant       kAudioFormatFlagIsBigEndian
                        Set for big endian, clear for little endian.
    @constant       kAudioFormatFlagIsSignedInteger
                        Set for signed integer, clear for unsigned integer. This is only valid if
                        kAudioFormatFlagIsFloat is clear.
    @constant       kAudioFormatFlagIsPacked
                        Set if the sample bits occupy the entire available bits for the channel,
                        clear if they are high or low aligned within the channel.
    @constant       kAudioFormatFlagIsAlignedHigh
                        Set if the sample bits are placed into the high bits of the channel, clear
                        for low bit placement. This is only valid if kAudioFormatFlagIsPacked is
                        clear.
    @constant       kAudioFormatFlagIsNonInterleaved
                        Set if the samples for each channel are located contiguously and the
                        channels are layed out end to end, clear if the samples for each frame are
                        layed out contiguously and the frames layed out end to end.
    @constant       kAudioFormatFlagIsNonMixable
                        Set to indicate when a format is non-mixable. Note that this flag is only
                        used when interacting with the HAL's stream format information. It is not a
                        valid flag for any other uses.
    @constant       kAudioFormatFlagsAreAllClear
                        Set if all the flags would be clear in order to preserve 0 as the wild card
                        value.
    @constant       kLinearPCMFormatFlagIsFloat
                        Synonym for kAudioFormatFlagIsFloat.
    @constant       kLinearPCMFormatFlagIsBigEndian
                        Synonym for kAudioFormatFlagIsBigEndian.
    @constant       kLinearPCMFormatFlagIsSignedInteger
                        Synonym for kAudioFormatFlagIsSignedInteger.
    @constant       kLinearPCMFormatFlagIsPacked
                        Synonym for kAudioFormatFlagIsPacked.
    @constant       kLinearPCMFormatFlagIsAlignedHigh
                        Synonym for kAudioFormatFlagIsAlignedHigh.
    @constant       kLinearPCMFormatFlagIsNonInterleaved
                        Synonym for kAudioFormatFlagIsNonInterleaved.
    @constant       kLinearPCMFormatFlagIsNonMixable
                        Synonym for kAudioFormatFlagIsNonMixable.
    @constant       kLinearPCMFormatFlagsAreAllClear
                        Synonym for kAudioFormatFlagsAreAllClear.
    @constant       kAppleLosslessFormatFlag_16BitSourceData
                        This flag is set for Apple Lossless data that was sourced from 16 bit native
                        endian signed integer data.
    @constant       kAppleLosslessFormatFlag_20BitSourceData
                        This flag is set for Apple Lossless data that was sourced from 20 bit native
                        endian signed integer data aligned high in 24 bits.
    @constant       kAppleLosslessFormatFlag_24BitSourceData
                        This flag is set for Apple Lossless data that was sourced from 24 bit native
                        endian signed integer data.
    @constant       kAppleLosslessFormatFlag_32BitSourceData
                        This flag is set for Apple Lossless data that was sourced from 32 bit native
                        endian signed integer data.
*/
enum
{
    kAudioFormatFlagIsFloat                     = (1L << 0),
    kAudioFormatFlagIsBigEndian                 = (1L << 1),
    kAudioFormatFlagIsSignedInteger             = (1L << 2),
    kAudioFormatFlagIsPacked                    = (1L << 3),
    kAudioFormatFlagIsAlignedHigh               = (1L << 4),
    kAudioFormatFlagIsNonInterleaved            = (1L << 5),
    kAudioFormatFlagIsNonMixable                = (1L << 6),
    kAudioFormatFlagsAreAllClear                = (1L << 31),
    
    kLinearPCMFormatFlagIsFloat                 = kAudioFormatFlagIsFloat,
    kLinearPCMFormatFlagIsBigEndian             = kAudioFormatFlagIsBigEndian,
    kLinearPCMFormatFlagIsSignedInteger         = kAudioFormatFlagIsSignedInteger,
    kLinearPCMFormatFlagIsPacked                = kAudioFormatFlagIsPacked,
    kLinearPCMFormatFlagIsAlignedHigh           = kAudioFormatFlagIsAlignedHigh,
    kLinearPCMFormatFlagIsNonInterleaved        = kAudioFormatFlagIsNonInterleaved,
    kLinearPCMFormatFlagIsNonMixable            = kAudioFormatFlagIsNonMixable,
    kLinearPCMFormatFlagsAreAllClear            = kAudioFormatFlagsAreAllClear,
    
    kAppleLosslessFormatFlag_16BitSourceData    = 1,
    kAppleLosslessFormatFlag_20BitSourceData    = 2,
    kAppleLosslessFormatFlag_24BitSourceData    = 3,
    kAppleLosslessFormatFlag_32BitSourceData    = 4
};

/*!
    @enum           Commonly Used Combinations of ASBD Flags
    @abstract       Some commonly used combinations of flags for AudioStreamBasicDescriptions.
    @constant       kAudioFormatFlagsNativeEndian
                        Defined to set or clear kAudioFormatFlagIsBigEndian depending on the
                        endianness of the processor at build time.
    @constant       kAudioFormatFlagsCanonical
                        The flags for the canonical audio sample type
	@constant       kAudioFormatFlagsNativeFloatPacked
                        The flags for fully packed, native endian floating point data.
*/
enum
{
#if TARGET_RT_BIG_ENDIAN
    kAudioFormatFlagsNativeEndian       = kAudioFormatFlagIsBigEndian,
#else
    kAudioFormatFlagsNativeEndian       = 0,
#endif
	kAudioFormatFlagsCanonical			= kAudioFormatFlagIsFloat | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked,
    kAudioFormatFlagsNativeFloatPacked  = kAudioFormatFlagIsFloat | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked
};

/*!
    @defined    TestAudioFormatNativeEndian
    @abstract   A macro for checking if an ASBD indicates native endian linear PCM data.
*/
#define TestAudioFormatNativeEndian(f)  ((f.mFormatID == kAudioFormatLinearPCM) && ((f.mFormatFlags & kAudioFormatFlagIsBigEndian) == kAudioFormatFlagsNativeEndian))

/*!
    @function   IsAudioFormatNativeEndian
    @abstract   A C++ inline function for checking if an ASBD indicates native endian linear PCM
                data.
    @param      f
                    The AudioStreamBasicDescription to examine.
    @result     Whether or not the ASBD indicates native endian linear PCM data.
*/
#if defined(__cplusplus)
    inline bool IsAudioFormatNativeEndian(const AudioStreamBasicDescription& f) { return (f.mFormatID == kAudioFormatLinearPCM) && ((f.mFormatFlags & kAudioFormatFlagIsBigEndian) == kAudioFormatFlagsNativeEndian); }
#endif

/*!
    @function   CalculateLPCMFlags
    @abstract   A C++ inline function for calculating the mFormatFlags for linear PCM data. Note
                that this function does not support specifying sample formats that are either
                unsigned integer or low-aligned.
    @param      inSampleRate
                    
    @param      inValidBitsPerChannel
                    The number of valid bits in each sample.
    @param      inTotalBitsPerChannel
                    The total number of bits in each sample.
    @param      inIsFloat
                    Whether or not the samples are represented with floating point numbers.
    @param      isIsBigEndian
                    Whether the samples are big endian or little endian.
    @result     A UInt32 containing the format flags.
*/
#if defined(__cplusplus)
inline UInt32    CalculateLPCMFlags(UInt32 inValidBitsPerChannel, UInt32 inTotalBitsPerChannel, bool inIsFloat, bool inIsBigEndian, bool inIsNonInterleaved = false) { return (inIsFloat ? kAudioFormatFlagIsFloat : kAudioFormatFlagIsSignedInteger) | (inIsBigEndian ? ((UInt32)kAudioFormatFlagIsBigEndian) : 0) | ((!inIsFloat && (inValidBitsPerChannel == inTotalBitsPerChannel)) ? kAudioFormatFlagIsPacked : kAudioFormatFlagIsAlignedHigh) | (inIsNonInterleaved ? ((UInt32)kAudioFormatFlagIsNonInterleaved) : 0); }
#endif

/*!
    @function   FillOutASBDForLPCM
    @abstract   A C++ inline function for filling out an AudioStreamBasicDescription to describe
                linear PCM data. Note that this function does not support specifying sample formats
                that are either unsigned integer or low-aligned.
    @param      outASBD
                    The AudioStreamBasicDescription to fill out.
    @param      inSampleRate
                    The number of sample frames per second of the data in the stream.
    @param      inChannelsPerFrame
                    The number of channels in each frame of data.
    @param      inValidBitsPerChannel
                    The number of valid bits in each sample.
    @param      inTotalBitsPerChannel
                    The total number of bits in each sample.
    @param      inIsFloat
                    Whether or not the samples are represented with floating point numbers.
    @param      isIsBigEndian
                    Whether the samples are big endian or little endian.
*/
#if defined(__cplusplus)
inline void    FillOutASBDForLPCM(AudioStreamBasicDescription& outASBD, Float64 inSampleRate, UInt32 inChannelsPerFrame, UInt32 inValidBitsPerChannel, UInt32 inTotalBitsPerChannel, bool inIsFloat, bool inIsBigEndian, bool inIsNonInterleaved = false)    { outASBD.mSampleRate = inSampleRate; outASBD.mFormatID = kAudioFormatLinearPCM; outASBD.mFormatFlags = CalculateLPCMFlags(inValidBitsPerChannel, inTotalBitsPerChannel, inIsFloat, inIsBigEndian, inIsNonInterleaved); outASBD.mBytesPerPacket = inChannelsPerFrame * (inTotalBitsPerChannel / 8); outASBD.mFramesPerPacket = 1; outASBD.mBytesPerFrame = inChannelsPerFrame * (inTotalBitsPerChannel / 8); outASBD.mChannelsPerFrame = inChannelsPerFrame; outASBD.mBitsPerChannel = inValidBitsPerChannel; }
#endif


/*!
    @struct         AudioStreamPacketDescription
    @abstract       This structure describes the packet layout of a buffer of data where the size of
                    each packet may not be the same or where there is extraneous data between
                    packets.
    @field          mStartOffset
                        The number of bytes from the start of the buffer to the beginning of the
                        packet.
    @field          mVariableFramesInPacket
                        The number of sample frames of data in the packet. For formats with a
                        constant number of frames per packet, this field is set to 0.
    @field          mDataByteSize
                        The number of bytes in the packet.
*/
struct  AudioStreamPacketDescription
{
    SInt64  mStartOffset;
    UInt32  mVariableFramesInPacket;
    UInt32  mDataByteSize;
};
typedef struct AudioStreamPacketDescription AudioStreamPacketDescription;

//  SMPTETime is also defined in the CoreVideo headers.
#if !defined(__SMPTETime__)
#define __SMPTETime__

/*!
    @struct         SMPTETime
    @abstract       A structure for holding a SMPTE time.
    @field          mSubframes
                        The number of subframes in the full message.
    @field          mSubframeDivisor
                        The number of subframes per frame (typically 80).
    @field          mCounter
                        The total number of messages received.
    @field          mType
                        The kind of SMPTE time using the SMPTE time type constants.
    @field          mFlags
                        A set of flags that indicate the SMPTE state.
    @field          mHours
                        The number of hourse in the full message.
    @field          mMinutes
                        The number of minutes in the full message.
    @field          mSeconds
                        The number of seconds in the full message.
    @field          mFrames
                        The number of frames in the full message.
*/
struct SMPTETime
{
    SInt16  mSubframes;
    SInt16  mSubframeDivisor;
    UInt32  mCounter;
    UInt32  mType;
    UInt32  mFlags;
    SInt16  mHours;
    SInt16  mMinutes;
    SInt16  mSeconds;
    SInt16  mFrames;
};
typedef struct SMPTETime    SMPTETime;

/*!
    @enum           SMPTE Time Types
    @abstract       Constants that describe the type of SMPTE time.
    @constant       kSMPTETimeType24
                        24 Frame
    @constant       kSMPTETimeType25
                        25 Frame
    @constant       kSMPTETimeType30Drop
                        30 Drop Frame
    @constant       kSMPTETimeType30
                        30 Frame
    @constant       kSMPTETimeType2997
                        29.97 Frame
    @constant       kSMPTETimeType2997Drop
                        29.97 Drop Frame
    @constant       kSMPTETimeType60
                        60 Frame
    @constant       kSMPTETimeType5994
                        59.94 Frame
    @constant       kSMPTETimeType60Drop
                        60 Drop Frame
    @constant       kSMPTETimeType5994Drop
                        59.94 Drop Frame
    @constant       kSMPTETimeType50
                        50 Frame
    @constant       kSMPTETimeType2398
                        23.98 Frame
*/
enum
{
    kSMPTETimeType24        = 0,
    kSMPTETimeType25        = 1,
    kSMPTETimeType30Drop    = 2,
    kSMPTETimeType30        = 3,
    kSMPTETimeType2997      = 4,
    kSMPTETimeType2997Drop  = 5,
    kSMPTETimeType60        = 6,
    kSMPTETimeType5994      = 7,
    kSMPTETimeType60Drop    = 8,
    kSMPTETimeType5994Drop  = 9,
    kSMPTETimeType50        = 10,
    kSMPTETimeType2398      = 11
};

/*!
    @enum           SMPTE State Flags
    @abstract       Flags that describe the SMPTE time state.
    @constant       kSMPTETimeValid
                        The full time is valid.
    @constant       kSMPTETimeRunning
                        Time is running.
*/
enum
{
    kSMPTETimeValid     = (1L << 0),
    kSMPTETimeRunning   = (1L << 1)
};

#endif

/*!
    @struct         AudioTimeStamp
    @abstract       A strucutre that holds different representations of the same point in time.
    @field          mSampleTime
                        The absolute sample frame time.
    @field          mHostTime
                        The host machine's time base (see <CoreAudio/HostTime.h>).
    @field          mRateScalar
                        The ratio of actual host ticks per sample frame to the nominal host ticks
                        per sample frame.
    @field          mWordClockTime
                        The word clock time.
    @field          mSMPTETime
                        The SMPTE time.
    @field          mFlags
                        A set of flags indicating which representations of the time are valid.
    @field          mReserved
                        Pads the structure out to force an even 8 byte alignment.
*/
struct AudioTimeStamp
{
    Float64         mSampleTime;
    UInt64          mHostTime;
    Float64         mRateScalar;
    UInt64          mWordClockTime;
    SMPTETime       mSMPTETime;
    UInt32          mFlags;
    UInt32          mReserved;
};
typedef struct AudioTimeStamp   AudioTimeStamp;

/*!
    @enum           AudioTimeStamp Flags
    @abstract       The flags that indicate which fields in an AudioTimeStamp structure are valid.
    @constant       kAudioTimeStampSampleTimeValid
                        The sample frame time is valid.
    @constant       kAudioTimeStampHostTimeValid
                        The host time is valid.
    @constant       kAudioTimeStampRateScalarValid
                        The rate scalar is valid.
    @constant       kAudioTimeStampWordClockTimeValid
                        The word clock time is valid.
    @constant       kAudioTimeStampSMPTETimeValid
                        The SMPTE time is valid.
*/
enum
{
    kAudioTimeStampSampleTimeValid      = (1L << 0),
    kAudioTimeStampHostTimeValid        = (1L << 1),
    kAudioTimeStampRateScalarValid      = (1L << 2),
    kAudioTimeStampWordClockTimeValid   = (1L << 3),
    kAudioTimeStampSMPTETimeValid       = (1L << 4)
};

/*!
    @enum           Commonly Used Combinations of AudioTimeStamp Flags
    @abstract       Some commonly used combinations of AudioTimeStamp flags.
    @constant       kAudioTimeStampSampleHostTimeValid
                        The sample frame time and the host time are valid.
*/
enum
{
    kAudioTimeStampSampleHostTimeValid  = (kAudioTimeStampSampleTimeValid | kAudioTimeStampHostTimeValid)
};

/*!
    @function   FillOutAudioTimeStampWithSampleTime
    @abstract   A C++ inline function for filling out an AudioTimeStamp with a sample time
    @param      outATS
                    The AudioTimeStamp to fill out.
    @param      inSampleTime
                    The sample time to put in the AudioTimeStamp.
*/
#if defined(__cplusplus)
inline void    FillOutAudioTimeStampWithSampleTime(AudioTimeStamp& outATS, Float64 inSampleTime)    { outATS.mSampleTime = inSampleTime; outATS.mHostTime = 0; outATS.mRateScalar = 0; outATS.mWordClockTime = 0; memset(&outATS.mSMPTETime, 0, sizeof(SMPTETime)); outATS.mFlags = kAudioTimeStampSampleTimeValid; }
#endif

/*!
    @function   FillOutAudioTimeStampWithHostTime
    @abstract   A C++ inline function for filling out an AudioTimeStamp with a host time
    @param      outATS
                    The AudioTimeStamp to fill out.
    @param      inHostTime
                    The host time to put in the AudioTimeStamp.
*/
#if defined(__cplusplus)
inline void    FillOutAudioTimeStampWithHostTime(AudioTimeStamp& outATS, UInt64 inHostTime) { outATS.mSampleTime = 0; outATS.mHostTime = inHostTime; outATS.mRateScalar = 0; outATS.mWordClockTime = 0; memset(&outATS.mSMPTETime, 0, sizeof(SMPTETime)); outATS.mFlags = kAudioTimeStampHostTimeValid; }
#endif

/*!
    @function   FillOutAudioTimeStampWithSampleAndHostTime
    @abstract   A C++ inline function for filling out an AudioTimeStamp with a sample time and a
                host time
    @param      outATS
                    The AudioTimeStamp to fill out.
    @param      inSampleTime
                    The sample time to put in the AudioTimeStamp.
    @param      inHostTime
                    The host time to put in the AudioTimeStamp.
*/
#if defined(__cplusplus)
inline void    FillOutAudioTimeStampWithSampleAndHostTime(AudioTimeStamp& outATS, Float64 inSampleTime, UInt64 inHostTime) { outATS.mSampleTime = inSampleTime; outATS.mHostTime = inHostTime; outATS.mRateScalar = 0; outATS.mWordClockTime = 0; memset(&outATS.mSMPTETime, 0, sizeof(SMPTETime)); outATS.mFlags = kAudioTimeStampSampleTimeValid | kAudioTimeStampHostTimeValid; }
#endif

/*!
    @struct         AudioClassDescription
    @abstract       This structure is used to describe codecs installed on the system.
    @field          mType
                        The four char code codec type.
    @field          mSubType
                        The four char code codec subtype.
    @field          mManufacturer
                        The four char code codec manufacturer.
*/
struct AudioClassDescription {
    OSType  mType;
    OSType  mSubType;
    OSType  mManufacturer;
};
typedef struct AudioClassDescription    AudioClassDescription;


/*!
    @typedef        AudioChannelLabel
    @abstract       A tag idenitfying how the channel is to be used.
*/
typedef UInt32 AudioChannelLabel;

/*!
    @typedef        AudioChannelLayoutTag
    @abstract       A tag identifying a particular pre-defined channel layout.
*/
typedef UInt32 AudioChannelLayoutTag;

/*!
    @struct         AudioChannelDescription
    @abstract       This structure describes a single channel.
    @field          mChannelLabel
                        The AudioChannelLabel that describes the channel.
    @field          mChannelFlags
                        Flags that control the interpretation of mCoordinates.
    @field          mCoordinates
                        An ordered triple that specifies a precise speaker location.
*/
struct AudioChannelDescription
{
    AudioChannelLabel   mChannelLabel;
    UInt32              mChannelFlags;
    Float32             mCoordinates[3];
};
typedef struct AudioChannelDescription AudioChannelDescription;

/*!
    @struct         AudioChannelLayout
    @abstract       This structure is used to specify channel layouts in files and hardware.
    @field          mChannelLayoutTag
                        The AudioChannelLayoutTag that indicates the layout.
    @field          mChannelBitmap
                        If mChannelLayoutTag is set to kAudioChannelLayoutTag_UseChannelBitmap, this
                        field is the channel usage bitmap.
    @field          mNumberChannelDescriptions
                        The number of items in the mChannelDescriptions array.
    @field          mChannelDescriptions
                        A variable length array of AudioChannelDescriptions that describe the
                        layout.
*/
struct AudioChannelLayout
{
    AudioChannelLayoutTag       mChannelLayoutTag;
    UInt32                      mChannelBitmap;
    UInt32                      mNumberChannelDescriptions;
    AudioChannelDescription     mChannelDescriptions[kVariableLengthArray];
};
typedef struct AudioChannelLayout AudioChannelLayout;

/*!
    @enum           AudioChannelLabel Constants
    @abstract       These constants are for use in the mChannelLabel field of an
                    AudioChannelDescription structure.
    @discussion     These channel labels attempt to list all labels in common use. Due to the
                    ambiguities in channel labeling by various groups, there may be some overlap or
                    duplication in the labels below. Use the label which most clearly describes what
                    you mean.

                    WAVE files seem to follow the USB spec for the channel flags. A channel map lets
                    you put these channels in any order, however a WAVE file only supports labels
                    1-18 and if present, they must be in the order given below. The integer values
                    for the labels below match the bit position of the label in the USB bitmap and
                    thus also the WAVE file bitmap.
*/
enum
{
    kAudioChannelLabel_Unknown                  = 0xFFFFFFFF,   // unknown or unspecified other use
    kAudioChannelLabel_Unused                   = 0,            // channel is present, but has no intended use or destination
    kAudioChannelLabel_UseCoordinates           = 100,          // channel is described by the mCoordinates fields.

    kAudioChannelLabel_Left                     = 1,
    kAudioChannelLabel_Right                    = 2,
    kAudioChannelLabel_Center                   = 3,
    kAudioChannelLabel_LFEScreen                = 4,
    kAudioChannelLabel_LeftSurround             = 5,            // WAVE: "Back Left"
    kAudioChannelLabel_RightSurround            = 6,            // WAVE: "Back Right"
    kAudioChannelLabel_LeftCenter               = 7,
    kAudioChannelLabel_RightCenter              = 8,
    kAudioChannelLabel_CenterSurround           = 9,            // WAVE: "Back Center" or plain "Rear Surround"
    kAudioChannelLabel_LeftSurroundDirect       = 10,           // WAVE: "Side Left"
    kAudioChannelLabel_RightSurroundDirect      = 11,           // WAVE: "Side Right"
    kAudioChannelLabel_TopCenterSurround        = 12,
    kAudioChannelLabel_VerticalHeightLeft       = 13,           // WAVE: "Top Front Left"
    kAudioChannelLabel_VerticalHeightCenter     = 14,           // WAVE: "Top Front Center"
    kAudioChannelLabel_VerticalHeightRight      = 15,           // WAVE: "Top Front Right"

    kAudioChannelLabel_TopBackLeft              = 16,
    kAudioChannelLabel_TopBackCenter            = 17,
    kAudioChannelLabel_TopBackRight             = 18,

    kAudioChannelLabel_RearSurroundLeft         = 33,
    kAudioChannelLabel_RearSurroundRight        = 34,
    kAudioChannelLabel_LeftWide                 = 35,
    kAudioChannelLabel_RightWide                = 36,
    kAudioChannelLabel_LFE2                     = 37,
    kAudioChannelLabel_LeftTotal                = 38,           // matrix encoded 4 channels
    kAudioChannelLabel_RightTotal               = 39,           // matrix encoded 4 channels
    kAudioChannelLabel_HearingImpaired          = 40,
    kAudioChannelLabel_Narration                = 41,
    kAudioChannelLabel_Mono                     = 42,
    kAudioChannelLabel_DialogCentricMix         = 43,

    kAudioChannelLabel_CenterSurroundDirect     = 44,           // back center, non diffuse
	
    kAudioChannelLabel_Haptic					= 45,

    // first order ambisonic channels
    kAudioChannelLabel_Ambisonic_W              = 200,
    kAudioChannelLabel_Ambisonic_X              = 201,
    kAudioChannelLabel_Ambisonic_Y              = 202,
    kAudioChannelLabel_Ambisonic_Z              = 203,

    // Mid/Side Recording
    kAudioChannelLabel_MS_Mid                   = 204,
    kAudioChannelLabel_MS_Side                  = 205,

    // X-Y Recording
    kAudioChannelLabel_XY_X                     = 206,
    kAudioChannelLabel_XY_Y                     = 207,

    // other
    kAudioChannelLabel_HeadphonesLeft           = 301,
    kAudioChannelLabel_HeadphonesRight          = 302,
    kAudioChannelLabel_ClickTrack               = 304,
    kAudioChannelLabel_ForeignLanguage          = 305,

    // generic discrete channel
    kAudioChannelLabel_Discrete                 = 400,

    // numbered discrete channel
    kAudioChannelLabel_Discrete_0               = (1L<<16) | 0,
    kAudioChannelLabel_Discrete_1               = (1L<<16) | 1,
    kAudioChannelLabel_Discrete_2               = (1L<<16) | 2,
    kAudioChannelLabel_Discrete_3               = (1L<<16) | 3,
    kAudioChannelLabel_Discrete_4               = (1L<<16) | 4,
    kAudioChannelLabel_Discrete_5               = (1L<<16) | 5,
    kAudioChannelLabel_Discrete_6               = (1L<<16) | 6,
    kAudioChannelLabel_Discrete_7               = (1L<<16) | 7,
    kAudioChannelLabel_Discrete_8               = (1L<<16) | 8,
    kAudioChannelLabel_Discrete_9               = (1L<<16) | 9,
    kAudioChannelLabel_Discrete_10              = (1L<<16) | 10,
    kAudioChannelLabel_Discrete_11              = (1L<<16) | 11,
    kAudioChannelLabel_Discrete_12              = (1L<<16) | 12,
    kAudioChannelLabel_Discrete_13              = (1L<<16) | 13,
    kAudioChannelLabel_Discrete_14              = (1L<<16) | 14,
    kAudioChannelLabel_Discrete_15              = (1L<<16) | 15,
    kAudioChannelLabel_Discrete_65535           = (1L<<16) | 65535
};

/*!
    @enum           Channel Bitmap Constants
    @abstract       These constants are for use in the mChannelBitmap field of an
                    AudioChannelLayout structure.
*/
enum
{
    kAudioChannelBit_Left                       = (1L<<0),
    kAudioChannelBit_Right                      = (1L<<1),
    kAudioChannelBit_Center                     = (1L<<2),
    kAudioChannelBit_LFEScreen                  = (1L<<3),
    kAudioChannelBit_LeftSurround               = (1L<<4),      // WAVE: "Back Left"
    kAudioChannelBit_RightSurround              = (1L<<5),      // WAVE: "Back Right"
    kAudioChannelBit_LeftCenter                 = (1L<<6),
    kAudioChannelBit_RightCenter                = (1L<<7),
    kAudioChannelBit_CenterSurround             = (1L<<8),      // WAVE: "Back Center"
    kAudioChannelBit_LeftSurroundDirect         = (1L<<9),      // WAVE: "Side Left"
    kAudioChannelBit_RightSurroundDirect        = (1L<<10),     // WAVE: "Side Right"
    kAudioChannelBit_TopCenterSurround          = (1L<<11),
    kAudioChannelBit_VerticalHeightLeft         = (1L<<12),     // WAVE: "Top Front Left"
    kAudioChannelBit_VerticalHeightCenter       = (1L<<13),     // WAVE: "Top Front Center"
    kAudioChannelBit_VerticalHeightRight        = (1L<<14),     // WAVE: "Top Front Right"
    kAudioChannelBit_TopBackLeft                = (1L<<15),
    kAudioChannelBit_TopBackCenter              = (1L<<16),
    kAudioChannelBit_TopBackRight               = (1L<<17)
};

/*!
    @enum           Channel Coordinate Flags
    @abstract       These constants are used in the mChannelFlags field of an
                    AudioChannelDescription structure.
    @constant       kAudioChannelFlags_RectangularCoordinates
                        The channel is specified by the cartesioan coordinates of the speaker
                        position. This flag is mutally exclusive with
                        kAudioChannelFlags_SphericalCoordinates.
    @constant       kAudioChannelFlags_SphericalCoordinates
                        The channel is specified by the spherical coordinates of the speaker
                        position. This flag is mutally exclusive with
                        kAudioChannelFlags_RectangularCoordinates.
    @constant       kAudioChannelFlags_Meters
                        Set to indicate the units are in meters, clear to indicate the units are
                        relative to the unit cube or unit sphere.
*/
enum
{
	kAudioChannelFlags_AllOff					= 0,
    kAudioChannelFlags_RectangularCoordinates   = (1L<<0),
    kAudioChannelFlags_SphericalCoordinates     = (1L<<1),
    kAudioChannelFlags_Meters                   = (1L<<2)
};

// these are indices for acessing the mCoordinates array in struct AudioChannelDescription
/*!
    @enum           Channel Coordinate Index Constants
    @abstract       Constants for indexing the mCoordinates array in an AudioChannelDescription
                    structure.
    @constant       kAudioChannelCoordinates_LeftRight
                        For rectangulare coordinates, negative is left and positive is right.
    @constant       kAudioChannelCoordinates_BackFront
                        For rectangulare coordinates, negative is back and positive is front.
    @constant       kAudioChannelCoordinates_DownUp
                        For rectangulare coordinates, negative is below ground level, 0 is ground
                        level, and positive is above ground level.
    @constant       kAudioChannelCoordinates_Azimuth
                        For spherical coordinates, 0 is front center, positive is right, negative is
                        left. This is measured in degrees.
    @constant       kAudioChannelCoordinates_Elevation
                        For spherical coordinates, +90 is zenith, 0 is horizontal, -90 is nadir.
                        This is measured in degrees.
    @constant       kAudioChannelCoordinates_Distance
                        For spherical coordinates, the units are described by flags.
*/
enum
{
    kAudioChannelCoordinates_LeftRight  = 0,
    kAudioChannelCoordinates_BackFront  = 1,
    kAudioChannelCoordinates_DownUp     = 2,
    kAudioChannelCoordinates_Azimuth    = 0,
    kAudioChannelCoordinates_Elevation  = 1,
    kAudioChannelCoordinates_Distance   = 2
};

/*!
    @function       AudioChannelLayoutTag_GetNumberOfChannels
    @abstract       A macro to get the number of channels out of an AudioChannelLayoutTag
    @discussion     The low 16 bits of an AudioChannelLayoutTag gives the number of channels except
                    for kAudioChannelLayoutTag_UseChannelDescriptions and
                    kAudioChannelLayoutTag_UseChannelBitmap.
    @param          layoutTag
                        The AudioChannelLayoutTag to examine.
    @result         The number of channels the tag indicates.
*/
#define AudioChannelLayoutTag_GetNumberOfChannels(layoutTag) ((UInt32)((layoutTag) & 0x0000FFFF))

/*!
    @enum           AudioChannelLayoutTag Constants
    @abstract       These constants are used in the mChannelLayoutTag field of an AudioChannelLayout
                    structure.
*/
enum
{
    // Some channel abbreviations used below:
    // L - left
    // R - right
    // C - center
    // Ls - left surround
    // Rs - right surround
    // Cs - center surround
    // Rls - rear left surround
    // Rrs - rear right surround
    // Lw - left wide
    // Rw - right wide
    // Lsd - left surround direct
    // Rsd - right surround direct
    // Lc - left center
    // Rc - right center
    // Ts - top surround
    // Vhl - vertical height left
    // Vhc - vertical height center
    // Vhr - vertical height right
    // Lt - left matrix total. for matrix encoded stereo.
    // Rt - right matrix total. for matrix encoded stereo.

    //  General layouts
    kAudioChannelLayoutTag_UseChannelDescriptions   = (0L<<16) | 0,     // use the array of AudioChannelDescriptions to define the mapping.
    kAudioChannelLayoutTag_UseChannelBitmap         = (1L<<16) | 0,     // use the bitmap to define the mapping.

    kAudioChannelLayoutTag_Mono                     = (100L<<16) | 1,   // a standard mono stream
    kAudioChannelLayoutTag_Stereo                   = (101L<<16) | 2,   // a standard stereo stream (L R) - implied playback
    kAudioChannelLayoutTag_StereoHeadphones         = (102L<<16) | 2,   // a standard stereo stream (L R) - implied headphone playbac
    kAudioChannelLayoutTag_MatrixStereo             = (103L<<16) | 2,   // a matrix encoded stereo stream (Lt, Rt)
    kAudioChannelLayoutTag_MidSide                  = (104L<<16) | 2,   // mid/side recording
    kAudioChannelLayoutTag_XY                       = (105L<<16) | 2,   // coincident mic pair (often 2 figure 8's)
    kAudioChannelLayoutTag_Binaural                 = (106L<<16) | 2,   // binaural stereo (left, right)
    kAudioChannelLayoutTag_Ambisonic_B_Format       = (107L<<16) | 4,   // W, X, Y, Z

    kAudioChannelLayoutTag_Quadraphonic             = (108L<<16) | 4,   // front left, front right, back left, back right

    kAudioChannelLayoutTag_Pentagonal               = (109L<<16) | 5,   // left, right, rear left, rear right, center

    kAudioChannelLayoutTag_Hexagonal                = (110L<<16) | 6,   // left, right, rear left, rear right, center, rear

    kAudioChannelLayoutTag_Octagonal                = (111L<<16) | 8,   // front left, front right, rear left, rear right,
                                                                        // front center, rear center, side left, side right

    kAudioChannelLayoutTag_Cube                     = (112L<<16) | 8,   // left, right, rear left, rear right
                                                                        // top left, top right, top rear left, top rear right

    //  MPEG defined layouts
    kAudioChannelLayoutTag_MPEG_1_0                 = kAudioChannelLayoutTag_Mono,          //  C
    kAudioChannelLayoutTag_MPEG_2_0                 = kAudioChannelLayoutTag_Stereo,        //  L R
    kAudioChannelLayoutTag_MPEG_3_0_A               = (113L<<16) | 3,                       //  L R C
    kAudioChannelLayoutTag_MPEG_3_0_B               = (114L<<16) | 3,                       //  C L R
    kAudioChannelLayoutTag_MPEG_4_0_A               = (115L<<16) | 4,                       //  L R C Cs
    kAudioChannelLayoutTag_MPEG_4_0_B               = (116L<<16) | 4,                       //  C L R Cs
    kAudioChannelLayoutTag_MPEG_5_0_A               = (117L<<16) | 5,                       //  L R C Ls Rs
    kAudioChannelLayoutTag_MPEG_5_0_B               = (118L<<16) | 5,                       //  L R Ls Rs C
    kAudioChannelLayoutTag_MPEG_5_0_C               = (119L<<16) | 5,                       //  L C R Ls Rs
    kAudioChannelLayoutTag_MPEG_5_0_D               = (120L<<16) | 5,                       //  C L R Ls Rs
    kAudioChannelLayoutTag_MPEG_5_1_A               = (121L<<16) | 6,                       //  L R C LFE Ls Rs
    kAudioChannelLayoutTag_MPEG_5_1_B               = (122L<<16) | 6,                       //  L R Ls Rs C LFE
    kAudioChannelLayoutTag_MPEG_5_1_C               = (123L<<16) | 6,                       //  L C R Ls Rs LFE
    kAudioChannelLayoutTag_MPEG_5_1_D               = (124L<<16) | 6,                       //  C L R Ls Rs LFE
    kAudioChannelLayoutTag_MPEG_6_1_A               = (125L<<16) | 7,                       //  L R C LFE Ls Rs Cs
    kAudioChannelLayoutTag_MPEG_7_1_A               = (126L<<16) | 8,                       //  L R C LFE Ls Rs Lc Rc
    kAudioChannelLayoutTag_MPEG_7_1_B               = (127L<<16) | 8,                       //  C Lc Rc L R Ls Rs LFE    (doc: IS-13818-7 MPEG2-AAC Table 3.1)
    kAudioChannelLayoutTag_MPEG_7_1_C               = (128L<<16) | 8,                       //  L R C LFE Ls Rs Rls Rrs
    kAudioChannelLayoutTag_Emagic_Default_7_1       = (129L<<16) | 8,                       //  L R Ls Rs C LFE Lc Rc
    kAudioChannelLayoutTag_SMPTE_DTV                = (130L<<16) | 8,                       //  L R C LFE Ls Rs Lt Rt
                                                                                            //      (kAudioChannelLayoutTag_ITU_5_1 plus a matrix encoded stereo mix)

    //  ITU defined layouts
    kAudioChannelLayoutTag_ITU_1_0                  = kAudioChannelLayoutTag_Mono,          //  C
    kAudioChannelLayoutTag_ITU_2_0                  = kAudioChannelLayoutTag_Stereo,        //  L R

    kAudioChannelLayoutTag_ITU_2_1                  = (131L<<16) | 3,                       //  L R Cs
    kAudioChannelLayoutTag_ITU_2_2                  = (132L<<16) | 4,                       //  L R Ls Rs
    kAudioChannelLayoutTag_ITU_3_0                  = kAudioChannelLayoutTag_MPEG_3_0_A,    //  L R C
    kAudioChannelLayoutTag_ITU_3_1                  = kAudioChannelLayoutTag_MPEG_4_0_A,    //  L R C Cs

    kAudioChannelLayoutTag_ITU_3_2                  = kAudioChannelLayoutTag_MPEG_5_0_A,    //  L R C Ls Rs
    kAudioChannelLayoutTag_ITU_3_2_1                = kAudioChannelLayoutTag_MPEG_5_1_A,    //  L R C LFE Ls Rs
    kAudioChannelLayoutTag_ITU_3_4_1                = kAudioChannelLayoutTag_MPEG_7_1_C,    //  L R C LFE Ls Rs Rls Rrs

    // DVD defined layouts
    kAudioChannelLayoutTag_DVD_0                    = kAudioChannelLayoutTag_Mono,          // C (mono)
    kAudioChannelLayoutTag_DVD_1                    = kAudioChannelLayoutTag_Stereo,        // L R
    kAudioChannelLayoutTag_DVD_2                    = kAudioChannelLayoutTag_ITU_2_1,       // L R Cs
    kAudioChannelLayoutTag_DVD_3                    = kAudioChannelLayoutTag_ITU_2_2,       // L R Ls Rs
    kAudioChannelLayoutTag_DVD_4                    = (133L<<16) | 3,                       // L R LFE
    kAudioChannelLayoutTag_DVD_5                    = (134L<<16) | 4,                       // L R LFE Cs
    kAudioChannelLayoutTag_DVD_6                    = (135L<<16) | 5,                       // L R LFE Ls Rs
    kAudioChannelLayoutTag_DVD_7                    = kAudioChannelLayoutTag_MPEG_3_0_A,    // L R C
    kAudioChannelLayoutTag_DVD_8                    = kAudioChannelLayoutTag_MPEG_4_0_A,    // L R C Cs
    kAudioChannelLayoutTag_DVD_9                    = kAudioChannelLayoutTag_MPEG_5_0_A,    // L R C Ls Rs
    kAudioChannelLayoutTag_DVD_10                   = (136L<<16) | 4,                       // L R C LFE
    kAudioChannelLayoutTag_DVD_11                   = (137L<<16) | 5,                       // L R C LFE Cs
    kAudioChannelLayoutTag_DVD_12                   = kAudioChannelLayoutTag_MPEG_5_1_A,    // L R C LFE Ls Rs
    // 13 through 17 are duplicates of 8 through 12.
    kAudioChannelLayoutTag_DVD_13                   = kAudioChannelLayoutTag_DVD_8,         // L R C Cs
    kAudioChannelLayoutTag_DVD_14                   = kAudioChannelLayoutTag_DVD_9,         // L R C Ls Rs
    kAudioChannelLayoutTag_DVD_15                   = kAudioChannelLayoutTag_DVD_10,        // L R C LFE
    kAudioChannelLayoutTag_DVD_16                   = kAudioChannelLayoutTag_DVD_11,        // L R C LFE Cs
    kAudioChannelLayoutTag_DVD_17                   = kAudioChannelLayoutTag_DVD_12,        // L R C LFE Ls Rs
    kAudioChannelLayoutTag_DVD_18                   = (138L<<16) | 5,                       // L R Ls Rs LFE
    kAudioChannelLayoutTag_DVD_19                   = kAudioChannelLayoutTag_MPEG_5_0_B,    // L R Ls Rs C
    kAudioChannelLayoutTag_DVD_20                   = kAudioChannelLayoutTag_MPEG_5_1_B,    // L R Ls Rs C LFE

    // These layouts are recommended for AudioUnit usage
        // These are the symmetrical layouts
    kAudioChannelLayoutTag_AudioUnit_4              = kAudioChannelLayoutTag_Quadraphonic,
    kAudioChannelLayoutTag_AudioUnit_5              = kAudioChannelLayoutTag_Pentagonal,
    kAudioChannelLayoutTag_AudioUnit_6              = kAudioChannelLayoutTag_Hexagonal,
    kAudioChannelLayoutTag_AudioUnit_8              = kAudioChannelLayoutTag_Octagonal,
        // These are the surround-based layouts
    kAudioChannelLayoutTag_AudioUnit_5_0            = kAudioChannelLayoutTag_MPEG_5_0_B,    // L R Ls Rs C
    kAudioChannelLayoutTag_AudioUnit_6_0            = (139L<<16) | 6,                       // L R Ls Rs C Cs
    kAudioChannelLayoutTag_AudioUnit_7_0            = (140L<<16) | 7,                       // L R Ls Rs C Rls Rrs
	kAudioChannelLayoutTag_AudioUnit_7_0_Front      = (148L<<16) | 7,                       // L R Ls Rs C Lc Rc
	kAudioChannelLayoutTag_AudioUnit_5_1            = kAudioChannelLayoutTag_MPEG_5_1_A,    // L R C LFE Ls Rs
    kAudioChannelLayoutTag_AudioUnit_6_1            = kAudioChannelLayoutTag_MPEG_6_1_A,    // L R C LFE Ls Rs Cs
    kAudioChannelLayoutTag_AudioUnit_7_1            = kAudioChannelLayoutTag_MPEG_7_1_C,    // L R C LFE Ls Rs Rls Rrs
    kAudioChannelLayoutTag_AudioUnit_7_1_Front      = kAudioChannelLayoutTag_MPEG_7_1_A,    // L R C LFE Ls Rs Lc Rc

    kAudioChannelLayoutTag_AAC_3_0                  = kAudioChannelLayoutTag_MPEG_3_0_B,    // C L R
    kAudioChannelLayoutTag_AAC_Quadraphonic         = kAudioChannelLayoutTag_Quadraphonic,  // L R Ls Rs
    kAudioChannelLayoutTag_AAC_4_0                  = kAudioChannelLayoutTag_MPEG_4_0_B,    // C L R Cs
    kAudioChannelLayoutTag_AAC_5_0                  = kAudioChannelLayoutTag_MPEG_5_0_D,    // C L R Ls Rs
    kAudioChannelLayoutTag_AAC_5_1                  = kAudioChannelLayoutTag_MPEG_5_1_D,    // C L R Ls Rs Lfe
    kAudioChannelLayoutTag_AAC_6_0                  = (141L<<16) | 6,                       // C L R Ls Rs Cs
    kAudioChannelLayoutTag_AAC_6_1                  = (142L<<16) | 7,                       // C L R Ls Rs Cs Lfe
    kAudioChannelLayoutTag_AAC_7_0                  = (143L<<16) | 7,                       // C L R Ls Rs Rls Rrs
    kAudioChannelLayoutTag_AAC_7_1                  = kAudioChannelLayoutTag_MPEG_7_1_B,    // C Lc Rc L R Ls Rs Lfe
    kAudioChannelLayoutTag_AAC_Octagonal            = (144L<<16) | 8,                       // C L R Ls Rs Rls Rrs Cs

    kAudioChannelLayoutTag_TMH_10_2_std             = (145L<<16) | 16,                      // L R C Vhc Lsd Rsd Ls Rs Vhl Vhr Lw Rw Csd Cs LFE1 LFE2
    kAudioChannelLayoutTag_TMH_10_2_full            = (146L<<16) | 21,                       // TMH_10_2_std plus: Lc Rc HI VI Haptic

	kAudioChannelLayoutTag_AC3_1_0_1				= (149L<<16) | 2,                       // C LFE
	kAudioChannelLayoutTag_AC3_3_0					= (150L<<16) | 3,                       // L C R
	kAudioChannelLayoutTag_AC3_3_1					= (151L<<16) | 4,                       // L C R Cs
	kAudioChannelLayoutTag_AC3_3_0_1				= (152L<<16) | 4,                       // L C R LFE
	kAudioChannelLayoutTag_AC3_2_1_1				= (153L<<16) | 4,                       // L R Cs LFE
	kAudioChannelLayoutTag_AC3_3_1_1				= (154L<<16) | 5,                       // L C R Cs LFE

	kAudioChannelLayoutTag_DiscreteInOrder			= (147L<<16) | 0,						// needs to be ORed with the actual number of channels  
	kAudioChannelLayoutTag_Unknown					= 0xFFFF0000							// needs to be ORed with the actual number of channels  
};


// Deprecated constants

/*!	@enum			MPEG-4 Audio Object IDs
	@deprecated		in version 10.5

	@abstract		Constants that describe the various kinds of MPEG-4 audio data.
	@discussion		These constants are used in the flags field of an AudioStreamBasicDescription
					that describes an MPEG-4 audio stream.
*/
enum
{
    kMPEG4Object_AAC_Main       = 1,
    kMPEG4Object_AAC_LC         = 2,
    kMPEG4Object_AAC_SSR        = 3,
    kMPEG4Object_AAC_LTP        = 4,
    kMPEG4Object_AAC_SBR        = 5,
    kMPEG4Object_AAC_Scalable   = 6,
    kMPEG4Object_TwinVQ         = 7,
    kMPEG4Object_CELP           = 8,
    kMPEG4Object_HVXC           = 9
};    

//==================================================================================================

#if defined(__cplusplus)
}
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
#endif

#endif  //  __CoreAudioTypes_h__
