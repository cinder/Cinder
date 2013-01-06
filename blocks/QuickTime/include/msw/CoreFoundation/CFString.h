/*	CFString.h
	Copyright (c) 1998-2003, Apple, Inc. All rights reserved.
*/

#if !defined(__COREFOUNDATION_CFSTRING__)
#define __COREFOUNDATION_CFSTRING__ 1

#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFCharacterSet.h>
#include <CoreFoundation/CFLocale.h>
#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
Please note: CFStrings are conceptually an array of Unicode characters.
However, in general, how a CFString stores this array is an implementation
detail. For instance, CFString might choose to use an array of 8-bit characters;
to store its contents; or it might use multiple blocks of memory; or whatever.
Furthermore, the implementation might change depending on the default
system encoding, the user's language, the OS, or even a given release.

What this means is that you should use the following advanced functions with care:

  CFStringGetPascalStringPtr()
  CFStringGetCStringPtr()
  CFStringGetCharactersPtr()

These functions are provided for optimization only. They will either return the desired
pointer quickly, in constant time, or they return NULL. They might choose to return NULL
for many reasons; for instance it's possible that for users running in different
languages these sometimes return NULL; or in a future OS release the first two might
switch to always returning NULL. Never observing NULL returns in your usages of these
functions does not mean they won't ever return NULL. (But note the CFStringGetCharactersPtr()
exception mentioned further below.)

In your usages of these functions, if you get a NULL return, use the non-Ptr version
of the functions as shown in this example:

  Str255 buffer;
  StringPtr ptr = CFStringGetPascalStringPtr(str, encoding);
  if (ptr == NULL) {
      if (CFStringGetPascalString(str, buffer, 256, encoding)) ptr = buffer;
  }

Note that CFStringGetPascalString() or CFStringGetCString() calls might still fail --- but
that will happen in two circumstances only: The conversion from the UniChar contents of CFString
to the specified encoding fails, or the buffer is too small. If they fail, that means
the conversion was not possible.

If you need a copy of the buffer in the above example, you might consider simply
calling CFStringGetPascalString() in all cases --- CFStringGetPascalStringPtr()
is simply an optimization.

In addition, the following functions, which create immutable CFStrings from developer
supplied buffers without copying the buffers, might have to actually copy
under certain circumstances (If they do copy, the buffer will be dealt with by the
"contentsDeallocator" argument.):

  CFStringCreateWithPascalStringNoCopy()
  CFStringCreateWithCStringNoCopy()
  CFStringCreateWithCharactersNoCopy()

You should of course never depend on the backing store of these CFStrings being
what you provided, and in other no circumstance should you change the contents
of that buffer (given that would break the invariant about the CFString being immutable).

Having said all this, there are actually ways to create a CFString where the backing store
is external, and can be manipulated by the developer or CFString itself:

  CFStringCreateMutableWithExternalCharactersNoCopy()
  CFStringSetExternalCharactersNoCopy()

A "contentsAllocator" is used to realloc or free the backing store by CFString.
kCFAllocatorNull can be provided to assure CFString will never realloc or free the buffer.
Developer can call CFStringSetExternalCharactersNoCopy() to update
CFString's idea of what's going on, if the buffer is changed externally. In these
strings, CFStringGetCharactersPtr() is guaranteed to return the external buffer.

These functions are here to allow wrapping a buffer of UniChar characters in a CFString,
allowing the buffer to passed into CFString functions and also manipulated via CFString
mutation functions. In general, developers should not use this technique for all strings,
as it prevents CFString from using certain optimizations.
*/

/* Identifier for character encoding; the values are the same as Text Encoding Converter TextEncoding.
*/
typedef UInt32 CFStringEncoding;

/* Platform-independent built-in encodings; always available on all platforms.
   Call CFStringGetSystemEncoding() to get the default system encoding.
*/
#define kCFStringEncodingInvalidId (0xffffffffU)
typedef enum {
    kCFStringEncodingMacRoman = 0,
    kCFStringEncodingWindowsLatin1 = 0x0500, /* ANSI codepage 1252 */
    kCFStringEncodingISOLatin1 = 0x0201, /* ISO 8859-1 */
    kCFStringEncodingNextStepLatin = 0x0B01, /* NextStep encoding*/
    kCFStringEncodingASCII = 0x0600, /* 0..127 (in creating CFString, values greater than 0x7F are treated as corresponding Unicode value) */
    kCFStringEncodingUnicode = 0x0100, /* kTextEncodingUnicodeDefault  + kTextEncodingDefaultFormat (aka kUnicode16BitFormat) */
    kCFStringEncodingUTF8 = 0x08000100, /* kTextEncodingUnicodeDefault + kUnicodeUTF8Format */
    kCFStringEncodingNonLossyASCII = 0x0BFF /* 7bit Unicode variants used by Cocoa & Java */
} CFStringBuiltInEncodings;

