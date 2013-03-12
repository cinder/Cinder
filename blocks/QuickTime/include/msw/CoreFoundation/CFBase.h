/*	CFBase.h
	Copyright (c) 1998-2003, Apple, Inc. All rights reserved.
*/

#if !defined(__COREFOUNDATION_CFBASE__)
#define __COREFOUNDATION_CFBASE__ 1

#if CF_QUICKTIME
#include <ConditionalMacros.h>
#include "QTMLCompilerOptions.h"
#include "QTCFRenames.h"

#pragma warning(disable:4090)
#pragma warning(disable:4133)

#endif

// Look for various windows environments, boil them all down to defining __WIN32__.
// _WIN32 comes from MS tools.  When using MinGW __WIN32__ is already defined.
#if (defined(__CYGWIN32__) || defined(_WIN32) || (defined(__MWERKS__) && defined(__INTEL__))) && !defined (__WIN32__)
#define __WIN32__						1
#endif

#if defined(_MSC_VER) && defined(_M_IX86) && !defined(__i386__)
#define __i386__						1
#endif

#if TARGET_OS_WIN32
	#define CF_BUILDING_CF_AS_LIB		1
#endif

#if defined(__GNUC__) || defined(__MWERKS__)
	#include <stdint.h>
	#include <stdbool.h>
#else
	// mostly for the benefit of MSVC
	#include <GNUCompatibility/stdint.h>
	#include <GNUCompatibility/stdbool.h>
	#if TARGET_OS_WIN32
		#undef pascal
		#define pascal
		typedef void *HANDLE;
	#endif
#endif

#include <AvailabilityMacros.h>

	#include <MacTypes.h>

#if !defined(__MACTYPES__)
    typedef unsigned char           Boolean;
    typedef unsigned char           UInt8;
    typedef signed char             SInt8;
    typedef unsigned short          UInt16;
    typedef signed short            SInt16;
    typedef unsigned long           UInt32;
    typedef signed long             SInt32;
    typedef uint64_t		        UInt64;
    typedef int64_t		            SInt64;
    typedef float                   Float32;
    typedef double                  Float64;
    typedef unsigned short          UniChar;
    typedef unsigned char *         StringPtr;
    typedef const unsigned char *   ConstStringPtr;
    typedef unsigned char           Str255[256];
    typedef const unsigned char *   ConstStr255Param;
    typedef SInt16                  OSErr;
    typedef SInt32                  OSStatus;
#endif
#if !defined(__MACTYPES__) || (defined(UNIVERSAL_INTERFACES_VERSION) && UNIVERSAL_INTERFACES_VERSION < 0x0340)
    typedef UInt32                  UTF32Char;
    typedef UInt16                  UTF16Char;
    typedef UInt8                   UTF8Char;
#endif

#if defined(__CYGWIN32__) || defined (D__CYGWIN_)
#error CoreFoundation is currently built with the Microsoft C Runtime, which is incompatible with the Cygwin DLL.  You must either use the -mno-cygwin flag, or complete a port of CF to the Cygwin environment.
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(NULL)
    #define NULL	0
#endif

#if !defined(TRUE)
    #define TRUE	1
#endif

#if !defined(FALSE)
    #define FALSE	0
#endif

#if defined(__WIN32__)
    #undef CF_EXPORT
	#if defined(CF_BUILDING_CF_AS_LIB)
    // we're building CF as a library
	#define CF_EXPORT extern
	#elif defined(CF_BUILDING_CF)
    // we're building CF as a DLL
	#define CF_EXPORT __declspec(dllexport) extern
    #else
	#define CF_EXPORT __declspec(dllimport) extern
    #endif
#elif defined(macintosh)
    #if defined(__MWERKS__)
        #define CF_EXPORT __declspec(export) extern
    #endif
#endif

#if !defined(CF_EXPORT)
    #define CF_EXPORT extern
#endif

#if !defined(CF_INLINE)
    #if defined(__GNUC__)
	#define CF_INLINE static __inline__
    #elif defined(__MWERKS__) || defined(__cplusplus)
	#define CF_INLINE static inline
    #elif defined(_MSC_VER)
        #define CF_INLINE static __inline
    #elif defined(__WIN32__)
	#define CF_INLINE static __inline__
    #endif
#endif


CF_EXPORT double kCFCoreFoundationVersionNumber;

