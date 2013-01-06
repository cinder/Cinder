/*
     File:       ICADevice.h
 
     Contains:   Low level Image Capture device definitions.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ICADEVICE__
#define __ICADEVICE__

#ifndef __ICAAPPLICATION__
#include <ICAApplication.h>
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

/* 
--------------- Completion Procs --------------- 
*/
/*
   
   NOTE: the parameter for the completion proc (ICDHeader*) has to be casted to the appropriate type
   e.g. (ICD_BuildObjectChildrenPB*), ...
   
*/
typedef struct ICDHeader                ICDHeader;
typedef CALLBACK_API_C( void , ICDCompletion )(ICDHeader * pb);
/* 
--------------- ICDHeader --------------- 
*/
struct ICDHeader {
  OSErr               err;                    /* --> */
  UInt32              refcon;                 /* <-- */
};

/*
--------------- Object parameter blocks ---------------
*/
struct ICD_NewObjectPB {
  ICDHeader           header;

  ICAObject           parentObject;           /* <-- */
  ICAObjectInfo       objectInfo;             /* <-- */

  ICAObject           object;                 /* --> */
};
typedef struct ICD_NewObjectPB          ICD_NewObjectPB;
struct ICD_DisposeObjectPB {
  ICDHeader           header;

  ICAObject           object;                 /* <-- */
};
typedef struct ICD_DisposeObjectPB      ICD_DisposeObjectPB;
/*
--------------- Property parameter blocks ---------------
*/
struct ICD_NewPropertyPB {
  ICDHeader           header;

  ICAObject           object;                 /* <-- */
  ICAPropertyInfo     propertyInfo;           /* <-- */

  ICAProperty         property;               /* --> */
};
typedef struct ICD_NewPropertyPB        ICD_NewPropertyPB;
struct ICD_DisposePropertyPB {
  ICDHeader           header;

  ICAProperty         property;               /* <-- */
};
typedef struct ICD_DisposePropertyPB    ICD_DisposePropertyPB;
/*
   
   NOTE: for all APIs - pass NULL as completion parameter to make a synchronous call 
   
*/

/* 
--------------- Object utilities for device libraries --------------- 
*/
/*
 *  ICDNewObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ImageCaptureLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICDNewObject(
  ICD_NewObjectPB *  pb,
  ICDCompletion      completion);      /* can be NULL */


/*
 *  ICDDisposeObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ImageCaptureLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICDDisposeObject(
  ICD_DisposeObjectPB *  pb,
  ICDCompletion          completion);      /* can be NULL */


/*
 *  ICDNewProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ImageCaptureLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICDNewProperty(
  ICD_NewPropertyPB *  pb,
  ICDCompletion        completion);      /* can be NULL */


/*
 *  ICDDisposeProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ImageCaptureLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICDDisposeProperty(
  ICD_DisposePropertyPB *  pb,
  ICDCompletion            completion);      /* can be NULL */







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

#endif /* __ICADEVICE__ */

