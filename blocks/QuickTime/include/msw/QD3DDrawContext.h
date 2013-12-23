/*
     File:       QD3DDrawContext.h
 
     Contains:   Draw context class types and routines
 
     Version:    Technology: Quickdraw 3D 1.6
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1995-1999 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QD3DDRAWCONTEXT__
#define __QD3DDRAWCONTEXT__

#ifndef __QD3D__
#include <QD3D.h>
#endif


#if TARGET_OS_MAC
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __FIXMATH__
#include <FixMath.h>
#endif

#ifndef __GXTYPES__
#include <GXTypes.h>
#endif

#endif  /* TARGET_OS_MAC */


#if TARGET_OS_UNIX
    #include <X11/Xlib.h>
  #include <X11/Xutil.h>
#endif   /* TARGET_OS_WIN32 */


#if TARGET_OS_WIN32
   /******************************************************************************
     *                                                                           **
     * ABOUT   QD3D_NO_DIRECTDRAW:   (Win32 Only)                                **
     *                                                                           **
     * NOTE: Define QD3D_NO_DIRECTDRAW for your application makefile/project     **
     *       only if you don't need Q3DDSurfaceDrawContext support and don't     **
     *       have access to ddraw.h.                                             ** 
    *                                                                           **
     *****************************************************************************/
    #include <windows.h>
   #if !defined(QD3D_NO_DIRECTDRAW)
       #include <ddraw.h>
 #endif /* !QD3D_NO_DIRECTDRAW */
#endif  /*  TARGET_OS_WIN32  */



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
        #define __QD3DDRAWCONTEXT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __QD3DDRAWCONTEXT__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/******************************************************************************
 **                                                                          **
 **                         DrawContext Data Structures                      **
 **                                                                          **
 *****************************************************************************/
enum TQ3DrawContextClearImageMethod {
  kQ3ClearMethodNone            = 0,
  kQ3ClearMethodWithColor       = 1
};
typedef enum TQ3DrawContextClearImageMethod TQ3DrawContextClearImageMethod;


struct TQ3DrawContextData {
  TQ3DrawContextClearImageMethod  clearImageMethod;
  TQ3ColorARGB        clearImageColor;
  TQ3Area             pane;
  TQ3Boolean          paneState;
  TQ3Bitmap           mask;
  TQ3Boolean          maskState;
  TQ3Boolean          doubleBufferState;
};
typedef struct TQ3DrawContextData       TQ3DrawContextData;
/******************************************************************************
 **                                                                          **
 **                             DrawContext Routines                         **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3DrawContext_GetType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ObjectType )
Q3DrawContext_GetType(TQ3DrawContextObject drawContext);


/*
 *  Q3DrawContext_SetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetData(
  TQ3DrawContextObject        context,
  const TQ3DrawContextData *  contextData);


/*
 *  Q3DrawContext_GetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetData(
  TQ3DrawContextObject   context,
  TQ3DrawContextData *   contextData);


/*
 *  Q3DrawContext_SetClearImageColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetClearImageColor(
  TQ3DrawContextObject   context,
  const TQ3ColorARGB *   color);


/*
 *  Q3DrawContext_GetClearImageColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetClearImageColor(
  TQ3DrawContextObject   context,
  TQ3ColorARGB *         color);


/*
 *  Q3DrawContext_SetPane()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetPane(
  TQ3DrawContextObject   context,
  const TQ3Area *        pane);


/*
 *  Q3DrawContext_GetPane()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetPane(
  TQ3DrawContextObject   context,
  TQ3Area *              pane);


/*
 *  Q3DrawContext_SetPaneState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetPaneState(
  TQ3DrawContextObject   context,
  TQ3Boolean             state);


/*
 *  Q3DrawContext_GetPaneState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetPaneState(
  TQ3DrawContextObject   context,
  TQ3Boolean *           state);


/*
 *  Q3DrawContext_SetClearImageMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetClearImageMethod(
  TQ3DrawContextObject             context,
  TQ3DrawContextClearImageMethod   method);


/*
 *  Q3DrawContext_GetClearImageMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetClearImageMethod(
  TQ3DrawContextObject              context,
  TQ3DrawContextClearImageMethod *  method);


/*
 *  Q3DrawContext_SetMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetMask(
  TQ3DrawContextObject   context,
  const TQ3Bitmap *      mask);


/*
 *  Q3DrawContext_GetMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetMask(
  TQ3DrawContextObject   context,
  TQ3Bitmap *            mask);


/*
 *  Q3DrawContext_SetMaskState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetMaskState(
  TQ3DrawContextObject   context,
  TQ3Boolean             state);


/*
 *  Q3DrawContext_GetMaskState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetMaskState(
  TQ3DrawContextObject   context,
  TQ3Boolean *           state);


/*
 *  Q3DrawContext_SetDoubleBufferState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_SetDoubleBufferState(
  TQ3DrawContextObject   context,
  TQ3Boolean             state);


/*
 *  Q3DrawContext_GetDoubleBufferState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DrawContext_GetDoubleBufferState(
  TQ3DrawContextObject   context,
  TQ3Boolean *           state);



/******************************************************************************
 **                                                                          **
 **                         Pixmap Data Structure                            **
 **                                                                          **
 *****************************************************************************/
