/*
     File:       TextEncodingConverter.h
 
     Contains:   Text Encoding Conversion Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1994-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __TEXTENCODINGCONVERTER__
#define __TEXTENCODINGCONVERTER__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif




#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

typedef OSType                          TECPluginSignature;
typedef UInt32                          TECPluginVersion;
/* plugin signatures */
enum {
  kTECSignature                 = FOUR_CHAR_CODE('encv'),
  kTECUnicodePluginSignature    = FOUR_CHAR_CODE('puni'),
  kTECJapanesePluginSignature   = FOUR_CHAR_CODE('pjpn'),
  kTECChinesePluginSignature    = FOUR_CHAR_CODE('pzho'),
  kTECKoreanPluginSignature     = FOUR_CHAR_CODE('pkor')
};


/* converter object reference */
typedef struct OpaqueTECObjectRef*      TECObjectRef;
typedef struct OpaqueTECSnifferObjectRef*  TECSnifferObjectRef;
typedef OSType                          TECPluginSig;
struct TECConversionInfo {
  TextEncoding        sourceEncoding;
  TextEncoding        destinationEncoding;
  UInt16              reserved1;
  UInt16              reserved2;
};
typedef struct TECConversionInfo        TECConversionInfo;
/* return number of encodings types supported by user's configuraton of the encoding converter */
/*
 *  TECCountAvailableTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountAvailableTextEncodings(ItemCount * numberEncodings);


/* fill in an array of type TextEncoding passed in by the user with types of encodings the current configuration of the encoder can handle. */
/*
 *  TECGetAvailableTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetAvailableTextEncodings(
  TextEncoding   availableEncodings[],
  ItemCount      maxAvailableEncodings,
  ItemCount *    actualAvailableEncodings);


/* return number of from-to encoding conversion pairs supported  */
/*
 *  TECCountDirectTextEncodingConversions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountDirectTextEncodingConversions(ItemCount * numberOfEncodings);


/* fill in an array of type TextEncodingPair passed in by the user with types of encoding pairs the current configuration of the encoder can handle. */
/*
 *  TECGetDirectTextEncodingConversions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetDirectTextEncodingConversions(
  TECConversionInfo   availableConversions[],
  ItemCount           maxAvailableConversions,
  ItemCount *         actualAvailableConversions);


/* return number of encodings a given encoding can be converter into */
/*
 *  TECCountDestinationTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountDestinationTextEncodings(
  TextEncoding   inputEncoding,
  ItemCount *    numberOfEncodings);


/* fill in an array of type TextEncodingPair passed in by the user with types of encodings pairs the current configuration of the encoder can handle. */
/*
 *  TECGetDestinationTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetDestinationTextEncodings(
  TextEncoding   inputEncoding,
  TextEncoding   destinationEncodings[],
  ItemCount      maxDestinationEncodings,
  ItemCount *    actualDestinationEncodings);


/* get info about a text encoding */
/*
 *  TECGetTextEncodingInternetName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetTextEncodingInternetName(
  TextEncoding   textEncoding,
  Str255         encodingName);


/*
 *  TECGetTextEncodingFromInternetName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetTextEncodingFromInternetName(
  TextEncoding *     textEncoding,
  ConstStr255Param   encodingName);


/* create/dispose converters */
/*
 *  TECCreateConverter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCreateConverter(
  TECObjectRef *  newEncodingConverter,
  TextEncoding    inputEncoding,
  TextEncoding    outputEncoding);


/*
 *  TECCreateConverterFromPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCreateConverterFromPath(
  TECObjectRef *       newEncodingConverter,
  const TextEncoding   inPath[],
  ItemCount            inEncodings);


/*
 *  TECDisposeConverter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECDisposeConverter(TECObjectRef newEncodingConverter);


/* convert text encodings */
/*
 *  TECClearConverterContextInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECClearConverterContextInfo(TECObjectRef encodingConverter);


/*
 *  TECConvertText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECConvertText(
  TECObjectRef   encodingConverter,
  ConstTextPtr   inputBuffer,
  ByteCount      inputBufferLength,
  ByteCount *    actualInputLength,
  TextPtr        outputBuffer,
  ByteCount      outputBufferLength,
  ByteCount *    actualOutputLength);


/*
 *  TECFlushText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECFlushText(
  TECObjectRef   encodingConverter,
  TextPtr        outputBuffer,
  ByteCount      outputBufferLength,
  ByteCount *    actualOutputLength);


/* one-to-many routines */
/*
 *  TECCountSubTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountSubTextEncodings(
  TextEncoding   inputEncoding,
  ItemCount *    numberOfEncodings);


/*
 *  TECGetSubTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetSubTextEncodings(
  TextEncoding   inputEncoding,
  TextEncoding   subEncodings[],
  ItemCount      maxSubEncodings,
  ItemCount *    actualSubEncodings);


/*
 *  TECGetEncodingList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetEncodingList(
  TECObjectRef   encodingConverter,
  ItemCount *    numEncodings,
  Handle *       encodingList);


/*
 *  TECCreateOneToManyConverter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCreateOneToManyConverter(
  TECObjectRef *       newEncodingConverter,
  TextEncoding         inputEncoding,
  ItemCount            numOutputEncodings,
  const TextEncoding   outputEncodings[]);


/*
 *  TECConvertTextToMultipleEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECConvertTextToMultipleEncodings(
  TECObjectRef      encodingConverter,
  ConstTextPtr      inputBuffer,
  ByteCount         inputBufferLength,
  ByteCount *       actualInputLength,
  TextPtr           outputBuffer,
  ByteCount         outputBufferLength,
  ByteCount *       actualOutputLength,
  TextEncodingRun   outEncodingsBuffer[],
  ItemCount         maxOutEncodingRuns,
  ItemCount *       actualOutEncodingRuns);


/*
 *  TECFlushMultipleEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECFlushMultipleEncodings(
  TECObjectRef      encodingConverter,
  TextPtr           outputBuffer,
  ByteCount         outputBufferLength,
  ByteCount *       actualOutputLength,
  TextEncodingRun   outEncodingsBuffer[],
  ItemCount         maxOutEncodingRuns,
  ItemCount *       actualOutEncodingRuns);


/* international internet info */
/*
 *  TECCountWebTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountWebTextEncodings(
  RegionCode   locale,
  ItemCount *  numberEncodings);


/*
 *  TECGetWebTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetWebTextEncodings(
  RegionCode     locale,
  TextEncoding   availableEncodings[],
  ItemCount      maxAvailableEncodings,
  ItemCount *    actualAvailableEncodings);


/*
 *  TECCountMailTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountMailTextEncodings(
  RegionCode   locale,
  ItemCount *  numberEncodings);


/*
 *  TECGetMailTextEncodings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetMailTextEncodings(
  RegionCode     locale,
  TextEncoding   availableEncodings[],
  ItemCount      maxAvailableEncodings,
  ItemCount *    actualAvailableEncodings);


/* examine text encodings */
/*
 *  TECCountAvailableSniffers()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCountAvailableSniffers(ItemCount * numberOfEncodings);


/*
 *  TECGetAvailableSniffers()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECGetAvailableSniffers(
  TextEncoding   availableSniffers[],
  ItemCount      maxAvailableSniffers,
  ItemCount *    actualAvailableSniffers);


/*
 *  TECCreateSniffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECCreateSniffer(
  TECSnifferObjectRef *  encodingSniffer,
  TextEncoding           testEncodings[],
  ItemCount              numTextEncodings);


/*
 *  TECSniffTextEncoding()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECSniffTextEncoding(
  TECSnifferObjectRef   encodingSniffer,
  TextPtr               inputBuffer,
  ByteCount             inputBufferLength,
  TextEncoding          testEncodings[],
  ItemCount             numTextEncodings,
  ItemCount             numErrsArray[],
  ItemCount             maxErrs,
  ItemCount             numFeaturesArray[],
  ItemCount             maxFeatures);


/*
 *  TECDisposeSniffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECDisposeSniffer(TECSnifferObjectRef encodingSniffer);


/*
 *  TECClearSnifferContextInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TECClearSnifferContextInfo(TECSnifferObjectRef encodingSniffer);


#if CALL_NOT_IN_CARBON
/*
 *  TECSetBasicOptions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in TextEncodingConverter 1.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSStatus )
TECSetBasicOptions(
  TECObjectRef   encodingConverter,
  OptionBits     controlFlags);


#endif  /* CALL_NOT_IN_CARBON */



#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TEXTENCODINGCONVERTER__ */

