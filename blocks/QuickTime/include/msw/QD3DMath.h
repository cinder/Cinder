/*
     File:       QD3DMath.h
 
     Contains:   Math & matrix routines and definitions.
 
     Version:    Technology: Quickdraw 3D 1.6
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1995-1998 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QD3DMATH__
#define __QD3DMATH__

#ifndef __QD3D__
#include <QD3D.h>
#endif


#include <float.h>


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
        #define __QD3DMATH__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __QD3DMATH__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/******************************************************************************
 **                                                                          **
 **                         Constant Definitions                             **
 **                                                                          **
 *****************************************************************************/
/*
 *  Real zero definition
 */

#ifdef FLT_EPSILON
    #define kQ3RealZero         (FLT_EPSILON)
#else
    #define kQ3RealZero         ((float)1.19209290e-07)
#endif

#ifdef FLT_MAX
 #define kQ3MaxFloat         (FLT_MAX)
#else
    #define kQ3MaxFloat         ((float)3.40282347e+38)
#endif

/*
 *  Values of PI
 */
#define kQ3Pi                   ((float)3.1415926535898)
#define kQ32Pi                  ((float)(2.0 * 3.1415926535898))
#define kQ3PiOver2              ((float)(3.1415926535898 / 2.0))
#define kQ33PiOver2             ((float)(3.0 * 3.1415926535898 / 2.0))


/******************************************************************************
 **                                                                          **
 **                         Miscellaneous Functions                          **
 **                                                                          **
 *****************************************************************************/
#define Q3Math_DegreesToRadians(x)  ((float)((x) *  kQ3Pi / 180.0f))
#define Q3Math_RadiansToDegrees(x)  ((float)((x) * 180.0f / kQ3Pi))
#define Q3Math_Min(x,y)             ((x) <= (y) ? (x) : (y))
#define Q3Math_Max(x,y)             ((x) >= (y) ? (x) : (y))

