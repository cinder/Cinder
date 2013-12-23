/*
     File:       QD3DPick.h
 
     Contains:   Q3Pick methods
 
     Version:    Technology: Quickdraw 3D 1.6
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1995-1998 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QD3DPICK__
#define __QD3DPICK__

#ifndef __QD3D__
#include <QD3D.h>
#endif


#ifndef __QD3DSTYLE__
#include <QD3DStyle.h>
#endif

#ifndef __QD3DGEOMETRY__
#include <QD3DGeometry.h>
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
        #define __QD3DPICK__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __QD3DPICK__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/*****************************************************************************
 **                                                                         **
 **                         Mask bits for hit information                   **
 **                                                                         **
 *****************************************************************************/
enum TQ3PickDetailMasks {
  kQ3PickDetailNone             = 0,
  kQ3PickDetailMaskPickID       = 1 << 0,
  kQ3PickDetailMaskPath         = 1 << 1,
  kQ3PickDetailMaskObject       = 1 << 2,
  kQ3PickDetailMaskLocalToWorldMatrix = 1 << 3,
  kQ3PickDetailMaskXYZ          = 1 << 4,
  kQ3PickDetailMaskDistance     = 1 << 5,
  kQ3PickDetailMaskNormal       = 1 << 6,
  kQ3PickDetailMaskShapePart    = 1 << 7,
  kQ3PickDetailMaskPickPart     = 1 << 8,
  kQ3PickDetailMaskUV           = 1 << 9
};
typedef enum TQ3PickDetailMasks TQ3PickDetailMasks;

typedef unsigned long                   TQ3PickDetail;
/******************************************************************************
 **                                                                          **
 **                             Hitlist sorting                              **
 **                                                                          **
 *****************************************************************************/
enum TQ3PickSort {
  kQ3PickSortNone               = 0,
  kQ3PickSortNearToFar          = 1,
  kQ3PickSortFarToNear          = 2
};
typedef enum TQ3PickSort TQ3PickSort;


/******************************************************************************
 **                                                                          **
 **                 Data structures to set up the pick object                **
 **                                                                          **
 *****************************************************************************/
#define kQ3ReturnAllHits        0
struct TQ3PickData {
  TQ3PickSort         sort;
  TQ3PickDetail       mask;
  unsigned long       numHitsToReturn;
};
typedef struct TQ3PickData              TQ3PickData;
struct TQ3WindowPointPickData {
  TQ3PickData         data;
  TQ3Point2D          point;
  float               vertexTolerance;
  float               edgeTolerance;
};
typedef struct TQ3WindowPointPickData   TQ3WindowPointPickData;
struct TQ3WindowRectPickData {
  TQ3PickData         data;
  TQ3Area             rect;
};
typedef struct TQ3WindowRectPickData    TQ3WindowRectPickData;
struct TQ3WorldRayPickData {
  TQ3PickData         data;
  TQ3Ray3D            ray;
  float               vertexTolerance;
  float               edgeTolerance;
};
typedef struct TQ3WorldRayPickData      TQ3WorldRayPickData;

/******************************************************************************
 **                                                                          **
 **                                 Hit data                                 **
 **                                                                          **
 *****************************************************************************/
struct TQ3HitPath {
  TQ3GroupObject      rootGroup;
  unsigned long       depth;
  TQ3GroupPosition *  positions;
};
typedef struct TQ3HitPath               TQ3HitPath;
/******************************************************************************
 **                                                                          **
 **                             Pick class methods                           **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3Pick_GetType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ObjectType )
Q3Pick_GetType(TQ3PickObject pick);


/*
 *  Q3Pick_GetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetData(
  TQ3PickObject   pick,
  TQ3PickData *   data);


/*
 *  Q3Pick_SetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_SetData(
  TQ3PickObject        pick,
  const TQ3PickData *  data);


/*
 *  Q3Pick_GetVertexTolerance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetVertexTolerance(
  TQ3PickObject   pick,
  float *         vertexTolerance);


/*
 *  Q3Pick_GetEdgeTolerance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetEdgeTolerance(
  TQ3PickObject   pick,
  float *         edgeTolerance);


/*
 *  Q3Pick_SetVertexTolerance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_SetVertexTolerance(
  TQ3PickObject   pick,
  float           vertexTolerance);


/*
 *  Q3Pick_SetEdgeTolerance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_SetEdgeTolerance(
  TQ3PickObject   pick,
  float           edgeTolerance);


/*
 *  Q3Pick_GetNumHits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetNumHits(
  TQ3PickObject    pick,
  unsigned long *  numHits);


/*
 *  Q3Pick_EmptyHitList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_EmptyHitList(TQ3PickObject pick);


/*
 *  Q3Pick_GetPickDetailValidMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetPickDetailValidMask(
  TQ3PickObject    pick,
  unsigned long    index,
  TQ3PickDetail *  pickDetailValidMask);


/*
 *  Q3Pick_GetPickDetailData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Pick_GetPickDetailData(
  TQ3PickObject   pick,
  unsigned long   index,
  TQ3PickDetail   pickDetailValue,
  void *          detailData);


/*
 *  Q3HitPath_EmptyData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3HitPath_EmptyData(TQ3HitPath * hitPath);


#endif  /* CALL_NOT_IN_CARBON */