/* CFString type ID */
CF_EXPORT
CFTypeID CFStringGetTypeID(void);

/* Macro to allow creation of compile-time constant strings; the argument should be a constant string.

CFSTR(), not being a "Copy" or "Create" function, does not return a new
reference for you. So, you should not release the return value. This is
much like constant C or Pascal strings --- when you use "hello world"
in a program, you do not free it.

However, strings returned from CFSTR() can be retained and released in a
properly nested fashion, just like any other CF type. That is, if you pass
a CFSTR() return value to a function such as SetMenuItemWithCFString(), the
function can retain it, then later, when it's done with it, it can release it.

At this point non-7 bit characters (that is, characters > 127) in CFSTR() are not 
supported and using them will lead to unpredictable results. This includes escaped
(\nnn) characters whose values are > 127. Even if it works for you in testing, 
it might not work for a user with a different language preference.
*/
#ifdef __CONSTANT_CFSTRINGS__
#define CFSTR(cStr)  ((CFStringRef) __builtin___CFStringMakeConstantString ("" cStr ""))
#else
#define CFSTR(cStr)  __CFStringMakeConstantString("" cStr "")
#endif

/*** Immutable string creation functions ***/

/* Functions to create basic immutable strings. The provided allocator is used for all memory activity in these functions.
*/

/* These functions copy the provided buffer into CFString's internal storage. */
CF_EXPORT
CFStringRef CFStringCreateWithPascalString(CFAllocatorRef alloc, ConstStr255Param pStr, CFStringEncoding encoding);

CF_EXPORT
CFStringRef CFStringCreateWithCString(CFAllocatorRef alloc, const char *cStr, CFStringEncoding encoding);

CF_EXPORT
CFStringRef CFStringCreateWithCharacters(CFAllocatorRef alloc, const UniChar *chars, CFIndex numChars);

/* These functions try not to copy the provided buffer. The buffer will be deallocated 
with the provided contentsDeallocator when it's no longer needed; to not free
the buffer, specify kCFAllocatorNull here. As usual, NULL means default allocator.

NOTE: Do not count on these buffers as being used by the string; 
in some cases the CFString might free the buffer and use something else
(for instance if it decides to always use Unicode encoding internally). 

NOTE: If you are not transferring ownership of the buffer to the CFString
(for instance, you supplied contentsDeallocator = kCFAllocatorNull), it is your
responsibility to assure the buffer does not go away during the lifetime of the string.
If the string is retained or copied, its lifetime might extend in ways you cannot
predict. So, for strings created with buffers whose lifetimes you cannot
guarantee, you need to be extremely careful --- do not hand it out to any
APIs which might retain or copy the strings.
*/
CF_EXPORT
CFStringRef CFStringCreateWithPascalStringNoCopy(CFAllocatorRef alloc, ConstStr255Param pStr, CFStringEncoding encoding, CFAllocatorRef contentsDeallocator);

CF_EXPORT
CFStringRef CFStringCreateWithCStringNoCopy(CFAllocatorRef alloc, const char *cStr, CFStringEncoding encoding, CFAllocatorRef contentsDeallocator);

CF_EXPORT
CFStringRef CFStringCreateWithCharactersNoCopy(CFAllocatorRef alloc, const UniChar *chars, CFIndex numChars, CFAllocatorRef contentsDeallocator);

/* Create copies of part or all of the string.
*/
CF_EXPORT
CFStringRef CFStringCreateWithSubstring(CFAllocatorRef alloc, CFStringRef str, CFRange range);

CF_EXPORT
CFStringRef CFStringCreateCopy(CFAllocatorRef alloc, CFStringRef theString);

/* These functions create a CFString from the provided printf-like format string and arguments.
*/
CF_EXPORT
CFStringRef CFStringCreateWithFormat(CFAllocatorRef alloc, CFDictionaryRef formatOptions, CFStringRef format, ...);

CF_EXPORT
CFStringRef CFStringCreateWithFormatAndArguments(CFAllocatorRef alloc, CFDictionaryRef formatOptions, CFStringRef format, va_list arguments);