/******************************************************************************
 **                                                                          **
 **                         Point and Vector Creation                        **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3Point2D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3Point2D_Set(
  TQ3Point2D *  point2D,
  float         x,
  float         y);


/*
 *  Q3Param2D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Param2D * )
Q3Param2D_Set(
  TQ3Param2D *  param2D,
  float         u,
  float         v);


/*
 *  Q3Point3D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_Set(
  TQ3Point3D *  point3D,
  float         x,
  float         y,
  float         z);


/*
 *  Q3RationalPoint3D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint3D * )
Q3RationalPoint3D_Set(
  TQ3RationalPoint3D *  point3D,
  float                 x,
  float                 y,
  float                 w);


/*
 *  Q3RationalPoint4D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint4D * )
Q3RationalPoint4D_Set(
  TQ3RationalPoint4D *  point4D,
  float                 x,
  float                 y,
  float                 z,
  float                 w);


/*
 *  Q3Vector2D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Set(
  TQ3Vector2D *  vector2D,
  float          x,
  float          y);


/*
 *  Q3Vector3D_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Set(
  TQ3Vector3D *  vector3D,
  float          x,
  float          y,
  float          z);


/*
 *  Q3PolarPoint_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3PolarPoint * )
Q3PolarPoint_Set(
  TQ3PolarPoint *  polarPoint,
  float            r,
  float            theta);


/*
 *  Q3SphericalPoint_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3SphericalPoint * )
Q3SphericalPoint_Set(
  TQ3SphericalPoint *  sphericalPoint,
  float                rho,
  float                theta,
  float                phi);



/******************************************************************************
 **                                                                          **
 **                 Point and Vector Dimension Conversion                    **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_To3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point2D_To3D(
  const TQ3Point2D *  point2D,
  TQ3Point3D *        result);


/*
 *  Q3RationalPoint3D_To2D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3RationalPoint3D_To2D(
  const TQ3RationalPoint3D *  point3D,
  TQ3Point2D *                result);


/*
 *  Q3Point3D_To4D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint4D * )
Q3Point3D_To4D(
  const TQ3Point3D *    point3D,
  TQ3RationalPoint4D *  result);


/*
 *  Q3RationalPoint4D_To3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3RationalPoint4D_To3D(
  const TQ3RationalPoint4D *  point4D,
  TQ3Point3D *                result);


/*
 *  Q3Vector2D_To3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector2D_To3D(
  const TQ3Vector2D *  vector2D,
  TQ3Vector3D *        result);


/*
 *  Q3Vector3D_To2D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector3D_To2D(
  const TQ3Vector3D *  vector3D,
  TQ3Vector2D *        result);



/******************************************************************************
 **                                                                          **
 **                         Point Subtraction                                **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Point2D_Subtract(
  const TQ3Point2D *  p1,
  const TQ3Point2D *  p2,
  TQ3Vector2D *       result);


/*
 *  Q3Param2D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Param2D_Subtract(
  const TQ3Param2D *  p1,
  const TQ3Param2D *  p2,
  TQ3Vector2D *       result);


/*
 *  Q3Point3D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Point3D_Subtract(
  const TQ3Point3D *  p1,
  const TQ3Point3D *  p2,
  TQ3Vector3D *       result);



/******************************************************************************
 **                                                                          **
 **                         Point Distance                                   **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_Distance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Point2D_Distance(
  const TQ3Point2D *  p1,
  const TQ3Point2D *  p2);


/*
 *  Q3Point2D_DistanceSquared()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Point2D_DistanceSquared(
  const TQ3Point2D *  p1,
  const TQ3Point2D *  p2);



/*
 *  Q3Param2D_Distance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Param2D_Distance(
  const TQ3Param2D *  p1,
  const TQ3Param2D *  p2);


/*
 *  Q3Param2D_DistanceSquared()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Param2D_DistanceSquared(
  const TQ3Param2D *  p1,
  const TQ3Param2D *  p2);



/*
 *  Q3RationalPoint3D_Distance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3RationalPoint3D_Distance(
  const TQ3RationalPoint3D *  p1,
  const TQ3RationalPoint3D *  p2);


/*
 *  Q3RationalPoint3D_DistanceSquared()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3RationalPoint3D_DistanceSquared(
  const TQ3RationalPoint3D *  p1,
  const TQ3RationalPoint3D *  p2);



/*
 *  Q3Point3D_Distance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Point3D_Distance(
  const TQ3Point3D *  p1,
  const TQ3Point3D *  p2);


/*
 *  Q3Point3D_DistanceSquared()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Point3D_DistanceSquared(
  const TQ3Point3D *  p1,
  const TQ3Point3D *  p2);



/*
 *  Q3RationalPoint4D_Distance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3RationalPoint4D_Distance(
  const TQ3RationalPoint4D *  p1,
  const TQ3RationalPoint4D *  p2);


/*
 *  Q3RationalPoint4D_DistanceSquared()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3RationalPoint4D_DistanceSquared(
  const TQ3RationalPoint4D *  p1,
  const TQ3RationalPoint4D *  p2);



/******************************************************************************
 **                                                                          **
 **                         Point Relative Ratio                             **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_RRatio()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3Point2D_RRatio(
  const TQ3Point2D *  p1,
  const TQ3Point2D *  p2,
  float               r1,
  float               r2,
  TQ3Point2D *        result);


/*
 *  Q3Param2D_RRatio()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Param2D * )
Q3Param2D_RRatio(
  const TQ3Param2D *  p1,
  const TQ3Param2D *  p2,
  float               r1,
  float               r2,
  TQ3Param2D *        result);


/*
 *  Q3Point3D_RRatio()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_RRatio(
  const TQ3Point3D *  p1,
  const TQ3Point3D *  p2,
  float               r1,
  float               r2,
  TQ3Point3D *        result);


/*
 *  Q3RationalPoint4D_RRatio()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint4D * )
Q3RationalPoint4D_RRatio(
  const TQ3RationalPoint4D *  p1,
  const TQ3RationalPoint4D *  p2,
  float                       r1,
  float                       r2,
  TQ3RationalPoint4D *        result);



/******************************************************************************
 **                                                                          **
 **                 Point / Vector Addition & Subtraction                    **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_Vector2D_Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3Point2D_Vector2D_Add(
  const TQ3Point2D *   point2D,
  const TQ3Vector2D *  vector2D,
  TQ3Point2D *         result);


/*
 *  Q3Param2D_Vector2D_Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Param2D * )
Q3Param2D_Vector2D_Add(
  const TQ3Param2D *   param2D,
  const TQ3Vector2D *  vector2D,
  TQ3Param2D *         result);


/*
 *  Q3Point3D_Vector3D_Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_Vector3D_Add(
  const TQ3Point3D *   point3D,
  const TQ3Vector3D *  vector3D,
  TQ3Point3D *         result);


/*
 *  Q3Point2D_Vector2D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3Point2D_Vector2D_Subtract(
  const TQ3Point2D *   point2D,
  const TQ3Vector2D *  vector2D,
  TQ3Point2D *         result);


/*
 *  Q3Param2D_Vector2D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Param2D * )
Q3Param2D_Vector2D_Subtract(
  const TQ3Param2D *   param2D,
  const TQ3Vector2D *  vector2D,
  TQ3Param2D *         result);


/*
 *  Q3Point3D_Vector3D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_Vector3D_Subtract(
  const TQ3Point3D *   point3D,
  const TQ3Vector3D *  vector3D,
  TQ3Point3D *         result);



/******************************************************************************
 **                                                                          **
 **                             Vector Scale                                 **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Scale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Scale(
  const TQ3Vector2D *  vector2D,
  float                scalar,
  TQ3Vector2D *        result);


/*
 *  Q3Vector3D_Scale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Scale(
  const TQ3Vector3D *  vector3D,
  float                scalar,
  TQ3Vector3D *        result);



/******************************************************************************
 **                                                                          **
 **                             Vector Length                                **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Length()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Vector2D_Length(const TQ3Vector2D * vector2D);


/*
 *  Q3Vector3D_Length()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Vector3D_Length(const TQ3Vector3D * vector3D);



/******************************************************************************
 **                                                                          **
 **                             Vector Normalize                             **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Normalize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Normalize(
  const TQ3Vector2D *  vector2D,
  TQ3Vector2D *        result);


/*
 *  Q3Vector3D_Normalize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Normalize(
  const TQ3Vector3D *  vector3D,
  TQ3Vector3D *        result);



/******************************************************************************
 **                                                                          **
 **                 Vector/Vector Addition and Subtraction                   **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Add(
  const TQ3Vector2D *  v1,
  const TQ3Vector2D *  v2,
  TQ3Vector2D *        result);


/*
 *  Q3Vector3D_Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Add(
  const TQ3Vector3D *  v1,
  const TQ3Vector3D *  v2,
  TQ3Vector3D *        result);



/*
 *  Q3Vector2D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Subtract(
  const TQ3Vector2D *  v1,
  const TQ3Vector2D *  v2,
  TQ3Vector2D *        result);


/*
 *  Q3Vector3D_Subtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Subtract(
  const TQ3Vector3D *  v1,
  const TQ3Vector3D *  v2,
  TQ3Vector3D *        result);



/******************************************************************************
 **                                                                          **
 **                             Cross Product                                **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Cross()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Vector2D_Cross(
  const TQ3Vector2D *  v1,
  const TQ3Vector2D *  v2);


/*
 *  Q3Vector3D_Cross()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Cross(
  const TQ3Vector3D *  v1,
  const TQ3Vector3D *  v2,
  TQ3Vector3D *        result);


/*
 *  Q3Point3D_CrossProductTri()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Point3D_CrossProductTri(
  const TQ3Point3D *  point1,
  const TQ3Point3D *  point2,
  const TQ3Point3D *  point3,
  TQ3Vector3D *       crossVector);



/******************************************************************************
 **                                                                          **
 **                             Dot Product                                  **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Dot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Vector2D_Dot(
  const TQ3Vector2D *  v1,
  const TQ3Vector2D *  v2);


/*
 *  Q3Vector3D_Dot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Vector3D_Dot(
  const TQ3Vector3D *  v1,
  const TQ3Vector3D *  v2);



/******************************************************************************
 **                                                                          **
 **                     Point and Vector Transformation                      **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Transform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Transform(
  const TQ3Vector2D *   vector2D,
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Vector2D *         result);


/*
 *  Q3Vector3D_Transform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Transform(
  const TQ3Vector3D *   vector3D,
  const TQ3Matrix4x4 *  matrix4x4,
  TQ3Vector3D *         result);


/*
 *  Q3Point2D_Transform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3Point2D_Transform(
  const TQ3Point2D *    point2D,
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Point2D *          result);


/*
 *  Q3Param2D_Transform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Param2D * )
Q3Param2D_Transform(
  const TQ3Param2D *    param2D,
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Param2D *          result);


/*
 *  Q3Point3D_Transform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_Transform(
  const TQ3Point3D *    point3D,
  const TQ3Matrix4x4 *  matrix4x4,
  TQ3Point3D *          result);


/*
 *  Q3RationalPoint4D_Transform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint4D * )
Q3RationalPoint4D_Transform(
  const TQ3RationalPoint4D *  point4D,
  const TQ3Matrix4x4 *        matrix4x4,
  TQ3RationalPoint4D *        result);


/*
 *  Q3Point3D_To3DTransformArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Point3D_To3DTransformArray(
  const TQ3Point3D *    inPoint3D,
  const TQ3Matrix4x4 *  matrix,
  TQ3Point3D *          outPoint3D,
  long                  numPoints,
  unsigned long         inStructSize,
  unsigned long         outStructSize);


/*
 *  Q3Point3D_To4DTransformArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Point3D_To4DTransformArray(
  const TQ3Point3D *    inPoint3D,
  const TQ3Matrix4x4 *  matrix,
  TQ3RationalPoint4D *  outPoint4D,
  long                  numPoints,
  unsigned long         inStructSize,
  unsigned long         outStructSize);


/*
 *  Q3RationalPoint4D_To4DTransformArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3RationalPoint4D_To4DTransformArray(
  const TQ3RationalPoint4D *  inPoint4D,
  const TQ3Matrix4x4 *        matrix,
  TQ3RationalPoint4D *        outPoint4D,
  long                        numPoints,
  unsigned long               inStructSize,
  unsigned long               outStructSize);



/******************************************************************************
 **                                                                          **
 **                             Vector Negation                              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Vector2D_Negate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector2D * )
Q3Vector2D_Negate(
  const TQ3Vector2D *  vector2D,
  TQ3Vector2D *        result);


/*
 *  Q3Vector3D_Negate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_Negate(
  const TQ3Vector3D *  vector3D,
  TQ3Vector3D *        result);



/******************************************************************************
 **                                                                          **
 **                 Point conversion from cartesian to polar                 **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_ToPolar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3PolarPoint * )
Q3Point2D_ToPolar(
  const TQ3Point2D *  point2D,
  TQ3PolarPoint *     result);


/*
 *  Q3PolarPoint_ToPoint2D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3PolarPoint_ToPoint2D(
  const TQ3PolarPoint *  polarPoint,
  TQ3Point2D *           result);


/*
 *  Q3Point3D_ToSpherical()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3SphericalPoint * )
Q3Point3D_ToSpherical(
  const TQ3Point3D *   point3D,
  TQ3SphericalPoint *  result);


/*
 *  Q3SphericalPoint_ToPoint3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3SphericalPoint_ToPoint3D(
  const TQ3SphericalPoint *  sphericalPoint,
  TQ3Point3D *               result);



/******************************************************************************
 **                                                                          **
 **                         Point Affine Combinations                        **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Point2D_AffineComb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point2D * )
Q3Point2D_AffineComb(
  const TQ3Point2D *  points2D,
  const float *       weights,
  unsigned long       nPoints,
  TQ3Point2D *        result);


/*
 *  Q3Param2D_AffineComb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Param2D * )
Q3Param2D_AffineComb(
  const TQ3Param2D *  params2D,
  const float *       weights,
  unsigned long       nPoints,
  TQ3Param2D *        result);


/*
 *  Q3RationalPoint3D_AffineComb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint3D * )
Q3RationalPoint3D_AffineComb(
  const TQ3RationalPoint3D *  points3D,
  const float *               weights,
  unsigned long               numPoints,
  TQ3RationalPoint3D *        result);


/*
 *  Q3Point3D_AffineComb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_AffineComb(
  const TQ3Point3D *  points3D,
  const float *       weights,
  unsigned long       numPoints,
  TQ3Point3D *        result);


/*
 *  Q3RationalPoint4D_AffineComb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3RationalPoint4D * )
Q3RationalPoint4D_AffineComb(
  const TQ3RationalPoint4D *  points4D,
  const float *               weights,
  unsigned long               numPoints,
  TQ3RationalPoint4D *        result);



/******************************************************************************
 **                                                                          **
 **                             Matrix Functions                             **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Matrix3x3_Copy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_Copy(
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Matrix3x3 *        result);


/*
 *  Q3Matrix4x4_Copy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_Copy(
  const TQ3Matrix4x4 *  matrix4x4,
  TQ3Matrix4x4 *        result);



/*
 *  Q3Matrix3x3_SetIdentity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_SetIdentity(TQ3Matrix3x3 * matrix3x3);


/*
 *  Q3Matrix4x4_SetIdentity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetIdentity(TQ3Matrix4x4 * matrix4x4);



/*
 *  Q3Matrix3x3_Transpose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_Transpose(
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Matrix3x3 *        result);


/*
 *  Q3Matrix4x4_Transpose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_Transpose(
  const TQ3Matrix4x4 *  matrix4x4,
  TQ3Matrix4x4 *        result);



/*
 *  Q3Matrix3x3_Invert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_Invert(
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Matrix3x3 *        result);


/*
 *  Q3Matrix4x4_Invert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_Invert(
  const TQ3Matrix4x4 *  matrix4x4,
  TQ3Matrix4x4 *        result);



/*
 *  Q3Matrix3x3_Adjoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_Adjoint(
  const TQ3Matrix3x3 *  matrix3x3,
  TQ3Matrix3x3 *        result);



/*
 *  Q3Matrix3x3_Multiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_Multiply(
  const TQ3Matrix3x3 *  matrixA,
  const TQ3Matrix3x3 *  matrixB,
  TQ3Matrix3x3 *        result);


/*
 *  Q3Matrix4x4_Multiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_Multiply(
  const TQ3Matrix4x4 *  matrixA,
  const TQ3Matrix4x4 *  matrixB,
  TQ3Matrix4x4 *        result);



/*
 *  Q3Matrix3x3_SetTranslate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_SetTranslate(
  TQ3Matrix3x3 *  matrix3x3,
  float           xTrans,
  float           yTrans);


/*
 *  Q3Matrix3x3_SetScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_SetScale(
  TQ3Matrix3x3 *  matrix3x3,
  float           xScale,
  float           yScale);



/*
 *  Q3Matrix3x3_SetRotateAboutPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix3x3 * )
Q3Matrix3x3_SetRotateAboutPoint(
  TQ3Matrix3x3 *      matrix3x3,
  const TQ3Point2D *  origin,
  float               angle);


/*
 *  Q3Matrix4x4_SetTranslate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetTranslate(
  TQ3Matrix4x4 *  matrix4x4,
  float           xTrans,
  float           yTrans,
  float           zTrans);


/*
 *  Q3Matrix4x4_SetScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetScale(
  TQ3Matrix4x4 *  matrix4x4,
  float           xScale,
  float           yScale,
  float           zScale);



/*
 *  Q3Matrix4x4_SetRotateAboutPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotateAboutPoint(
  TQ3Matrix4x4 *      matrix4x4,
  const TQ3Point3D *  origin,
  float               xAngle,
  float               yAngle,
  float               zAngle);


/*
 *  Q3Matrix4x4_SetRotateAboutAxis()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotateAboutAxis(
  TQ3Matrix4x4 *       matrix4x4,
  const TQ3Point3D *   origin,
  const TQ3Vector3D *  orientation,
  float                angle);


/*
 *  Q3Matrix4x4_SetRotate_X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotate_X(
  TQ3Matrix4x4 *  matrix4x4,
  float           angle);


/*
 *  Q3Matrix4x4_SetRotate_Y()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotate_Y(
  TQ3Matrix4x4 *  matrix4x4,
  float           angle);


/*
 *  Q3Matrix4x4_SetRotate_Z()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotate_Z(
  TQ3Matrix4x4 *  matrix4x4,
  float           angle);


/*
 *  Q3Matrix4x4_SetRotate_XYZ()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotate_XYZ(
  TQ3Matrix4x4 *  matrix4x4,
  float           xAngle,
  float           yAngle,
  float           zAngle);


/*
 *  Q3Matrix4x4_SetRotateVectorToVector()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetRotateVectorToVector(
  TQ3Matrix4x4 *       matrix4x4,
  const TQ3Vector3D *  v1,
  const TQ3Vector3D *  v2);


/*
 *  Q3Matrix4x4_SetQuaternion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Matrix4x4 * )
Q3Matrix4x4_SetQuaternion(
  TQ3Matrix4x4 *         matrix,
  const TQ3Quaternion *  quaternion);


/*
 *  Q3Matrix3x3_Determinant()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Matrix3x3_Determinant(const TQ3Matrix3x3 * matrix3x3);


/*
 *  Q3Matrix4x4_Determinant()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Matrix4x4_Determinant(const TQ3Matrix4x4 * matrix4x4);



/******************************************************************************
 **                                                                          **
 **                             Quaternion Routines                          **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3Quaternion_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_Set(
  TQ3Quaternion *  quaternion,
  float            w,
  float            x,
  float            y,
  float            z);


/*
 *  Q3Quaternion_SetIdentity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetIdentity(TQ3Quaternion * quaternion);


/*
 *  Q3Quaternion_Copy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_Copy(
  const TQ3Quaternion *  quaternion,
  TQ3Quaternion *        result);


/*
 *  Q3Quaternion_IsIdentity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Boolean )
Q3Quaternion_IsIdentity(const TQ3Quaternion * quaternion);


/*
 *  Q3Quaternion_Invert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_Invert(
  const TQ3Quaternion *  quaternion,
  TQ3Quaternion *        result);


/*
 *  Q3Quaternion_Normalize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_Normalize(
  const TQ3Quaternion *  quaternion,
  TQ3Quaternion *        result);


/*
 *  Q3Quaternion_Dot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( float )
Q3Quaternion_Dot(
  const TQ3Quaternion *  q1,
  const TQ3Quaternion *  q2);


/*
 *  Q3Quaternion_Multiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_Multiply(
  const TQ3Quaternion *  q1,
  const TQ3Quaternion *  q2,
  TQ3Quaternion *        result);


/*
 *  Q3Quaternion_SetRotateAboutAxis()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetRotateAboutAxis(
  TQ3Quaternion *      quaternion,
  const TQ3Vector3D *  axis,
  float                angle);


/*
 *  Q3Quaternion_SetRotate_XYZ()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetRotate_XYZ(
  TQ3Quaternion *  quaternion,
  float            xAngle,
  float            yAngle,
  float            zAngle);


/*
 *  Q3Quaternion_SetRotate_X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetRotate_X(
  TQ3Quaternion *  quaternion,
  float            angle);


/*
 *  Q3Quaternion_SetRotate_Y()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetRotate_Y(
  TQ3Quaternion *  quaternion,
  float            angle);


/*
 *  Q3Quaternion_SetRotate_Z()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetRotate_Z(
  TQ3Quaternion *  quaternion,
  float            angle);



/*
 *  Q3Quaternion_SetMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetMatrix(
  TQ3Quaternion *       quaternion,
  const TQ3Matrix4x4 *  matrix);


/*
 *  Q3Quaternion_SetRotateVectorToVector()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_SetRotateVectorToVector(
  TQ3Quaternion *      quaternion,
  const TQ3Vector3D *  v1,
  const TQ3Vector3D *  v2);


/*
 *  Q3Quaternion_MatchReflection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_MatchReflection(
  const TQ3Quaternion *  q1,
  const TQ3Quaternion *  q2,
  TQ3Quaternion *        result);


/*
 *  Q3Quaternion_InterpolateFast()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_InterpolateFast(
  const TQ3Quaternion *  q1,
  const TQ3Quaternion *  q2,
  float                  t,
  TQ3Quaternion *        result);


/*
 *  Q3Quaternion_InterpolateLinear()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Quaternion * )
Q3Quaternion_InterpolateLinear(
  const TQ3Quaternion *  q1,
  const TQ3Quaternion *  q2,
  float                  t,
  TQ3Quaternion *        result);


/*
 *  Q3Vector3D_TransformQuaternion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Vector3D * )
Q3Vector3D_TransformQuaternion(
  const TQ3Vector3D *    vector3D,
  const TQ3Quaternion *  quaternion,
  TQ3Vector3D *          result);


/*
 *  Q3Point3D_TransformQuaternion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Point3D * )
Q3Point3D_TransformQuaternion(
  const TQ3Point3D *     point3D,
  const TQ3Quaternion *  quaternion,
  TQ3Point3D *           result);



/******************************************************************************
 **                                                                          **
 **                             Volume Routines                              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3BoundingBox_Copy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_Copy(
  const TQ3BoundingBox *  src,
  TQ3BoundingBox *        dest);


/*
 *  Q3BoundingBox_Union()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_Union(
  const TQ3BoundingBox *  v1,
  const TQ3BoundingBox *  v2,
  TQ3BoundingBox *        result);


/*
 *  Q3BoundingBox_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_Set(
  TQ3BoundingBox *    bBox,
  const TQ3Point3D *  min,
  const TQ3Point3D *  max,
  TQ3Boolean          isEmpty);


/*
 *  Q3BoundingBox_UnionPoint3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_UnionPoint3D(
  const TQ3BoundingBox *  bBox,
  const TQ3Point3D *      point3D,
  TQ3BoundingBox *        result);


/*
 *  Q3BoundingBox_UnionRationalPoint4D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_UnionRationalPoint4D(
  const TQ3BoundingBox *      bBox,
  const TQ3RationalPoint4D *  point4D,
  TQ3BoundingBox *            result);


/*
 *  Q3BoundingBox_SetFromPoints3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_SetFromPoints3D(
  TQ3BoundingBox *    bBox,
  const TQ3Point3D *  points3D,
  unsigned long       numPoints,
  unsigned long       structSize);


/*
 *  Q3BoundingBox_SetFromRationalPoints4D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingBox * )
Q3BoundingBox_SetFromRationalPoints4D(
  TQ3BoundingBox *            bBox,
  const TQ3RationalPoint4D *  points4D,
  unsigned long               numPoints,
  unsigned long               structSize);



/******************************************************************************
 **                                                                          **
 **                             Sphere Routines                              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3BoundingSphere_Copy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_Copy(
  const TQ3BoundingSphere *  src,
  TQ3BoundingSphere *        dest);


/*
 *  Q3BoundingSphere_Union()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_Union(
  const TQ3BoundingSphere *  s1,
  const TQ3BoundingSphere *  s2,
  TQ3BoundingSphere *        result);


/*
 *  Q3BoundingSphere_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_Set(
  TQ3BoundingSphere *  bSphere,
  const TQ3Point3D *   origin,
  float                radius,
  TQ3Boolean           isEmpty);


/*
 *  Q3BoundingSphere_UnionPoint3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_UnionPoint3D(
  const TQ3BoundingSphere *  bSphere,
  const TQ3Point3D *         point3D,
  TQ3BoundingSphere *        result);


/*
 *  Q3BoundingSphere_UnionRationalPoint4D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_UnionRationalPoint4D(
  const TQ3BoundingSphere *   bSphere,
  const TQ3RationalPoint4D *  point4D,
  TQ3BoundingSphere *         result);



/*
 *  Q3BoundingSphere_SetFromPoints3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_SetFromPoints3D(
  TQ3BoundingSphere *  bSphere,
  const TQ3Point3D *   points3D,
  unsigned long        numPoints,
  unsigned long        structSize);


/*
 *  Q3BoundingSphere_SetFromRationalPoints4D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3BoundingSphere * )
Q3BoundingSphere_SetFromRationalPoints4D(
  TQ3BoundingSphere *         bSphere,
  const TQ3RationalPoint4D *  points4D,
  unsigned long               numPoints,
  unsigned long               structSize);





#endif  /* CALL_NOT_IN_CARBON */


#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __QD3DMATH__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__QD3DMATH__RESTORE_PACKED_ENUMS)
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

#endif /* __QD3DMATH__ */

