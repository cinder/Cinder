/*	CFURL.h
	Copyright (c) 1998-2003, Apple, Inc. All rights reserved.
*/

#if !defined(__COREFOUNDATION_CFURL__)
#define __COREFOUNDATION_CFURL__ 1

#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFString.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
    kCFURLPOSIXPathStyle = 0,
    kCFURLHFSPathStyle,
    kCFURLWindowsPathStyle
} CFURLPathStyle;
    
typedef const struct __CFURL * CFURLRef;

/* CFURLs are composed of two fundamental pieces - their string, and a */
/* (possibly NULL) base URL.  A relative URL is one in which the string */
/* by itself does not fully specify the URL (for instance "myDir/image.tiff"); */
/* an absolute URL is one in which the string does fully specify the URL */
/* ("file://localhost/myDir/image.tiff").  Absolute URLs always have NULL */
/* base URLs; however, it is possible for a URL to have a NULL base, and still */
/* not be absolute.  Such a URL has only a relative string, and cannot be */
/* resolved.  Two CFURLs are considered equal if and only if their strings */
/* are equal and their bases are equal.  In other words, */
/* "file://localhost/myDir/image.tiff" is NOT equal to the URL with relative */
/* string "myDir/image.tiff" and base URL "file://localhost/".  Clients that */
/* need these less strict form of equality should convert all URLs to their */
/* absolute form via CFURLCopyAbsoluteURL(), then compare the absolute forms. */

CF_EXPORT
CFTypeID CFURLGetTypeID(void);

/* encoding will be used both to interpret the bytes of URLBytes, and to */
/* interpret any percent-escapes within the bytes. */
CF_EXPORT
CFURLRef CFURLCreateWithBytes(CFAllocatorRef allocator, const UInt8 *URLBytes, CFIndex length, CFStringEncoding encoding, CFURLRef baseURL);

/* Escapes any character that is not 7-bit ASCII with the byte-code */
/* for the given encoding.  If escapeWhitespace is true, whitespace */
/* characters (' ', '\t', '\r', '\n') will be escaped also (desirable */
/* if embedding the URL into a larger text stream like HTML) */
CF_EXPORT
CFDataRef CFURLCreateData(CFAllocatorRef allocator, CFURLRef url, CFStringEncoding encoding, Boolean escapeWhitespace);

/* Any escape sequences in URLString will be interpreted via UTF-8. */
CF_EXPORT
CFURLRef CFURLCreateWithString(CFAllocatorRef allocator, CFStringRef URLString, CFURLRef baseURL);

#if MAC_OS_X_VERSION_10_3 <= MAC_OS_X_VERSION_MAX_ALLOWED

/* Create an absolute URL directly, without requiring the extra step */
/* of calling CFURLCopyAbsoluteURL().  If useCompatibilityMode is  */
/* true, the rules historically used on the web are used to resolve */
/* relativeString against baseURL - these rules are generally listed */
/* in the RFC as optional or alternate interpretations.  Otherwise, */
/* the strict rules from the RFC are used.  The major differences are */
/* that in compatibility mode, we are lenient of the scheme appearing */
/* in relative portion, leading "../" components are removed from the */
/* final URL's path, and if the relative portion contains only */
/* resource specifier pieces (query, parameters, and fragment), then */
/* the last path component of the base URL will not be deleted  */
CFURLRef CFURLCreateAbsoluteURLWithBytes(CFAllocatorRef alloc, const UInt8 *relativeURLBytes, CFIndex length, CFStringEncoding encoding, CFURLRef baseURL, Boolean useCompatibilityMode) AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;
#endif

/* filePath should be the URL's path expressed as a path of the type */
/* fsType.  If filePath is not absolute, the resulting URL will be */
/* considered relative to the current working directory (evaluated */
/* at creation time).  isDirectory determines whether filePath is */
/* treated as a directory path when resolving against relative path */
/* components */
CF_EXPORT
CFURLRef CFURLCreateWithFileSystemPath(CFAllocatorRef allocator, CFStringRef filePath, CFURLPathStyle pathStyle, Boolean isDirectory);