/* Functions to create mutable strings. "maxLength", if not 0, is a hard bound on the length of the string. If 0, there is no limit on the length.
*/
CF_EXPORT
CFMutableStringRef CFStringCreateMutable(CFAllocatorRef alloc, CFIndex maxLength);

CF_EXPORT
CFMutableStringRef CFStringCreateMutableCopy(CFAllocatorRef alloc, CFIndex maxLength, CFStringRef theString);

/* This function creates a mutable string that has a developer supplied and directly editable backing store.
The string will be manipulated within the provided buffer (if any) until it outgrows capacity; then the
externalCharactersAllocator will be consulted for more memory. When the CFString is deallocated, the
buffer will be freed with the externalCharactersAllocator. Provide kCFAllocatorNull here to prevent the buffer
from ever being reallocated or deallocated by CFString. See comments at top of this file for more info.
*/
CF_EXPORT
CFMutableStringRef CFStringCreateMutableWithExternalCharactersNoCopy(CFAllocatorRef alloc, UniChar *chars, CFIndex numChars, CFIndex capacity, CFAllocatorRef externalCharactersAllocator);

/*** Basic accessors for the contents ***/

/* Number of 16-bit Unicode characters in the string.
*/
CF_EXPORT
CFIndex CFStringGetLength(CFStringRef theString);

/* Extracting the contents of the string. For obtaining multiple characters, calling
CFStringGetCharacters() is more efficient than multiple calls to CFStringGetCharacterAtIndex().
If the length of the string is not known (so you can't use a fixed size buffer for CFStringGetCharacters()),
another method is to use is CFStringGetCharacterFromInlineBuffer() (see further below).
*/
CF_EXPORT
UniChar CFStringGetCharacterAtIndex(CFStringRef theString, CFIndex idx);

CF_EXPORT
void CFStringGetCharacters(CFStringRef theString, CFRange range, UniChar *buffer);

/*** Conversion to other encodings ***/

/* These two convert into the provided buffer; they return false if conversion isn't possible
(due to conversion error, or not enough space in the provided buffer). 
These functions do zero-terminate or put the length byte; the provided bufferSize should include
space for this (so pass 256 for Str255). More sophisticated usages can go through CFStringGetBytes().
These functions are equivalent to calling CFStringGetBytes() with 
the range of the string; lossByte = 0; and isExternalRepresentation = false; 
if successful, they then insert the leading length of terminating zero, as desired.
*/
CF_EXPORT
Boolean CFStringGetPascalString(CFStringRef theString, StringPtr buffer, CFIndex bufferSize, CFStringEncoding encoding);

CF_EXPORT
Boolean CFStringGetCString(CFStringRef theString, char *buffer, CFIndex bufferSize, CFStringEncoding encoding);

/* These functions attempt to return in O(1) time the desired format for the string.
Note that although this means a pointer to the internal structure is being returned,
this can't always be counted on. Please see note at the top of the file for more
details.
*/
CF_EXPORT
ConstStringPtr CFStringGetPascalStringPtr(CFStringRef theString, CFStringEncoding encoding);	/* May return NULL at any time; be prepared for NULL */

CF_EXPORT
const char *CFStringGetCStringPtr(CFStringRef theString, CFStringEncoding encoding);		/* May return NULL at any time; be prepared for NULL */

CF_EXPORT
const UniChar *CFStringGetCharactersPtr(CFStringRef theString);					/* May return NULL at any time; be prepared for NULL */

/* The primitive conversion routine; allows you to convert a string piece at a time
   into a fixed size buffer. Returns number of characters converted. 
   Characters that cannot be converted to the specified encoding are represented
   with the byte specified by lossByte; if lossByte is 0, then lossy conversion
   is not allowed and conversion stops, returning partial results.
   Pass buffer==NULL if you don't care about the converted string (but just the convertability,
   or number of bytes required). 
   maxBufLength indicates the maximum number of bytes to generate. It is ignored when buffer==NULL.
   Does not zero-terminate. If you want to create Pascal or C string, allow one extra byte at start or end. 
   Setting isExternalRepresentation causes any extra bytes that would allow 
   the data to be made persistent to be included; for instance, the Unicode BOM.
*/
CF_EXPORT
CFIndex CFStringGetBytes(CFStringRef theString, CFRange range, CFStringEncoding encoding, UInt8 lossByte, Boolean isExternalRepresentation, UInt8 *buffer, CFIndex maxBufLen, CFIndex *usedBufLen);

