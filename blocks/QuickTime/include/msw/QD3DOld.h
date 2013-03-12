/*
     File:       QD3DOld.h
 
     Contains:   Obsolete API for Quickdraw 3D that QuickTime still uses
 
     Version:    Technology: Quickdraw 3D 1.5.1
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1995-1998 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QD3DOLD__
#define __QD3DOLD__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


#if TARGET_OS_MAC
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#endif  /* TARGET_OS_MAC */

#ifndef __QD3D__
#include <QD3D.h>
#endif

#ifndef __QD3DPICK__
#include <QD3DPick.h>
#endif

#ifndef __QD3DRENDERER__
#include <QD3DRenderer.h>
#endif

#ifndef __QD3DIO__
#include <QD3DIO.h>
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
    #pragma options align=power
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __QD3DOLD__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __QD3DOLD__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/******************************************************************************
 **                                                                          **
 **                     Obsolete Object System Types                         **
 **                         from QD3D.i                                      **
 **                                                                          **
 *****************************************************************************/
typedef struct OpaqueTQ3ObjectClass*    TQ3ObjectClass;
typedef unsigned long                   TQ3MethodType;
/*
 * Object methods
 */
#define kQ3MethodTypeObjectUnregister       Q3_METHOD_TYPE('u','n','r','g')
/*
 * IO Methods
 */
#define kQ3MethodTypeObjectFileVersion      Q3_METHOD_TYPE('v','e','r','s') /* version */
#define kQ3MethodTypeObjectTraverse         Q3_METHOD_TYPE('t','r','v','s') /* byte count */
#define kQ3MethodTypeObjectTraverseData     Q3_METHOD_TYPE('t','r','v','d') /* byte count */
#define kQ3MethodTypeObjectWrite            Q3_METHOD_TYPE('w','r','i','t') /* Dump info to file */
#define kQ3MethodTypeObjectReadData         Q3_METHOD_TYPE('r','d','d','t') /* Read info from file into buffer or, attach read data to parent */ 
typedef CALLBACK_API_C( void , TQ3FunctionPointer )(void);
typedef CALLBACK_API_C( TQ3FunctionPointer , TQ3MetaHandler )(TQ3MethodType methodType);
/*
 * MetaHandler:
 *      When you give a metahandler to QuickDraw 3D, it is called multiple times to
 *      build method tables, and then is thrown away. You are guaranteed that
 *      your metahandler will never be called again after a call that was passed
 *      a metahandler returns.
 *
 *      Your metahandler should contain a switch on the methodType passed to it
 *      and should return the corresponding method as an TQ3FunctionPointer.
 *
 *      IMPORTANT: A metaHandler MUST always "return" a value. If you are
 *      passed a methodType that you do not understand, ALWAYS return NULL.
 *
 *      These types here are prototypes of how your functions should look.
 */
typedef CALLBACK_API_C( TQ3Status , TQ3ObjectUnregisterMethod )(TQ3ObjectClass objectClass);
/******************************************************************************
 **                                                                          **
 **                     Obsolete ObjectClass Routines                        **
 **                         from QD3D.i                                      **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ObjectClass_Unregister is an obsolete function.  For the new extensions 
 *  model you should now use Q3XObjectHierarchy_UnregisterClass which is 
 *  defined in QD3DExtension.h and takes identical parameters.
 */
#if CALL_NOT_IN_CARBON
/*
 *  Q3ObjectClass_Unregister()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3ObjectClass_Unregister(TQ3ObjectClass objectClass);




/******************************************************************************
 **                                                                          **
 **                 Obsolete Custom Element Registration                     **
 **                         from QD3DSet.i                                   **
 **                                                                          **
 *****************************************************************************/
#define kQ3MethodTypeElementCopyAdd         Q3_METHOD_TYPE('e','c','p','a')
#define kQ3MethodTypeElementCopyReplace     Q3_METHOD_TYPE('e','c','p','r')
#define kQ3MethodTypeElementCopyGet         Q3_METHOD_TYPE('e','c','p','g')
#define kQ3MethodTypeElementCopyDuplicate   Q3_METHOD_TYPE('e','c','p','d')
#define kQ3MethodTypeElementDelete          Q3_METHOD_TYPE('e','d','e','l')
#endif  /* CALL_NOT_IN_CARBON */