#define kCFCoreFoundationVersionNumber10_0 196.4
#define kCFCoreFoundationVersionNumber10_0_3 196.5
#if MAC_OS_X_VERSION_10_2 <= MAC_OS_X_VERSION_MAX_ALLOWED
#define kCFCoreFoundationVersionNumber10_1 226.0
/* Note these do not follow the usual numbering policy from the base release */
#define kCFCoreFoundationVersionNumber10_1_2 227.2
#define kCFCoreFoundationVersionNumber10_1_4 227.3
#endif
#if MAC_OS_X_VERSION_10_3 <= MAC_OS_X_VERSION_MAX_ALLOWED
#define kCFCoreFoundationVersionNumber10_2 263.0
#endif

typedef UInt32 CFTypeID;
typedef UInt32 CFOptionFlags;
typedef UInt32 CFHashCode;
typedef SInt32 CFIndex;

/* Base "type" of all "CF objects", and polymorphic functions on them */
typedef const void * CFTypeRef;

typedef const struct __CFString * CFStringRef;
typedef struct __CFString * CFMutableStringRef;

/*
        Type to mean any instance of a property list type;
        currently, CFString, CFData, CFNumber, CFBoolean, CFDate,
        CFArray, and CFDictionary.
*/
typedef CFTypeRef CFPropertyListRef;

/* Values returned from comparison functions */
typedef enum {
    kCFCompareLessThan = -1,
    kCFCompareEqualTo = 0,
    kCFCompareGreaterThan = 1
} CFComparisonResult;

/* A standard comparison function */
typedef CFComparisonResult (*CFComparatorFunction)(const void *val1, const void *val2, void *context);

/* Constant used by some functions to indicate failed searches. */
/* This is of type CFIndex. */
enum {
    kCFNotFound = -1
};


#if TARGET_OS_WIN32
CF_EXPORT
void *QTGetCFConstant(const char *cfGlobalConstantKey);
#endif // TARGET_OS_WIN32

/* Range type */
typedef struct {
    CFIndex location;
    CFIndex length;
} CFRange;

#if defined(CF_INLINE)
CF_INLINE CFRange CFRangeMake(CFIndex loc, CFIndex len) {
    CFRange range;
    range.location = loc;
    range.length = len;
    return range;
}
#else
#define CFRangeMake(LOC, LEN) __CFRangeMake(LOC, LEN)
#endif

/* Private; do not use */
CF_EXPORT
CFRange __CFRangeMake(CFIndex loc, CFIndex len);


#if MAC_OS_X_VERSION_10_2 <= MAC_OS_X_VERSION_MAX_ALLOWED
/* Null representant */

typedef const struct __CFNull * CFNullRef;

CF_EXPORT
CFTypeID CFNullGetTypeID(void);

#if TARGET_OS_WIN32
#define kCFNull (*((const CFNullRef *)QTGetCFConstant("kCFNull")))
#else
CF_EXPORT
const CFNullRef kCFNull;	// the singleton null instance
#endif

#endif


/* Allocator API

   Most of the time when specifying an allocator to Create functions, the NULL
   argument indicates "use the default"; this is the same as using kCFAllocatorDefault
   or the return value from CFAllocatorGetDefault().  This assures that you will use
   the allocator in effect at that time.

   You should rarely use kCFAllocatorSystemDefault, the default default allocator.
*/
typedef const struct __CFAllocator * CFAllocatorRef;

#if TARGET_OS_WIN32
#define kCFAllocatorDefault (*((const CFAllocatorRef *)QTGetCFConstant("kCFAllocatorDefault")))
#define kCFAllocatorSystemDefault (*((const CFAllocatorRef *)QTGetCFConstant("kCFAllocatorSystemDefault")))
#define kCFAllocatorMalloc (*((const CFAllocatorRef *)QTGetCFConstant("kCFAllocatorMalloc")))
#define kCFAllocatorNull (*((const CFAllocatorRef *)QTGetCFConstant("kCFAllocatorNull")))
#define kCFAllocatorUseContext (*((const CFAllocatorRef *)QTGetCFConstant("kCFAllocatorUseContext")))
#else
/* This is a synonym for NULL, if you'd rather use a named constant. */
CF_EXPORT
const CFAllocatorRef kCFAllocatorDefault;

/* Default system allocator; you rarely need to use this. */
CF_EXPORT
const CFAllocatorRef kCFAllocatorSystemDefault;

/* This allocator uses malloc(), realloc(), and free(). This should not be
   generally used; stick to kCFAllocatorDefault whenever possible. This
   allocator is useful as the "bytesDeallocator" in CFData or
   "contentsDeallocator" in CFString where the memory was obtained as a
   result of malloc() type functions.
*/
CF_EXPORT
const CFAllocatorRef kCFAllocatorMalloc;