/* This one goes the other way by creating a CFString from a bag of bytes.
This is much like CFStringCreateWithPascalString or CFStringCreateWithCString,
except the length is supplied explicitly. In addition, you can specify whether
the data is an external format --- that is, whether to pay attention to the
BOM character (if any) and do byte swapping if necessary
*/
CF_EXPORT
CFStringRef CFStringCreateWithBytes(CFAllocatorRef alloc, const UInt8 *bytes, CFIndex numBytes, CFStringEncoding encoding, Boolean isExternalRepresentation);

/* Convenience functions String <-> Data. These generate "external" formats, that is, formats that
   can be written out to disk. For instance, if the encoding is Unicode, CFStringCreateFromExternalRepresentation()
   pays attention to the BOM character (if any) and does byte swapping if necessary.
   Similarly CFStringCreateExternalRepresentation() will always include a BOM character if the encoding is
   Unicode. See above for description of lossByte.
*/
CF_EXPORT
CFStringRef CFStringCreateFromExternalRepresentation(CFAllocatorRef alloc, CFDataRef data, CFStringEncoding encoding);	/* May return NULL on conversion error */

CF_EXPORT
CFDataRef CFStringCreateExternalRepresentation(CFAllocatorRef alloc, CFStringRef theString, CFStringEncoding encoding, UInt8 lossByte);	/* May return NULL on conversion error */	

/* Hints about the contents of a string
*/
CF_EXPORT
CFStringEncoding CFStringGetSmallestEncoding(CFStringRef theString);	/* Result in O(n) time max */

CF_EXPORT
CFStringEncoding CFStringGetFastestEncoding(CFStringRef theString);	/* Result in O(1) time max */

/* General encoding info
*/
CF_EXPORT
CFStringEncoding CFStringGetSystemEncoding(void);		/* The default encoding for the system; untagged 8-bit characters are usually in this encoding */

CF_EXPORT
CFIndex CFStringGetMaximumSizeForEncoding(CFIndex length, CFStringEncoding encoding);	/* Max bytes a string of specified length (in UniChars) will take up if encoded */

/*** Comparison functions. ***/

/* Find and compare flags; these are OR'ed together as compareOptions or searchOptions in the various functions. 
   This typedef doesn't appear in the functions; instead the argument is CFOptionFlags. 
*/
typedef enum {	
    kCFCompareCaseInsensitive = 1,	
    kCFCompareBackwards = 4,		/* Starting from the end of the string */
    kCFCompareAnchored = 8,		/* Only at the specified starting point */
    kCFCompareNonliteral = 16,		/* If specified, loose equivalence is performed (o-umlaut == o, umlaut) */
    kCFCompareLocalized = 32,		/* User's default locale is used for the comparisons */
    kCFCompareNumerically = 64		/* Numeric comparison is used; that is, Foo2.txt < Foo7.txt < Foo25.txt */
} CFStringCompareFlags;	

/* The main comparison routine; compares specified range of the first string to (the full range of) the second string.
   locale == NULL indicates canonical locale.
   kCFCompareNumerically, added in 10.2, does not work if kCFCompareLocalized is specified on systems before 10.3
   kCFCompareBackwards and kCFCompareAnchored are not applicable.
*/
CF_EXPORT
CFComparisonResult CFStringCompareWithOptions(CFStringRef theString1, CFStringRef theString2, CFRange rangeToCompare, CFOptionFlags compareOptions);

/* Comparison convenience suitable for passing as sorting functions.
   kCFCompareNumerically, added in 10.2, does not work if kCFCompareLocalized is specified on systems before 10.3
   kCFCompareBackwards and kCFCompareAnchored are not applicable.
*/
CF_EXPORT
CFComparisonResult CFStringCompare(CFStringRef theString1, CFStringRef theString2, CFOptionFlags compareOptions);

/* CFStringFindWithOptions() returns the found range in the CFRange * argument; you can pass NULL for simple discovery check.
   If stringToFind is the empty string (zero length), nothing is found.
   Ignores the kCFCompareNumerically option.
*/
CF_EXPORT
Boolean CFStringFindWithOptions(CFStringRef theString, CFStringRef stringToFind, CFRange rangeToSearch, CFOptionFlags searchOptions, CFRange *result);