/******************************************************************************
 **                                                                          **
 **                         Window point pick methods                        **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3WindowPointPick_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3PickObject )
Q3WindowPointPick_New(const TQ3WindowPointPickData * data);


/*
 *  Q3WindowPointPick_GetPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowPointPick_GetPoint(
  TQ3PickObject   pick,
  TQ3Point2D *    point);


/*
 *  Q3WindowPointPick_SetPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowPointPick_SetPoint(
  TQ3PickObject       pick,
  const TQ3Point2D *  point);


/*
 *  Q3WindowPointPick_GetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowPointPick_GetData(
  TQ3PickObject             pick,
  TQ3WindowPointPickData *  data);


/*
 *  Q3WindowPointPick_SetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowPointPick_SetData(
  TQ3PickObject                   pick,
  const TQ3WindowPointPickData *  data);



/******************************************************************************
 **                                                                          **
 **                         Window rect pick methods                         **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3WindowRectPick_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3PickObject )
Q3WindowRectPick_New(const TQ3WindowRectPickData * data);


/*
 *  Q3WindowRectPick_GetRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowRectPick_GetRect(
  TQ3PickObject   pick,
  TQ3Area *       rect);


/*
 *  Q3WindowRectPick_SetRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowRectPick_SetRect(
  TQ3PickObject    pick,
  const TQ3Area *  rect);


/*
 *  Q3WindowRectPick_GetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowRectPick_GetData(
  TQ3PickObject            pick,
  TQ3WindowRectPickData *  data);


/*
 *  Q3WindowRectPick_SetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WindowRectPick_SetData(
  TQ3PickObject                  pick,
  const TQ3WindowRectPickData *  data);



/******************************************************************************
 **                                                                          **
 **                         World ray pick methods                           **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3WorldRayPick_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3PickObject )
Q3WorldRayPick_New(const TQ3WorldRayPickData * data);


/*
 *  Q3WorldRayPick_GetRay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WorldRayPick_GetRay(
  TQ3PickObject   pick,
  TQ3Ray3D *      ray);


/*
 *  Q3WorldRayPick_SetRay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WorldRayPick_SetRay(
  TQ3PickObject     pick,
  const TQ3Ray3D *  ray);


/*
 *  Q3WorldRayPick_GetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WorldRayPick_GetData(
  TQ3PickObject          pick,
  TQ3WorldRayPickData *  data);


/*
 *  Q3WorldRayPick_SetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3WorldRayPick_SetData(
  TQ3PickObject                pick,
  const TQ3WorldRayPickData *  data);




/******************************************************************************
 **                                                                          **
 **                             Shape Part methods                           **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ShapePart_GetType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ObjectType )
Q3ShapePart_GetType(TQ3ShapePartObject shapePartObject);


/*
 *  Q3MeshPart_GetType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ObjectType )
Q3MeshPart_GetType(TQ3MeshPartObject meshPartObject);


/*
 *  Q3ShapePart_GetShape()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3ShapePart_GetShape(
  TQ3ShapePartObject   shapePartObject,
  TQ3ShapeObject *     shapeObject);


/*
 *  Q3MeshPart_GetComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MeshPart_GetComponent(
  TQ3MeshPartObject   meshPartObject,
  TQ3MeshComponent *  component);


/*
 *  Q3MeshFacePart_GetFace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MeshFacePart_GetFace(
  TQ3MeshFacePartObject   meshFacePartObject,
  TQ3MeshFace *           face);


/*
 *  Q3MeshEdgePart_GetEdge()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MeshEdgePart_GetEdge(
  TQ3MeshEdgePartObject   meshEdgePartObject,
  TQ3MeshEdge *           edge);


/*
 *  Q3MeshVertexPart_GetVertex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MeshVertexPart_GetVertex(
  TQ3MeshVertexPartObject   meshVertexPartObject,
  TQ3MeshVertex *           vertex);




#endif  /* CALL_NOT_IN_CARBON */


#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __QD3DPICK__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__QD3DPICK__RESTORE_PACKED_ENUMS)
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

#endif /* __QD3DPICK__ */