#endif  /* CALL_NOT_IN_CARBON */

struct TQ3PixmapDrawContextData {
  TQ3DrawContextData  drawContextData;
  TQ3Pixmap           pixmap;
};
typedef struct TQ3PixmapDrawContextData TQ3PixmapDrawContextData;
/******************************************************************************
 **                                                                          **
 **                     Pixmap DrawContext Routines                          **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3PixmapDrawContext_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3DrawContextObject )
Q3PixmapDrawContext_New(const TQ3PixmapDrawContextData * contextData);


/*
 *  Q3PixmapDrawContext_SetPixmap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3PixmapDrawContext_SetPixmap(
  TQ3DrawContextObject   drawContext,
  const TQ3Pixmap *      pixmap);


/*
 *  Q3PixmapDrawContext_GetPixmap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3PixmapDrawContext_GetPixmap(
  TQ3DrawContextObject   drawContext,
  TQ3Pixmap *            pixmap);




#endif  /* CALL_NOT_IN_CARBON */

#if TARGET_OS_MAC
/******************************************************************************
 **                                                                          **
 **                     Macintosh DrawContext Data Structures                **
 **                                                                          **
 *****************************************************************************/
enum TQ3MacDrawContext2DLibrary {
  kQ3Mac2DLibraryNone           = 0,
  kQ3Mac2DLibraryQuickDraw      = 1,
  kQ3Mac2DLibraryQuickDrawGX    = 2
};
typedef enum TQ3MacDrawContext2DLibrary TQ3MacDrawContext2DLibrary;


struct TQ3MacDrawContextData {
  TQ3DrawContextData  drawContextData;
  CWindowPtr          window;
  TQ3MacDrawContext2DLibrary  library;
  gxViewPort          viewPort;
  CGrafPtr            grafPort;
};
typedef struct TQ3MacDrawContextData    TQ3MacDrawContextData;
/******************************************************************************
 **                                                                          **
 **                     Macintosh DrawContext Routines                       **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3MacDrawContext_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3DrawContextObject )
Q3MacDrawContext_New(const TQ3MacDrawContextData * drawContextData);


/*
 *  Q3MacDrawContext_SetWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_SetWindow(
  TQ3DrawContextObject   drawContext,
  CWindowPtr             window);


/*
 *  Q3MacDrawContext_GetWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_GetWindow(
  TQ3DrawContextObject   drawContext,
  CWindowPtr *           window);


/*
 *  Q3MacDrawContext_SetGXViewPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_SetGXViewPort(
  TQ3DrawContextObject   drawContext,
  gxViewPort             viewPort);


/*
 *  Q3MacDrawContext_GetGXViewPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_GetGXViewPort(
  TQ3DrawContextObject   drawContext,
  gxViewPort *           viewPort);


/*
 *  Q3MacDrawContext_SetGrafPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_SetGrafPort(
  TQ3DrawContextObject   drawContext,
  CGrafPtr               grafPort);


/*
 *  Q3MacDrawContext_GetGrafPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_GetGrafPort(
  TQ3DrawContextObject   drawContext,
  CGrafPtr *             grafPort);


/*
 *  Q3MacDrawContext_Set2DLibrary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_Set2DLibrary(
  TQ3DrawContextObject         drawContext,
  TQ3MacDrawContext2DLibrary   library);


/*
 *  Q3MacDrawContext_Get2DLibrary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3MacDrawContext_Get2DLibrary(
  TQ3DrawContextObject          drawContext,
  TQ3MacDrawContext2DLibrary *  library);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_MAC */