/* CFStringCreateArrayWithFindResults() returns an array of CFRange pointers, or NULL if there are no matches.
   Overlapping instances are not found; so looking for "AA" in "AAA" finds just one range.
   Post 10.1: If kCFCompareBackwards is provided, the scan is done from the end (which can give a different result), and
      the results are stored in the array backwards (last found range in slot 0).
   If stringToFind is the empty string (zero length), nothing is found.
   kCFCompareAnchored causes just the consecutive instances at start (or end, if kCFCompareBackwards) to be reported. So, searching for "AB" in "ABABXAB..." you just get the first two occurrences.
   Ignores the kCFCompareNumerically option.
*/
CF_EXPORT
CFArrayRef CFStringCreateArrayWithFindResults(CFAllocatorRef alloc, CFStringRef theString, CFStringRef stringToFind, CFRange rangeToSearch, CFOptionFlags compareOptions);

/* Find conveniences; see comments above concerning empty string and options.
*/
CF_EXPORT
CFRange CFStringFind(CFStringRef theString, CFStringRef stringToFind, CFOptionFlags compareOptions);

CF_EXPORT
Boolean CFStringHasPrefix(CFStringRef theString, CFStringRef prefix);

CF_EXPORT
Boolean CFStringHasSuffix(CFStringRef theString, CFStringRef suffix);

#if MAC_OS_X_VERSION_10_2 <= MAC_OS_X_VERSION_MAX_ALLOWED
/*!
	@function CFStringGetRangeOfComposedCharactersAtIndex
	Returns the range of the composed character sequence at the specified index.
	@param theString The CFString which is to be searched.  If this
                		parameter is not a valid CFString, the behavior is
              		undefined.
	@param theIndex The index of the character contained in the
			composed character sequence.  If the index is
			outside the index space of the string (0 to N-1 inclusive,
			where N is the length of the string), the behavior is
			undefined.
	@result The range of the composed character sequence.
*/
CF_EXPORT CFRange CFStringGetRangeOfComposedCharactersAtIndex(CFStringRef theString, CFIndex theIndex);

/*!
	@function CFStringFindCharacterFromSet
	Query the range of the first character contained in the specified character set.
	@param theString The CFString which is to be searched.  If this
                		parameter is not a valid CFString, the behavior is
              		undefined.
	@param theSet The CFCharacterSet against which the membership
			of characters is checked.  If this parameter is not a valid
			CFCharacterSet, the behavior is undefined.
	@param range The range of characters within the string to search. If
			the range location or end point (defined by the location
			plus length minus 1) are outside the index space of the
			string (0 to N-1 inclusive, where N is the length of the
			string), the behavior is undefined. If the range length is
			negative, the behavior is undefined. The range may be empty
			(length 0), in which case no search is performed.
	@param searchOptions The bitwise-or'ed option flags to control
			the search behavior.  The supported options are
			kCFCompareBackwards andkCFCompareAnchored.
			If other option flags are specified, the behavior
                        is undefined.
	@param result The pointer to a CFRange supplied by the caller in
			which the search result is stored.  Note that the length
                        of this range could be more than If a pointer to an invalid
			memory is specified, the behavior is undefined.
	@result true, if at least a character which is a member of the character
			set is found and result is filled, otherwise, false.
*/
CF_EXPORT Boolean CFStringFindCharacterFromSet(CFStringRef theString, CFCharacterSetRef theSet, CFRange rangeToSearch, CFOptionFlags searchOptions, CFRange *result);
#endif

/* Find range of bounds of the line(s) that span the indicated range (startIndex, numChars),
   taking into account various possible line separator sequences (CR, CRLF, LF, and Unicode LS, PS).
   All return values are "optional" (provide NULL if you don't want them)
     lineStartIndex: index of first character in line
     lineEndIndex: index of first character of the next line (including terminating line separator characters)
     contentsEndIndex: index of the first line separator character
   Thus, lineEndIndex - lineStartIndex is the number of chars in the line, including the line separators
         contentsEndIndex - lineStartIndex is the number of chars in the line w/out the line separators
*/
CF_EXPORT
void CFStringGetLineBounds(CFStringRef theString, CFRange range, CFIndex *lineBeginIndex, CFIndex *lineEndIndex, CFIndex *contentsEndIndex); 

/*** Exploding and joining strings with a separator string ***/

CF_EXPORT
CFStringRef CFStringCreateByCombiningStrings(CFAllocatorRef alloc, CFArrayRef theArray, CFStringRef separatorString);	/* Empty array returns empty string; one element array returns the element */