typedef CALLBACK_API_C( TQ3Status , TQ3ElementCopyAddMethod )(const void *fromAPIElement, void *toInternalElement);
typedef CALLBACK_API_C( TQ3Status , TQ3ElementCopyReplaceMethod )(const void *fromAPIElement, void *ontoInternalElement);
typedef CALLBACK_API_C( TQ3Status , TQ3ElementCopyGetMethod )(const void *fromInternalElement, void *toAPIElement);
typedef CALLBACK_API_C( TQ3Status , TQ3ElementCopyDuplicateMethod )(const void *fromInternalElement, void *toInternalElement);
typedef CALLBACK_API_C( TQ3Status , TQ3ElementDeleteMethod )(void * internalElement);
#if CALL_NOT_IN_CARBON
/*
 *  Q3ElementClass_Register()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ObjectClass )
Q3ElementClass_Register(
  TQ3ElementType   elementType,
  const char *     name,
  unsigned long    sizeOfElement,
  TQ3MetaHandler   metaHandler);


/*
 *  Q3ElementType_GetElementSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3ElementType_GetElementSize(
  TQ3ElementType   elementType,
  unsigned long *  sizeOfElement);



/******************************************************************************
 **                                                                          **
 **                 Obsolete QD3DPick.i API                                  **
 **                                                                          **
 *****************************************************************************/
/*
 *  The following data structure has been obsoleted by
 *  the Q3Pick_GetPickDetailData function in conjunction
 *  with the pickDetailValue enum
 */
#endif  /* CALL_NOT_IN_CARBON */

struct TQ3HitData {
  TQ3PickParts        part;
  TQ3PickDetail       validMask;
  unsigned long       pickID;
  TQ3HitPath          path;
  TQ3Object           object;
  TQ3Matrix4x4        localToWorldMatrix;
  TQ3Point3D          xyzPoint;
  float               distance;
  TQ3Vector3D         normal;
  TQ3ShapePartObject  shapePart;
};
typedef struct TQ3HitData               TQ3HitData;
/*
 *  The following functions have been superseeded
 *  by the Q3Pick_GetPickDetailData function
 */
#if CALL_NOT_IN_CARBON
/*
 *  Q3Pick_GetHitData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetHitData(
  TQ3PickObject   pick,
  unsigned long   index,
  TQ3HitData *    hitData);


/*
 *  Q3Hit_EmptyData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Hit_EmptyData(TQ3HitData * hitData);


/******************************************************************************
 **                                                                          **
 **                 Obsolete QD3DRenderer.i API                              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Blocking, flush all buffered graphics to rasterizer and update
 *  draw context.
 *  
 *  This function has been replaced by Q3View_Sync
 */
/*
 *  Q3Renderer_Sync()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Renderer_Sync(
  TQ3RendererObject   renderer,
  TQ3ViewObject       view);



/******************************************************************************
 **                                                                          **
 **                     Obsolete QD3DIO.i API                                **
 **                                                                          **
 *****************************************************************************/
/*
 *  TQ3ObjectTraverseMethod
 *
 *  For "elements" (meaning "attributes, too), you will be passed NULL for 
 *  object. Sorry, custom objects will be available in the next major revision.
 *
 *  The "data" is a pointer to your internal element data.
 *
 *  The view is the current traversal view.
 */
#endif  /* CALL_NOT_IN_CARBON */

typedef CALLBACK_API_C( TQ3Status , TQ3ObjectTraverseMethod )(TQ3Object object, void *data, TQ3ViewObject view);
typedef CALLBACK_API_C( TQ3Status , TQ3ObjectWriteMethod )(const void *object, TQ3FileObject theFile);
/*
 *  Use Q3XView_SubmitWriteData instead...
 */
typedef CALLBACK_API_C( void , TQ3DataDeleteMethod )(void * data);
#if CALL_NOT_IN_CARBON
/*
 *  Q3View_SubmitWriteData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3View_SubmitWriteData(
  TQ3ViewObject         view,
  TQ3Size               size,
  void *                data,
  TQ3DataDeleteMethod   deleteData);


/*
 *  TQ3ObjectReadDataMethod
 *
 *  For "elements" (meaning "attributes", too), you must allocate stack space 
 *  and call Q3Set_Add on "parentObject", which is an TQ3SetObject.
 *
 *  Otherwise, parentObject is whatever object your element is a subobject of...
 */
#endif  /* CALL_NOT_IN_CARBON */

typedef CALLBACK_API_C( TQ3Status , TQ3ObjectReadDataMethod )(TQ3Object parentObject, TQ3FileObject theFile);


#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __QD3DOLD__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__QD3DOLD__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

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

#endif /* __QD3DOLD__ */