#if TARGET_OS_UNIX
/******************************************************************************
 **                                                                          **
 **                     X/Windows DrawContext Data Structures                **
 **                                                                          **
 *****************************************************************************/
typedef struct OpaqueTQ3XBufferObject*  TQ3XBufferObject;
struct TQ3XColormapData {
  long                baseEntry;
  long                maxRed;
  long                maxGreen;
  long                maxBlue;
  long                multRed;
  long                multGreen;
  long                multBlue;
};
typedef struct TQ3XColormapData         TQ3XColormapData;
struct TQ3XDrawContextData {
  TQ3DrawContextData  contextData;
  Display *           display;
  Drawable            drawable;
  Visual *            visual;
  Colormap            cmap;
  TQ3XColormapData *  colorMapData;
};
typedef struct TQ3XDrawContextData      TQ3XDrawContextData;
/******************************************************************************
 **                                                                          **
 **                     X/Windows DrawContext Routines                       **
 **                                                                          **
 *****************************************************************************/
#ifdef XDC_OLD
#if CALL_NOT_IN_CARBON
/*
 *  Q3XDrawContext_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3DrawContextObject )
Q3XDrawContext_New(void);


/*
 *  Q3XDrawContext_Set()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
Q3XDrawContext_Set(
  TQ3DrawContextObject   drawContext,
  unsigned long          flag,
  void *                 data);


/*
 *  Q3XDrawContext_Get()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
Q3XDrawContext_Get(
  TQ3DrawContextObject   drawContext,
  unsigned long          flag,
  void *                 data);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* defined(XDC_OLD) */

#if CALL_NOT_IN_CARBON
/*
 *  Q3XBuffers_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3XBufferObject )
Q3XBuffers_New(
  Display *       dpy,
  unsigned long   numBuffers,
  Window          window);


/*
 *  Q3XBuffers_Swap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
Q3XBuffers_Swap(
  Display *          dpy,
  TQ3XBufferObject   buffers);


/*
 *  Q3X_GetVisualInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( XVisualInfo * )
Q3X_GetVisualInfo(
  Display *  dpy,
  Screen *   screen);



/*
 *  Q3XDrawContext_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3DrawContextObject )
Q3XDrawContext_New(const TQ3XDrawContextData * xContextData);


/*
 *  Q3XDrawContext_SetDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_SetDisplay(
  TQ3DrawContextObject   drawContext,
  const Display *        display);


/*
 *  Q3XDrawContext_GetDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_GetDisplay(
  TQ3DrawContextObject   drawContext,
  Display **             display);


/*
 *  Q3XDrawContext_SetDrawable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_SetDrawable(
  TQ3DrawContextObject   drawContext,
  Drawable               drawable);


/*
 *  Q3XDrawContext_GetDrawable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_GetDrawable(
  TQ3DrawContextObject   drawContext,
  Drawable *             drawable);


/*
 *  Q3XDrawContext_SetVisual()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_SetVisual(
  TQ3DrawContextObject   drawContext,
  const Visual *         visual);


/*
 *  Q3XDrawContext_GetVisual()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_GetVisual(
  TQ3DrawContextObject   drawContext,
  Visual **              visual);


/*
 *  Q3XDrawContext_SetColormap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_SetColormap(
  TQ3DrawContextObject   drawContext,
  Colormap               colormap);


/*
 *  Q3XDrawContext_GetColormap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_GetColormap(
  TQ3DrawContextObject   drawContext,
  Colormap *             colormap);


/*
 *  Q3XDrawContext_SetColormapData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_SetColormapData(
  TQ3DrawContextObject      drawContext,
  const TQ3XColormapData *  colormapData);


/*
 *  Q3XDrawContext_GetColormapData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3XDrawContext_GetColormapData(
  TQ3DrawContextObject   drawContext,
  TQ3XColormapData *     colormapData);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_UNIX */