/* Null allocator which does nothing and allocates no memory. This allocator
   is useful as the "bytesDeallocator" in CFData or "contentsDeallocator"
   in CFString where the memory should not be freed. 
*/
CF_EXPORT
const CFAllocatorRef kCFAllocatorNull;

/* Special allocator argument to CFAllocatorCreate() which means
   "use the functions given in the context to allocate the allocator
   itself as well". 
*/
CF_EXPORT
const CFAllocatorRef kCFAllocatorUseContext;
#endif

typedef const void *	(*CFAllocatorRetainCallBack)(const void *info);
typedef void		(*CFAllocatorReleaseCallBack)(const void *info);
typedef CFStringRef	(*CFAllocatorCopyDescriptionCallBack)(const void *info);
typedef void *		(*CFAllocatorAllocateCallBack)(CFIndex allocSize, CFOptionFlags hint, void *info);
typedef void *		(*CFAllocatorReallocateCallBack)(void *ptr, CFIndex newsize, CFOptionFlags hint, void *info);
typedef void		(*CFAllocatorDeallocateCallBack)(void *ptr, void *info);
typedef CFIndex		(*CFAllocatorPreferredSizeCallBack)(CFIndex size, CFOptionFlags hint, void *info);
typedef struct {
    CFIndex				version;
    void *				info;
    CFAllocatorRetainCallBack		retain;
    CFAllocatorReleaseCallBack		release;        
    CFAllocatorCopyDescriptionCallBack	copyDescription;
    CFAllocatorAllocateCallBack		allocate;
    CFAllocatorReallocateCallBack	reallocate;
    CFAllocatorDeallocateCallBack	deallocate;
    CFAllocatorPreferredSizeCallBack	preferredSize;
} CFAllocatorContext;

CF_EXPORT
CFTypeID	CFAllocatorGetTypeID(void);

/*
	CFAllocatorSetDefault() sets the allocator that is used in the current
	thread whenever NULL is specified as an allocator argument. This means
	that most, if not all allocations will go through this allocator. It
	also means that any allocator set as the default needs to be ready to
	deal with arbitrary memory allocation requests; in addition, the size
	and number of requests will change between releases.

	An allocator set as the default will never be released, even if later
	another allocator replaces it as the default. Not only is it impractical
	for it to be released (as there might be caches created under the covers
	that refer to the allocator), in general it's also safer and more
	efficient to keep it around.

	If you wish to use a custom allocator in a context, it's best to provide
	it as the argument to the various creation functions rather than setting
	it as the default. Setting the default allocator is not encouraged.

	If you do set an allocator as the default, either do it for all time in
	your app, or do it in a nested fashion (by restoring the previous allocator
	when you exit your context). The latter might be appropriate for plug-ins
	or libraries that wish to set the default allocator.
*/
CF_EXPORT
void CFAllocatorSetDefault(CFAllocatorRef allocator);

CF_EXPORT
CFAllocatorRef CFAllocatorGetDefault(void);

CF_EXPORT
CFAllocatorRef CFAllocatorCreate(CFAllocatorRef allocator, CFAllocatorContext *context);

CF_EXPORT
void *CFAllocatorAllocate(CFAllocatorRef allocator, CFIndex size, CFOptionFlags hint);

CF_EXPORT
void *CFAllocatorReallocate(CFAllocatorRef allocator, void *ptr, CFIndex newsize, CFOptionFlags hint);

CF_EXPORT
void CFAllocatorDeallocate(CFAllocatorRef allocator, void *ptr);

CF_EXPORT
CFIndex CFAllocatorGetPreferredSizeForSize(CFAllocatorRef allocator, CFIndex size, CFOptionFlags hint);

CF_EXPORT
void CFAllocatorGetContext(CFAllocatorRef allocator, CFAllocatorContext *context);


/* Polymorphic CF functions */

CF_EXPORT
CFTypeID CFGetTypeID(CFTypeRef cf);

CF_EXPORT
CFStringRef CFCopyTypeIDDescription(CFTypeID type_id);

CF_EXPORT
CFTypeRef CFRetain(CFTypeRef cf);

CF_EXPORT
void CFRelease(CFTypeRef cf);

CF_EXPORT
CFIndex CFGetRetainCount(CFTypeRef cf);

CF_EXPORT
Boolean CFEqual(CFTypeRef cf1, CFTypeRef cf2);

CF_EXPORT
CFHashCode CFHash(CFTypeRef cf);

CF_EXPORT
CFStringRef CFCopyDescription(CFTypeRef cf);

CF_EXPORT
CFAllocatorRef CFGetAllocator(CFTypeRef cf);

#if defined(__cplusplus)
}
#endif

#endif /* ! __COREFOUNDATION_CFBASE__ */