CF_EXPORT
CFURLRef CFURLCreateFromFileSystemRepresentation(CFAllocatorRef allocator, const UInt8 *buffer, CFIndex bufLen, Boolean isDirectory);

CF_EXPORT
CFURLRef CFURLCreateWithFileSystemPathRelativeToBase(CFAllocatorRef allocator, CFStringRef filePath, CFURLPathStyle pathStyle, Boolean isDirectory, CFURLRef baseURL); 

CF_EXPORT
CFURLRef CFURLCreateFromFileSystemRepresentationRelativeToBase(CFAllocatorRef allocator, const UInt8 *buffer, CFIndex bufLen, Boolean isDirectory, CFURLRef baseURL);
                                                                         
/* Fills buffer with the file system's native representation of */
/* url's path. No more than maxBufLen bytes are written to buffer. */
/* The buffer should be at least the maximum path length for */
/* the file system in question to avoid failures for insufficiently */
/* large buffers.  If resolveAgainstBase is true, the url's relative */
/* portion is resolved against its base before the path is computed. */
/* Returns success or failure. */
CF_EXPORT
Boolean CFURLGetFileSystemRepresentation(CFURLRef url, Boolean resolveAgainstBase, UInt8 *buffer, CFIndex maxBufLen);

/* Creates a new URL by resolving the relative portion of relativeURL against its base. */
CF_EXPORT
CFURLRef CFURLCopyAbsoluteURL(CFURLRef relativeURL);

/* Returns the URL's string. */
CF_EXPORT
CFStringRef CFURLGetString(CFURLRef anURL);

/* Returns the base URL if it exists */
CF_EXPORT
CFURLRef CFURLGetBaseURL(CFURLRef anURL);

/*
All URLs can be broken into two pieces - the scheme (preceding the
first colon) and the resource specifier (following the first colon).
Most URLs are also "standard" URLs conforming to RFC 1808 (available
from www.w3c.org).  This category includes URLs of the file, http,
https, and ftp schemes, to name a few.  Standard URLs start the
resource specifier with two slashes ("//"), and can be broken into
four distinct pieces - the scheme, the net location, the path, and
further resource specifiers (typically an optional parameter, query,
and/or fragment).  The net location appears immediately following
the two slashes and goes up to the next slash; it's format is
scheme-specific, but is usually composed of some or all of a username,
password, host name, and port.  The path is a series of path components
separated by slashes; if the net location is present, the path always
begins with a slash.  Standard URLs can be relative to another URL,
in which case at least the scheme and possibly other pieces as well
come from the base URL (see RFC 1808 for precise details when resolving
a relative URL against its base).  The full URL is therefore

<scheme> "://" <net location> <path, always starting with slash> <add'l resource specifiers>

If a given CFURL can be decomposed (that is, conforms to RFC 1808), you
can ask for each of the four basic pieces (scheme, net location, path,
and resource specifer) separately, as well as for its base URL.  The
basic pieces are returned with any percent escape sequences still in
place (although note that the scheme may not legally include any
percent escapes); this is to allow the caller to distinguish between
percent sequences that may have syntactic meaning if replaced by the
character being escaped (for instance, a '/' in a path component).
Since only the individual schemes know which characters are
syntactically significant, CFURL cannot safely replace any percent
escape sequences.  However, you can use
CFURLCreateStringByReplacingPercentEscapes() to create a new string with
the percent escapes removed; see below.

If a given CFURL can not be decomposed, you can ask for its scheme and its
resource specifier; asking it for its net location or path will return NULL.

To get more refined information about the components of a decomposable
CFURL, you may ask for more specific pieces of the URL, expressed with
the percent escapes removed.  The available functions are CFURLCopyHostName(),
CFURLGetPortNumber() (returns an Int32), CFURLCopyUserName(),
CFURLCopyPassword(), CFURLCopyQuery(), CFURLCopyParameters(), and
CFURLCopyFragment().  Because the parameters, query, and fragment of an
URL may contain scheme-specific syntaxes, these methods take a second
argument, giving a list of characters which should NOT be replaced if
percent escaped.  For instance, the ftp parameter syntax gives simple
key-value pairs as "<key>=<value>;"  Clearly if a key or value includes
either '=' or ';', it must be escaped to avoid corrupting the meaning of
the parameters, so the caller may request the parameter string as

CFStringRef myParams = CFURLCopyParameters(ftpURL, CFSTR("=;%"));

requesting that all percent escape sequences be replaced by the represented
characters, except for escaped '=', '%' or ';' characters.  Pass the empty
string (CFSTR("")) to request that all percent escapes be replaced, or NULL
to request that none be.
*/