CF_EXPORT
CFArrayRef CFStringCreateArrayBySeparatingStrings(CFAllocatorRef alloc, CFStringRef theString, CFStringRef separatorString);	/* No separators in the string returns array with that string; string == sep returns two empty strings */

/*** Parsing non-localized numbers from strings ***/

CF_EXPORT
SInt32 CFStringGetIntValue(CFStringRef str);		/* Skips whitespace; returns 0 on error, MAX or -MAX on overflow */

CF_EXPORT
double CFStringGetDoubleValue(CFStringRef str);	/* Skips whitespace; returns 0.0 on error */

/*** MutableString functions ***/

/* CFStringAppend("abcdef", "xxxxx") -> "abcdefxxxxx"
   CFStringDelete("abcdef", CFRangeMake(2, 3)) -> "abf"
   CFStringReplace("abcdef", CFRangeMake(2, 3), "xxxxx") -> "abxxxxxf"
   CFStringReplaceAll("abcdef", "xxxxx") -> "xxxxx"
*/
CF_EXPORT
void CFStringAppend(CFMutableStringRef theString, CFStringRef appendedString);

CF_EXPORT
void CFStringAppendCharacters(CFMutableStringRef theString, const UniChar *chars, CFIndex numChars);

CF_EXPORT
void CFStringAppendPascalString(CFMutableStringRef theString, ConstStr255Param pStr, CFStringEncoding encoding);

CF_EXPORT
void CFStringAppendCString(CFMutableStringRef theString, const char *cStr, CFStringEncoding encoding);

CF_EXPORT
void CFStringAppendFormat(CFMutableStringRef theString, CFDictionaryRef formatOptions, CFStringRef format, ...);

CF_EXPORT
void CFStringAppendFormatAndArguments(CFMutableStringRef theString, CFDictionaryRef formatOptions, CFStringRef format, va_list arguments);

CF_EXPORT
void CFStringInsert(CFMutableStringRef str, CFIndex idx, CFStringRef insertedStr);

CF_EXPORT
void CFStringDelete(CFMutableStringRef theString, CFRange range);

CF_EXPORT
void CFStringReplace(CFMutableStringRef theString, CFRange range, CFStringRef replacement);

CF_EXPORT
void CFStringReplaceAll(CFMutableStringRef theString, CFStringRef replacement);	/* Replaces whole string */

#if MAC_OS_X_VERSION_10_2 <= MAC_OS_X_VERSION_MAX_ALLOWED
/* Replace all occurrences of target in rangeToSearch of theString with replacement.
   Pays attention to kCFCompareCaseInsensitive, kCFCompareBackwards, kCFCompareNonliteral, and kCFCompareAnchored.
   kCFCompareBackwards can be used to do the replacement starting from the end, which could give a different result.
     ex. AAAAA, replace AA with B -> BBA or ABB; latter if kCFCompareBackwards
   kCFCompareAnchored assures only anchored but multiple instances are found (the instances must be consecutive at start or end)
     ex. AAXAA, replace A with B -> BBXBB or BBXAA; latter if kCFCompareAnchored
   Returns number of replacements performed.
*/
CF_EXPORT
CFIndex CFStringFindAndReplace(CFMutableStringRef theString, CFStringRef stringToFind, CFStringRef replacementString, CFRange rangeToSearch, CFOptionFlags compareOptions);

#endif

/* This function will make the contents of a mutable CFString point directly at the specified UniChar array.
   It works only with CFStrings created with CFStringCreateMutableWithExternalCharactersNoCopy().
   This function does not free the previous buffer.
   The string will be manipulated within the provided buffer (if any) until it outgrows capacity; then the
     externalCharactersAllocator will be consulted for more memory.
   See comments at the top of this file for more info.
*/
CF_EXPORT
void CFStringSetExternalCharactersNoCopy(CFMutableStringRef theString, UniChar *chars, CFIndex length, CFIndex capacity);	/* Works only on specially created mutable strings! */

/* CFStringPad() will pad or cut down a string to the specified size.
   The pad string is used as the fill string; indexIntoPad specifies which character to start with.
     CFStringPad("abc", " ", 9, 0) ->  "abc      "
     CFStringPad("abc", ". ", 9, 1) -> "abc . . ."
     CFStringPad("abcdef", ?, 3, ?) -> "abc"

     CFStringTrim() will trim the specified string from both ends of the string.
     CFStringTrimWhitespace() will do the same with white space characters (tab, newline, etc)
     CFStringTrim("  abc ", " ") -> "abc"
     CFStringTrim("* * * *abc * ", "* ") -> "*abc "
*/
CF_EXPORT
void CFStringPad(CFMutableStringRef theString, CFStringRef padString, CFIndex length, CFIndex indexIntoPad);

