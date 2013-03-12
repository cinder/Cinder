/*
     File:       AEHelpers.h
 
     Contains:   AEPrint, AEBuild and AEStream for Carbon
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
/*
 * Originally from AEGIzmos by Jens Alfke, circa 1992.
 */
#ifndef __AEHELPERS__
#define __AEHELPERS__

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#include <stdarg.h>


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

/*
 * AEBuild:
 *
 * AEBuild provides a very high level abstraction for building
 * complete AppleEvents and complex ObjectSpeciers.  Using AEBuild it
 * is easy to produce a textual representation of an AEDesc.  The
 * format is similar to the stdio printf call, where meta data is
 * extracted from a format string and used to build the final
 * representation.
 * 
 * The structure of the format string is described here:
 * 
 *          < describe it >
 */
/* Syntax Error Codes: */
typedef UInt32 AEBuildErrorCode;
enum {
  aeBuildSyntaxNoErr            = 0,    /* (No error) */
  aeBuildSyntaxBadToken         = 1,    /* Illegal character */
  aeBuildSyntaxBadEOF           = 2,    /* Unexpected end of format string */
  aeBuildSyntaxNoEOF            = 3,    /* Unexpected extra stuff past end */
  aeBuildSyntaxBadNegative      = 4,    /* "-" not followed by digits */
  aeBuildSyntaxMissingQuote     = 5,    /* Missing close "'" */
  aeBuildSyntaxBadHex           = 6,    /* Non-digit in hex string */
  aeBuildSyntaxOddHex           = 7,    /* Odd # of hex digits */
  aeBuildSyntaxNoCloseHex       = 8,    /* Missing "." */
  aeBuildSyntaxUncoercedHex     = 9,    /* Hex string must be coerced to a type */
  aeBuildSyntaxNoCloseString    = 10,   /* Missing """ */
  aeBuildSyntaxBadDesc          = 11,   /* Illegal descriptor */
  aeBuildSyntaxBadData          = 12,   /* Bad data value inside (...) */
  aeBuildSyntaxNoCloseParen     = 13,   /* Missing ")" after data value */
  aeBuildSyntaxNoCloseBracket   = 14,   /* Expected "," or "]" */
  aeBuildSyntaxNoCloseBrace     = 15,   /* Expected "," or "}" */
  aeBuildSyntaxNoKey            = 16,   /* Missing keyword in record */
  aeBuildSyntaxNoColon          = 17,   /* Missing ":" after keyword in record */
  aeBuildSyntaxCoercedList      = 18,   /* Cannot coerce a list */
  aeBuildSyntaxUncoercedDoubleAt = 19   /* "@@" substitution must be coerced */
};

/* A structure containing error state.*/

struct AEBuildError {
  AEBuildErrorCode    fError;
  UInt32              fErrorPos;
};
typedef struct AEBuildError             AEBuildError;
/*
   Create an AEDesc from the format string.  AEBuildError can be NULL, in which case
   no explicit error information will be returned.
*/
/*
 *  AEBuildDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEBuildDesc(
  AEDesc *        dst,
  AEBuildError *  error,       /* can be NULL */
  const char *    src,
  ...);


/* varargs version of AEBuildDesc*/
/*
 *  vAEBuildDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
vAEBuildDesc(
  AEDesc *        dst,
  AEBuildError *  error,       /* can be NULL */
  const char *    src,
  va_list         args);



/* Append parameters to an existing AppleEvent*/
/*
 *  AEBuildParameters()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEBuildParameters(
  AppleEvent *    event,
  AEBuildError *  error,        /* can be NULL */
  const char *    format,
  ...);


/* varargs version of AEBuildParameters*/
/*
 *  vAEBuildParameters()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
vAEBuildParameters(
  AppleEvent *    event,
  AEBuildError *  error,        /* can be NULL */
  const char *    format,
  va_list         args);


/* Building an entire Apple event:*/
/*
 *  AEBuildAppleEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEBuildAppleEvent(
  AEEventClass    theClass,
  AEEventID       theID,
  DescType        addressType,
  const void *    addressData,
  long            addressLength,
  short           returnID,
  long            transactionID,
  AppleEvent *    result,
  AEBuildError *  error,               /* can be NULL */
  const char *    paramsFmt,
  ...);


/* varargs version of AEBuildAppleEvent*/
/*
 *  vAEBuildAppleEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
vAEBuildAppleEvent(
  AEEventClass    theClass,
  AEEventID       theID,
  DescType        addressType,
  const void *    addressData,
  long            addressLength,
  short           returnID,
  long            transactionID,
  AppleEvent *    resultEvt,
  AEBuildError *  error,               /* can be NULL */
  const char *    paramsFmt,
  va_list         args);


/*
 * AEPrintDescToHandle
 *
 * AEPrintDescToHandle provides a way to turn an AEDesc into a textual
 * representation.  This is most useful for debugging calls to
 * AEBuildDesc and friends.  The Handle returned should be disposed by
 * the caller.  The size of the handle is the actual number of
 * characters in the string.
 */