/* Returns true if anURL conforms to RFC 1808 */
CF_EXPORT
Boolean CFURLCanBeDecomposed(CFURLRef anURL); 

/* The next several methods leave any percent escape sequences intact */

CF_EXPORT
CFStringRef CFURLCopyScheme(CFURLRef anURL);

/* NULL if CFURLCanBeDecomposed(anURL) is false */
CF_EXPORT
CFStringRef CFURLCopyNetLocation(CFURLRef anURL); 

/* NULL if CFURLCanBeDecomposed(anURL) is false; also does not resolve the URL */
/* against its base.  See also CFURLCopyAbsoluteURL().  Note that, strictly */
/* speaking, any leading '/' is not considered part of the URL's path, although */
/* its presence or absence determines whether the path is absolute. */
/* CFURLCopyPath()'s return value includes any leading slash (giving the path */
/* the normal POSIX appearance); CFURLCopyStrictPath()'s return value omits any */
/* leading slash, and uses isAbsolute to report whether the URL's path is absolute. */

/* CFURLCopyFileSystemPath() returns the URL's path as a file system path for the */
/* given path style.  All percent escape sequences are replaced.  The URL is not */
/* resolved against its base before computing the path. */
CF_EXPORT
CFStringRef CFURLCopyPath(CFURLRef anURL);

CF_EXPORT
CFStringRef CFURLCopyStrictPath(CFURLRef anURL, Boolean *isAbsolute);

CF_EXPORT
CFStringRef CFURLCopyFileSystemPath(CFURLRef anURL, CFURLPathStyle pathStyle);

/* Returns whether anURL's path represents a directory */
/* (true returned) or a simple file (false returned) */
CF_EXPORT
Boolean CFURLHasDirectoryPath(CFURLRef anURL);

/* Any additional resource specifiers after the path.  For URLs */
/* that cannot be decomposed, this is everything except the scheme itself. */
CF_EXPORT
CFStringRef CFURLCopyResourceSpecifier(CFURLRef anURL); 

CF_EXPORT
CFStringRef CFURLCopyHostName(CFURLRef anURL);

CF_EXPORT
SInt32 CFURLGetPortNumber(CFURLRef anURL); /* Returns -1 if no port number is specified */

CF_EXPORT
CFStringRef CFURLCopyUserName(CFURLRef anURL);

CF_EXPORT
CFStringRef CFURLCopyPassword(CFURLRef anURL);

/* These remove all percent escape sequences except those for */
/* characters in charactersToLeaveEscaped.  If charactersToLeaveEscaped */
/* is empty (""), all percent escape sequences are replaced by their */
/* corresponding characters.  If charactersToLeaveEscaped is NULL, */
/* then no escape sequences are removed at all */
CF_EXPORT
CFStringRef CFURLCopyParameterString(CFURLRef anURL, CFStringRef charactersToLeaveEscaped);

CF_EXPORT
CFStringRef CFURLCopyQueryString(CFURLRef anURL, CFStringRef charactersToLeaveEscaped);