CF_EXPORT
void CFStringTrim(CFMutableStringRef theString, CFStringRef trimString);

CF_EXPORT
void CFStringTrimWhitespace(CFMutableStringRef theString);

#if MAC_OS_X_VERSION_10_3 <= MAC_OS_X_VERSION_MAX_ALLOWED
CF_EXPORT
void CFStringLowercase(CFMutableStringRef theString, CFLocaleRef locale);

CF_EXPORT
void CFStringUppercase(CFMutableStringRef theString, CFLocaleRef locale);

CF_EXPORT
void CFStringCapitalize(CFMutableStringRef theString, CFLocaleRef locale);
#else
CF_EXPORT
void CFStringLowercase(CFMutableStringRef theString, const void *localeTBD); // localeTBD must be NULL on pre-10.3

CF_EXPORT
void CFStringUppercase(CFMutableStringRef theString, const void *localeTBD); // localeTBD must be NULL on pre-10.3

CF_EXPORT
void CFStringCapitalize(CFMutableStringRef theString, const void *localeTBD); // localeTBD must be NULL on pre-10.3
#endif

#if MAC_OS_X_VERSION_10_2 <= MAC_OS_X_VERSION_MAX_ALLOWED
/*!
	@typedef CFStringNormalizationForm
	This is the type of Unicode normalization forms as described in
	Unicode Technical Report #15.
*/
typedef enum {
	kCFStringNormalizationFormD = 0, // Canonical Decomposition
	kCFStringNormalizationFormKD, // Compatibility Decomposition
	kCFStringNormalizationFormC, // Canonical Decomposition followed by Canonical Composition
	kCFStringNormalizationFormKC // Compatibility Decomposition followed by Canonical Composition
} CFStringNormalizationForm;

/*!
	@function CFStringNormalize
	Normalizes the string into the specified form as described in
	Unicode Technical Report #15.
	@param theString  The string which is to be normalized.  If this
		parameter is not a valid mutable CFString, the behavior is
		undefined.
	@param theForm  The form into which the string is to be normalized.
		If this parameter is not a valid CFStringNormalizationForm value,
		the behavior is undefined.
*/
CF_EXPORT void CFStringNormalize(CFMutableStringRef theString, CFStringNormalizationForm theForm);
#endif

/* This returns availability of the encoding on the system
*/
CF_EXPORT
Boolean CFStringIsEncodingAvailable(CFStringEncoding encoding);

/* This function returns list of available encodings.  The returned list is terminated with kCFStringEncodingInvalidId and owned by the system.
*/
CF_EXPORT
const CFStringEncoding *CFStringGetListOfAvailableEncodings(void);

/* Returns name of the encoding; non-localized.
*/
CF_EXPORT
CFStringRef CFStringGetNameOfEncoding(CFStringEncoding encoding);

/* ID mapping functions from/to Cocoa NSStringEncoding.  Returns kCFStringEncodingInvalidId if no mapping exists.
*/
CF_EXPORT
UInt32 CFStringConvertEncodingToNSStringEncoding(CFStringEncoding encoding);

CF_EXPORT
CFStringEncoding CFStringConvertNSStringEncodingToEncoding(UInt32 encoding);

/* ID mapping functions from/to Microsoft Windows codepage (covers both OEM & ANSI).  Returns kCFStringEncodingInvalidId if no mapping exists.
*/
CF_EXPORT
UInt32 CFStringConvertEncodingToWindowsCodepage(CFStringEncoding encoding);

CF_EXPORT
CFStringEncoding CFStringConvertWindowsCodepageToEncoding(UInt32 codepage);

/* ID mapping functions from/to IANA registery charset names.  Returns kCFStringEncodingInvalidId if no mapping exists.
*/
CF_EXPORT
CFStringEncoding CFStringConvertIANACharSetNameToEncoding(CFStringRef theString);

CF_EXPORT
CFStringRef  CFStringConvertEncodingToIANACharSetName(CFStringEncoding encoding);

