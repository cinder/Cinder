/*
     File:       PMPluginHeader.h
 
     Contains:   Mac OS X Printing Manager Raster Definitions.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

/* API Version is used for plugin object API compatibility check. */


#if PRAGMA_ONCE
#pragma once
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

struct PMPlugInAPIVersion {
                                              /* API build version. */
                                              /* This is the API version this object was compiled with. */
  UInt32              buildVersionMajor;
  UInt32              buildVersionMinor;

                                              /* Base API version this object is upward compatible with. */
                                              /* The object guarantees to be a super-set of said API. */
  UInt32              baseVersionMajor;
  UInt32              baseVersionMinor;

};
typedef struct PMPlugInAPIVersion       PMPlugInAPIVersion;
/* The plugin header interface, and its vtable. */
/* Plugin interface is an address of an object which contains a vtable pointer */
/* as its first member. Every plugin interface (except 'iunknown') must begin */
/* with PMPlugInHeader which must not change. */
typedef struct PMPlugInHeader           PMPlugInHeader;
struct PMPlugInHeaderInterface {
  const PMPlugInHeader * vtable;
};
typedef struct PMPlugInHeaderInterface  PMPlugInHeaderInterface;
typedef CALLBACK_API_C( OSStatus , PMPluginRetain )(PMPlugInHeaderInterface * obj);
typedef CALLBACK_API_C( OSStatus , PMPluginRelease )(PMPlugInHeaderInterface ** objPtr);
typedef CALLBACK_API_C( OSStatus , PMPluginGetAPIVersion )(PMPlugInHeaderInterface *obj, PMPlugInAPIVersion *versionPtr);
struct PMPlugInHeader {
                                              /* ---- API 'IMMUTABLE' Begin: ---- */

                                              /* Retain an object.*/
  PMPluginRetain      Retain;

                                              /* Release an object. When reference count goes down to zero object*/
                                              /* is deleted. The call clears caller's reference to the object.*/
  PMPluginRelease     Release;

                                              /* GetAPIVersion method is required with all plugin modules*/
                                              /* to be able to get object's API build and compatibility version.*/
                                              /* This version is used for plugin API compatibility check.*/
  PMPluginGetAPIVersion  GetAPIVersion;
};


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