CF_EXPORT
CFStringRef CFURLCopyFragment(CFURLRef anURL, CFStringRef charactersToLeaveEscaped);

CF_EXPORT
CFStringRef CFURLCopyLastPathComponent(CFURLRef url);

CF_EXPORT
CFStringRef CFURLCopyPathExtension(CFURLRef url);

/* These functions all treat the base URL of the supplied url as */
/* invariant.  In other words, the URL returned will always have */
/* the same base as the URL supplied as an argument. */

CF_EXPORT
CFURLRef CFURLCreateCopyAppendingPathComponent(CFAllocatorRef allocator, CFURLRef url, CFStringRef pathComponent, Boolean isDirectory);

CF_EXPORT
CFURLRef CFURLCreateCopyDeletingLastPathComponent(CFAllocatorRef allocator, CFURLRef url);

CF_EXPORT
CFURLRef CFURLCreateCopyAppendingPathExtension(CFAllocatorRef allocator, CFURLRef url, CFStringRef extension);

CF_EXPORT
CFURLRef CFURLCreateCopyDeletingPathExtension(CFAllocatorRef allocator, CFURLRef url);

#if MAC_OS_X_VERSION_10_3 <= MAC_OS_X_VERSION_MAX_ALLOWED
/* Fills buffer with the bytes for url, returning the number of bytes */
/* filled.  If buffer is of insufficient size, returns -1 and no bytes */
/* are placed in buffer.  If buffer is NULL, the needed length is */
/* computed and returned.  The returned bytes are the original bytes */ 
/* from which the URL was created; if the URL was created from a */
/* string, the bytes will be the bytes of the string encoded via UTF-8  */
CF_EXPORT
CFIndex CFURLGetBytes(CFURLRef url, UInt8 *buffer, CFIndex bufferLength) AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;

typedef enum {
	kCFURLComponentScheme = 1,
	kCFURLComponentNetLocation = 2,
	kCFURLComponentPath = 3,
	kCFURLComponentResourceSpecifier = 4,

	kCFURLComponentUser = 5,
	kCFURLComponentPassword = 6,
	kCFURLComponentUserInfo = 7,
	kCFURLComponentHost = 8,
	kCFURLComponentPort = 9,
	kCFURLComponentParameterString = 10,
	kCFURLComponentQuery = 11,
	kCFURLComponentFragment = 12
} CFURLComponentType;
 
/* 
Gets the  range of the requested component in the bytes of url, as
returned by CFURLGetBytes().  This range is only good for use in the
bytes returned by CFURLGetBytes!

If non-NULL, rangeIncludingSeparators gives the range of component
including the sequences that separate component from the previous and
next components.  If there is no previous or next component, that end of
rangeIncludingSeparators will match the range of the component itself.
If url does not contain the given component type, (kCFNotFound, 0) is
returned, and rangeIncludingSeparators is set to the location where the
component would be inserted.  Some examples -

For the URL http://www.apple.com/hotnews/

Component           returned range      rangeIncludingSeparators
scheme              (0, 4)              (0, 7)
net location        (7, 13)             (4, 16)
path                (20, 9)             (20, 9)    
resource specifier  (kCFNotFound, 0)    (29, 0)
user                (kCFNotFound, 0)    (7, 0)
password            (kCFNotFound, 0)    (7, 0)
user info           (kCFNotFound, 0)    (7, 0)
host                (7, 13)             (4, 16)
port                (kCFNotFound, 0)    (20, 0)
parameter           (kCFNotFound, 0)    (29, 0)
query               (kCFNotFound, 0)    (29, 0)
fragment            (kCFNotFound, 0)    (29, 0)


For the URL ./relPath/file.html#fragment

Component           returned range      rangeIncludingSeparators
scheme              (kCFNotFound, 0)    (0, 0)
net location        (kCFNotFound, 0)    (0, 0)
path                (0, 19)             (0, 20)
resource specifier  (20, 8)             (19, 9)
user                (kCFNotFound, 0)    (0, 0)
password            (kCFNotFound, 0)    (0, 0)
user info           (kCFNotFound, 0)    (0, 0)
host                (kCFNotFound, 0)    (0, 0)
port                (kCFNotFound, 0)    (0, 0)
parameter           (kCFNotFound, 0)    (19, 0)
query               (kCFNotFound, 0)    (19, 0)
fragment            (20, 8)             (19, 9)


For the URL scheme://user:pass@host:1/path/path2/file.html;params?query#fragment

Component           returned range      rangeIncludingSeparators
scheme              (0, 6)              (0, 9)
net location        (9, 16)             (6, 19)
path                (25, 21)            (25, 22) 
resource specifier  (47, 21)            (46, 22)
user                (9, 4)              (6, 8)
password            (14, 4)             (13, 6)
user info           (9, 9)              (6, 13)
host                (19, 4)             (18, 6)
port                (24, 1)             (23, 2)
parameter           (47, 6)             (46, 8)
query               (54, 5)             (53, 7)
fragment            (60, 8)             (59, 9)
*/
CF_EXPORT
CFRange CFURLGetByteRangeForComponent(CFURLRef url, CFURLComponentType component, CFRange *rangeIncludingSeparators) AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;
#endif