/* Returns the most compatible MacOS script value for the input encoding */
/* i.e. kCFStringEncodingMacRoman -> kCFStringEncodingMacRoman */
/*	kCFStringEncodingWindowsLatin1 -> kCFStringEncodingMacRoman */
/*	kCFStringEncodingISO_2022_JP -> kCFStringEncodingMacJapanese */
CF_EXPORT
CFStringEncoding CFStringGetMostCompatibleMacStringEncoding(CFStringEncoding encoding);

/* The next two functions allow fast access to the contents of a string, 
   assuming you are doing sequential or localized accesses. To use, call
   CFStringInitInlineBuffer() with a CFStringInlineBuffer (on the stack, say),
   and a range in the string to look at. Then call CFStringGetCharacterFromInlineBuffer()
   as many times as you want, with a index into that range (relative to the start
   of that range). These are INLINE functions and will end up calling CFString only 
   once in a while, to fill a buffer.  CFStringGetCharacterFromInlineBuffer() returns 0 if
   a location outside the original range is specified.
*/
#define __kCFStringInlineBufferLength 64
typedef struct {
    UniChar buffer[__kCFStringInlineBufferLength];
    CFStringRef theString;
    const UniChar *directBuffer;
    CFRange rangeToBuffer;		/* Range in string to buffer */
    CFIndex bufferedRangeStart;		/* Start of range currently buffered (relative to rangeToBuffer.location) */
    CFIndex bufferedRangeEnd;		/* bufferedRangeStart + number of chars actually buffered */
} CFStringInlineBuffer;

#if defined(CF_INLINE)
CF_INLINE void CFStringInitInlineBuffer(CFStringRef str, CFStringInlineBuffer *buf, CFRange range) {
    buf->theString = str;
    buf->rangeToBuffer = range;
    buf->directBuffer = CFStringGetCharactersPtr(str);
    buf->bufferedRangeStart = buf->bufferedRangeEnd = 0;
}

CF_INLINE UniChar CFStringGetCharacterFromInlineBuffer(CFStringInlineBuffer *buf, CFIndex idx) {
    if (buf->directBuffer) {
	if (idx < 0 || idx >= buf->rangeToBuffer.length) return 0;
        return buf->directBuffer[idx + buf->rangeToBuffer.location];
    }
    if (idx >= buf->bufferedRangeEnd || idx < buf->bufferedRangeStart) {
	if (idx < 0 || idx >= buf->rangeToBuffer.length) return 0;
	if ((buf->bufferedRangeStart = idx - 4) < 0) buf->bufferedRangeStart = 0;
	buf->bufferedRangeEnd = buf->bufferedRangeStart + __kCFStringInlineBufferLength;
	if (buf->bufferedRangeEnd > buf->rangeToBuffer.length) buf->bufferedRangeEnd = buf->rangeToBuffer.length;
	CFStringGetCharacters(buf->theString, CFRangeMake(buf->rangeToBuffer.location + buf->bufferedRangeStart, buf->bufferedRangeEnd - buf->bufferedRangeStart), buf->buffer);
    }
    return buf->buffer[idx - buf->bufferedRangeStart];
}

#else
/* If INLINE functions are not available, we do somewhat less powerful macros that work similarly (except be aware that the buf argument is evaluated multiple times).
*/
#define CFStringInitInlineBuffer(str, buf, range) \
    do {(buf)->theString = str; (buf)->rangeToBuffer = range; (buf)->directBuffer = CFStringGetCharactersPtr(str);} while (0)

#define CFStringGetCharacterFromInlineBuffer(buf, idx) \
    (((idx) < 0 || (idx) >= (buf)->rangeToBuffer.length) ? 0 : ((buf)->directBuffer ? (buf)->directBuffer[(idx) + (buf)->rangeToBuffer.location] : CFStringGetCharacterAtIndex((buf)->theString, (idx) + (buf)->rangeToBuffer.location)))

#endif /* CF_INLINE */


/* Rest of the stuff in this file is private and should not be used directly
*/
/* For debugging only
   Use CFShow() to printf the description of any CFType;
   Use CFShowStr() to printf detailed info about a CFString
*/
CF_EXPORT
void CFShow(CFTypeRef obj);

CF_EXPORT
void CFShowStr(CFStringRef str);

/* This function is private and should not be used directly */
CF_EXPORT
CFStringRef  __CFStringMakeConstantString(const char *cStr);	/* Private; do not use */

#if defined(__cplusplus)
}
#endif

#endif /* !__COREFOUNDATION_CFSTRING__ */