#if TARGET_OS_WIN32
/******************************************************************************
 **                                                                          **
 **                      Win32 DrawContext Data Structures                   **
 **                                                                          **
 *****************************************************************************/
struct TQ3Win32DCDrawContextData {
  TQ3DrawContextData  drawContextData;
  HDC                 hdc;
};
typedef struct TQ3Win32DCDrawContextData TQ3Win32DCDrawContextData;
#ifndef QD3D_NO_DIRECTDRAW
enum TQ3DirectDrawObjectSelector {
  kQ3DirectDrawObject           = 1,
  kQ3DirectDrawObject2          = 2
};
typedef enum TQ3DirectDrawObjectSelector TQ3DirectDrawObjectSelector;

enum TQ3DirectDrawSurfaceSelector {
  kQ3DirectDrawSurface          = 1,
  kQ3DirectDrawSurface2         = 2
};
typedef enum TQ3DirectDrawSurfaceSelector TQ3DirectDrawSurfaceSelector;


struct TQ3DDSurfaceDescriptor {
   TQ3DirectDrawObjectSelector     objectSelector;
    union
  {
      LPDIRECTDRAW                lpDirectDraw;
      LPDIRECTDRAW2               lpDirectDraw2;
 };

    TQ3DirectDrawSurfaceSelector    surfaceSelector;
   union
  {
      LPDIRECTDRAWSURFACE         lpDirectDrawSurface;
       LPDIRECTDRAWSURFACE2        lpDirectDrawSurface2;
  };
};
typedef struct TQ3DDSurfaceDescriptor TQ3DDSurfaceDescriptor;

struct TQ3DDSurfaceDrawContextData {
  TQ3DrawContextData              drawContextData;
   TQ3DDSurfaceDescriptor          ddSurfaceDescriptor;
};
typedef struct TQ3DDSurfaceDrawContextData TQ3DDSurfaceDrawContextData;

#endif  /* !defined(QD3D_NO_DIRECTDRAW) */

/******************************************************************************
 **                                                                          **
 **                         Win32DC DrawContext Routines                     **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3Win32DCDrawContext_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3DrawContextObject )
Q3Win32DCDrawContext_New(const TQ3Win32DCDrawContextData * drawContextData);


/*
 *  Q3Win32DCDrawContext_SetDC()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Win32DCDrawContext_SetDC(
  TQ3DrawContextObject   drawContext,
  HDC                    newHDC);


/*
 *  Q3Win32DCDrawContext_GetDC()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3Win32DCDrawContext_GetDC(
  TQ3DrawContextObject   drawContext,
  HDC *                  curHDC);


/******************************************************************************
 **                                                                          **
 **                         DDSurface DrawContext Routines                   **
 **                                                                          **
 *****************************************************************************/
#endif  /* CALL_NOT_IN_CARBON */

#ifndef QD3D_NO_DIRECTDRAW
#if CALL_NOT_IN_CARBON
/*
 *  Q3DDSurfaceDrawContext_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3DrawContextObject )
Q3DDSurfaceDrawContext_New(const TQ3DDSurfaceDrawContextData * drawContextData);


/*
 *  Q3DDSurfaceDrawContext_SetDirectDrawSurface()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DDSurfaceDrawContext_SetDirectDrawSurface(
  TQ3DrawContextObject            drawContext,
  const TQ3DDSurfaceDescriptor *  ddSurfaceDescriptor);


/*
 *  Q3DDSurfaceDrawContext_GetDirectDrawSurface()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3Status )
Q3DDSurfaceDrawContext_GetDirectDrawSurface(
  TQ3DrawContextObject      drawContext,
  TQ3DDSurfaceDescriptor *  ddSurfaceDescriptor);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !defined(QD3D_NO_DIRECTDRAW) */

#endif  /* TARGET_OS_WIN32 */





#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __QD3DDRAWCONTEXT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__QD3DDRAWCONTEXT__RESTORE_PACKED_ENUMS)
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

#endif /* __QD3DDRAWCONTEXT__ */