/* Returns a string with any percent escape sequences that do NOT */
/* correspond to characters in charactersToLeaveEscaped with their */
/* equivalent.  Returns NULL on failure (if an invalid percent sequence */
/* is encountered), or the original string (retained) if no characters */
/* need to be replaced. Pass NULL to request that no percent escapes be */
/* replaced, or the empty string (CFSTR("")) to request that all percent */
/* escapes be replaced.  Uses UTF8 to interpret percent escapes. */
CF_EXPORT
CFStringRef CFURLCreateStringByReplacingPercentEscapes(CFAllocatorRef allocator, CFStringRef originalString, CFStringRef charactersToLeaveEscaped);

#if MAC_OS_X_VERSION_10_3 <= MAC_OS_X_VERSION_MAX_ALLOWED
/* As above, but allows you to specify the encoding to use when interpreting percent escapes */
CF_EXPORT
CFStringRef CFURLCreateStringByReplacingPercentEscapesUsingEncoding(CFAllocatorRef allocator, CFStringRef origString, CFStringRef charsToLeaveEscaped, CFStringEncoding encoding) AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER;
#endif

/* Creates a copy or originalString, replacing certain characters with */
/* the equivalent percent escape sequence based on the encoding specified. */
/* If the originalString does not need to be modified (no percent escape */
/* sequences are missing), may retain and return originalString. */
/* If you are uncertain of the correct encoding, you should use UTF-8, */
/* which is the encoding designated by RFC 2396 as the correct encoding */
/* for use in URLs.  The characters so escaped are all characters that */
/* are not legal URL characters (based on RFC 2396), plus any characters */
/* in legalURLCharactersToBeEscaped, less any characters in */
/* charactersToLeaveUnescaped.  To simply correct any non-URL characters */
/* in an otherwise correct URL string, do: */

/* newString = CFURLCreateStringByAddingPercentEscapes(NULL, origString, NULL, NULL, kCFStringEncodingUTF8); */
CF_EXPORT
CFStringRef CFURLCreateStringByAddingPercentEscapes(CFAllocatorRef allocator, CFStringRef originalString, CFStringRef charactersToLeaveUnescaped, CFStringRef legalURLCharactersToBeEscaped, CFStringEncoding encoding);


struct FSRef;

CF_EXPORT
CFURLRef CFURLCreateFromFSRef(CFAllocatorRef allocator, const struct FSRef *fsRef);

CF_EXPORT
Boolean CFURLGetFSRef(CFURLRef url, struct FSRef *fsRef);


#if defined(__cplusplus)
}
#endif

#endif /* !__COREFOUNDATION_CFURL__ */