/*
 *  AEPrintDescToHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEPrintDescToHandle(
  const AEDesc *  desc,
  Handle *        result);


/*
 * AEStream:
 *
 * The AEStream interface allows you to build AppleEvents by appending
 * to an opaque structure (an AEStreamRef) and then turning this
 * structure into an AppleEvent.  The basic idea is to open the
 * stream, write data, and then close it - closing it produces an
 * AEDesc, which may be partially complete, or may be a complete
 * AppleEvent.
 */
typedef struct OpaqueAEStreamRef*       AEStreamRef;
/*
   Create and return an AEStreamRef
   Returns NULL on memory allocation failure
*/
/*
 *  AEStreamOpen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AEStreamRef )
AEStreamOpen(void);


/*
   Closes and disposes of an AEStreamRef, producing
   results in the desc.  You must dispose of the desc yourself.
   If you just want to dispose of the AEStreamRef, you can pass NULL for desc.
*/
/*
 *  AEStreamClose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamClose(
  AEStreamRef   ref,
  AEDesc *      desc);


/*
   Prepares an AEStreamRef for appending data to a newly created desc.
   You append data with AEStreamWriteData
*/
/*
 *  AEStreamOpenDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamOpenDesc(
  AEStreamRef   ref,
  DescType      newType);


/* Append data to the previously opened desc.*/
/*
 *  AEStreamWriteData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamWriteData(
  AEStreamRef   ref,
  const void *  data,
  Size          length);


/*
   Finish a desc.  After this, you can close the stream, or adding new
   descs, if you're assembling a list.
*/
/*
 *  AEStreamCloseDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamCloseDesc(AEStreamRef ref);


/* Write data as a desc to the stream*/
/*
 *  AEStreamWriteDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamWriteDesc(
  AEStreamRef   ref,
  DescType      newType,
  const void *  data,
  Size          length);


/* Write an entire desc to the stream*/
/*
 *  AEStreamWriteAEDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamWriteAEDesc(
  AEStreamRef     ref,
  const AEDesc *  desc);


/*
   Begin a list.  You can then append to the list by doing
   AEStreamOpenDesc, or AEStreamWriteDesc.
*/
/*
 *  AEStreamOpenList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamOpenList(AEStreamRef ref);


/* Finish a list.*/
/*
 *  AEStreamCloseList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamCloseList(AEStreamRef ref);


/*
   Begin a record.  A record usually has type 'reco', however, this is
   rather generic, and frequently a different type is used.
*/
/*
 *  AEStreamOpenRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamOpenRecord(
  AEStreamRef   ref,
  DescType      newType);


/* Change the type of a record.*/
/*
 *  AEStreamSetRecordType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamSetRecordType(
  AEStreamRef   ref,
  DescType      newType);


/* Finish a record*/
/*
 *  AEStreamCloseRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamCloseRecord(AEStreamRef ref);


/*
   Add a keyed descriptor to a record.  This is analogous to AEPutParamDesc.
   it can only be used when writing to a record.
*/
/*
 *  AEStreamWriteKeyDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamWriteKeyDesc(
  AEStreamRef   ref,
  AEKeyword     key,
  DescType      newType,
  const void *  data,
  Size          length);


/*
   OpenDesc for a keyed record entry.  You can use AEStreamWriteData
   after opening a keyed desc.
*/
/*
 *  AEStreamOpenKeyDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamOpenKeyDesc(
  AEStreamRef   ref,
  AEKeyword     key,
  DescType      newType);


/* Write a key to the stream - you can follow this with an AEWriteDesc.*/
/*
 *  AEStreamWriteKey()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamWriteKey(
  AEStreamRef   ref,
  AEKeyword     key);


/*
   Create a complete AppleEvent.  This creates and returns a new stream.
   Use this call to populate the meta fields in an AppleEvent record.
   After this, you can add your records, lists and other parameters.
*/
/*
 *  AEStreamCreateEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AEStreamRef )
AEStreamCreateEvent(
  AEEventClass   clazz,
  AEEventID      id,
  DescType       targetType,
  const void *   targetData,
  long           targetLength,
  short          returnID,
  long           transactionID);


/*
   This call lets you augment an existing AppleEvent using the stream
   APIs.  This would be useful, for example, in constructing the reply
   record in an AppleEvent handler.  Note that AEStreamOpenEvent will
   consume the AppleEvent passed in - you can't access it again until the
   stream is closed.  When you're done building the event, AEStreamCloseStream
    will reconstitute it.
*/
/*
 *  AEStreamOpenEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AEStreamRef )
AEStreamOpenEvent(AppleEvent * event);


/* Mark a keyword as being an optional parameter.*/
/*
 *  AEStreamOptionalParam()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEStreamOptionalParam(
  AEStreamRef   ref,
  AEKeyword     key);



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

#endif /* __AEHELPERS__ */

