/*
     File:       CGDataProvider.h
 
     Contains:   CoreGraphics data provider
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGDATAPROVIDER_H_
#define CGDATAPROVIDER_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CFURL__
#include <CFURL.h>
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

typedef struct CGDataProvider*          CGDataProviderRef;
typedef CALLBACK_API_C( size_t , CGGetBytesProcPtr )(void *info, void *buffer, size_t count);
typedef CALLBACK_API_C( void , CGSkipBytesProcPtr )(void *info, size_t count);
typedef CALLBACK_API_C( void , CGRewindProcPtr )(void * info);
typedef CALLBACK_API_C( void , CGReleaseProviderProcPtr )(void * info);
/* Callbacks for sequentially accessing data.
 * `getBytes' is called to copy `count' bytes from the provider's data to
 * `buffer'.  It should return the number of bytes copied, or 0 if there's
 * no more data.
 * `skipBytes' is called to skip ahead in the provider's data by `count' bytes.
 * `rewind' is called to rewind the provider to the beginning of the data.
 * `releaseProvider', if non-NULL, is called when the provider is freed. */
struct CGDataProviderCallbacks {
  CGGetBytesProcPtr   getBytes;
  CGSkipBytesProcPtr  skipBytes;
  CGRewindProcPtr     rewind;
  CGReleaseProviderProcPtr  releaseProvider;
};
typedef struct CGDataProviderCallbacks  CGDataProviderCallbacks;
typedef CALLBACK_API_C( void *, CGGetBytePointerProcPtr )(void * info);
typedef CALLBACK_API_C( void , CGReleaseByteProcPtr )(void *info, const void *pointer);
typedef CALLBACK_API_C( size_t , CGGetBytesDirectProcPtr )(void *info, void *buffer, size_t offset, size_t count);
/* Callbacks for directly accessing data.
 * `getBytePointer', if non-NULL, is called to return a pointer to the
 * provider's entire block of data.
 * `releaseBytePointer', if non-NULL, is called to release a pointer to
 * the provider's entire block of data.
 * `getBytes', if non-NULL, is called to copy `count' bytes at offset
 * `offset' from the provider's data to `buffer'.  It should return the
 * number of bytes copied, or 0 if there's no more data.
 * `releaseProvider', if non-NULL, is called when the provider is freed.
 * At least one of `getBytePointer' or `getBytes' must be non-NULL.  */
struct CGDataProviderDirectAccessCallbacks {
  CGGetBytePointerProcPtr  getBytePointer;
  CGReleaseByteProcPtr  releaseBytePointer;
  CGGetBytesDirectProcPtr  getBytes;
  CGReleaseProviderProcPtr  releaseProvider;
};
typedef struct CGDataProviderDirectAccessCallbacks CGDataProviderDirectAccessCallbacks;
typedef CALLBACK_API_C( void , CGReleaseDataProcPtr )(void *info, const void *data, size_t size);
/* Create a sequential-access data provider using `callbacks' to provide
 * the data.  `info' is passed to each of the callback functions. */
/*
 *  CGDataProviderCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDataProviderRef )
CGDataProviderCreate(
  void *                           info,
  const CGDataProviderCallbacks *  callbacks);


/* Create a direct-access data provider using `callbacks' to supply `size'
 * bytes of data. `info' is passed to each of the callback functions. */
/*
 *  CGDataProviderCreateDirectAccess()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDataProviderRef )
CGDataProviderCreateDirectAccess(
  void *                                       info,
  size_t                                       size,
  const CGDataProviderDirectAccessCallbacks *  callbacks);


/* Create a direct-access data provider using `data', an array of `size'
 * bytes.  `releaseData' is called when the data provider is freed, and is
 * passed `info' as its first argument. */
/*
 *  CGDataProviderCreateWithData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDataProviderRef )
CGDataProviderCreateWithData(
  void *                 info,
  const void *           data,
  size_t                 size,
  CGReleaseDataProcPtr   releaseData);


/* Create a data provider using `url'. */
/*
 *  CGDataProviderCreateWithURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDataProviderRef )
CGDataProviderCreateWithURL(CFURLRef url);


/* Increment the retain count of `provider' and return it.  All data
 * providers are created with an initial retain count of 1. */
/*
 *  CGDataProviderRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDataProviderRef )
CGDataProviderRetain(CGDataProviderRef provider);


/* Decrement the retain count of `provider'.  If the retain count reaches
 * 0, then free `provider' and any associated resources. */
/*
 *  CGDataProviderRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGDataProviderRelease(CGDataProviderRef provider);



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

#endif /* CGDATAPROVIDER_H_ */

