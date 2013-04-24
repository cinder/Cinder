/*
     File:       ColorPicker.h
 
     Contains:   Color Picker package Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1987-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __COLORPICKER__
#define __COLORPICKER__

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __CMAPPLICATION__
#include <CMApplication.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
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

enum {
                                        /*Maximum small fract value, as long*/
  kMaximumSmallFract            = 0x0000FFFF
};

enum {
  kDefaultColorPickerWidth      = 383,
  kDefaultColorPickerHeight     = 238
};

typedef SInt16 DialogPlacementSpec;
enum {
  kAtSpecifiedOrigin            = 0,
  kDeepestColorScreen           = 1,
  kCenterOnMainScreen           = 2
};


/* These are for the flags field in the structs below (for example ColorPickerInfo). */
enum {
  kColorPickerDialogIsMoveable  = 1,
  kColorPickerDialogIsModal     = 2,
  kColorPickerCanModifyPalette  = 4,
  kColorPickerCanAnimatePalette = 8,
  kColorPickerAppIsColorSyncAware = 16,
  kColorPickerInSystemDialog    = 32,
  kColorPickerInApplicationDialog = 64,
  kColorPickerInPickerDialog    = 128,
  kColorPickerDetachedFromChoices = 256,
  kColorPickerCallColorProcLive = 512
};


#if OLDROUTINENAMES
enum {
                                        /*Maximum small fract value, as long*/
  MaxSmallFract                 = 0x0000FFFF
};

enum {
  kDefaultWidth                 = 383,
  kDefaultHeight                = 238
};

/* These are for the flags field in the structs below (for example ColorPickerInfo). */
enum {
  DialogIsMoveable              = 1,
  DialogIsModal                 = 2,
  CanModifyPalette              = 4,
  CanAnimatePalette             = 8,
  AppIsColorSyncAware           = 16,
  InSystemDialog                = 32,
  InApplicationDialog           = 64,
  InPickerDialog                = 128,
  DetachedFromChoices           = 256,
  CallColorProcLive             = 512
};

#endif  /* OLDROUTINENAMES */

/* A SmallFract value is just the fractional part of a Fixed number,
which is the low order word.  SmallFracts are used to save room,
and to be compatible with Quickdraw's RGBColor.  They can be
assigned directly to and from INTEGERs. */
/* Unsigned fraction between 0 and 1 */
typedef unsigned short                  SmallFract;
/* For developmental simplicity in switching between the HLS and HSV
models, HLS is reordered into HSL. Thus both models start with
hue and saturation values; value/lightness/brightness is last. */

struct HSVColor {
  SmallFract          hue;                    /*Fraction of circle, red at 0*/
  SmallFract          saturation;             /*0-1, 0 for gray, 1 for pure color*/
  SmallFract          value;                  /*0-1, 0 for black, 1 for max intensity*/
};
typedef struct HSVColor                 HSVColor;
struct HSLColor {
  SmallFract          hue;                    /*Fraction of circle, red at 0*/
  SmallFract          saturation;             /*0-1, 0 for gray, 1 for pure color*/
  SmallFract          lightness;              /*0-1, 0 for black, 1 for white*/
};
typedef struct HSLColor                 HSLColor;
struct CMYColor {
  SmallFract          cyan;
  SmallFract          magenta;
  SmallFract          yellow;
};
typedef struct CMYColor                 CMYColor;
struct PMColor {
  CMProfileHandle     profile;
  CMColor             color;
};
typedef struct PMColor                  PMColor;
typedef PMColor *                       PMColorPtr;
struct NPMColor {
  CMProfileRef        profile;
  CMColor             color;
};
typedef struct NPMColor                 NPMColor;
typedef NPMColor *                      NPMColorPtr;
typedef struct OpaquePicker*            Picker;
typedef Picker picker;
struct PickerMenuItemInfo {
  short               editMenuID;
  short               cutItem;
  short               copyItem;
  short               pasteItem;
  short               clearItem;
  short               undoItem;
};
typedef struct PickerMenuItemInfo       PickerMenuItemInfo;
/* Structs related to deprecated API's have been pulled from this file. */
/* Those structs necessary for developers writing their own color pickers... */
/* have been moved to ColorPickerComponents.h. */

typedef CALLBACK_API( void , ColorChangedProcPtr )(long userData, PMColor *newColor);
typedef CALLBACK_API( void , NColorChangedProcPtr )(long userData, NPMColor *newColor);
typedef CALLBACK_API( Boolean , UserEventProcPtr )(EventRecord * event);
typedef STACK_UPP_TYPE(ColorChangedProcPtr)                     ColorChangedUPP;
typedef STACK_UPP_TYPE(NColorChangedProcPtr)                    NColorChangedUPP;
typedef STACK_UPP_TYPE(UserEventProcPtr)                        UserEventUPP;
struct ColorPickerInfo {
  PMColor             theColor;
  CMProfileHandle     dstProfile;
  UInt32              flags;
  DialogPlacementSpec  placeWhere;
  Point               dialogOrigin;
  OSType              pickerType;
  UserEventUPP        eventProc;
  ColorChangedUPP     colorProc;
  UInt32              colorProcData;
  Str255              prompt;
  PickerMenuItemInfo  mInfo;
  Boolean             newColorChosen;
  SInt8               filler;
};
typedef struct ColorPickerInfo          ColorPickerInfo;
struct NColorPickerInfo {
  NPMColor            theColor;
  CMProfileRef        dstProfile;
  UInt32              flags;
  DialogPlacementSpec  placeWhere;
  Point               dialogOrigin;
  OSType              pickerType;
  UserEventUPP        eventProc;
  NColorChangedUPP    colorProc;
  UInt32              colorProcData;
  Str255              prompt;
  PickerMenuItemInfo  mInfo;
  Boolean             newColorChosen;
  UInt8               reserved;               /*Must be 0*/
};
typedef struct NColorPickerInfo         NColorPickerInfo;

/*  Below are the color conversion routines.*/
/*
 *  Fix2SmallFract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( SmallFract )
Fix2SmallFract(Fixed f)                                       THREEWORDINLINE(0x3F3C, 0x0001, 0xA82E);


/*
 *  SmallFract2Fix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( Fixed )
SmallFract2Fix(SmallFract s)                                  THREEWORDINLINE(0x3F3C, 0x0002, 0xA82E);


/*
 *  CMY2RGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CMY2RGB(
  const CMYColor *  cColor,
  RGBColor *        rColor)                                   THREEWORDINLINE(0x3F3C, 0x0003, 0xA82E);


/*
 *  RGB2CMY()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( void )
RGB2CMY(
  const RGBColor *  rColor,
  CMYColor *        cColor)                                   THREEWORDINLINE(0x3F3C, 0x0004, 0xA82E);


/*
 *  HSL2RGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( void )
HSL2RGB(
  const HSLColor *  hColor,
  RGBColor *        rColor)                                   THREEWORDINLINE(0x3F3C, 0x0005, 0xA82E);


/*
 *  RGB2HSL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( void )
RGB2HSL(
  const RGBColor *  rColor,
  HSLColor *        hColor)                                   THREEWORDINLINE(0x3F3C, 0x0006, 0xA82E);


/*
 *  HSV2RGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( void )
HSV2RGB(
  const HSVColor *  hColor,
  RGBColor *        rColor)                                   THREEWORDINLINE(0x3F3C, 0x0007, 0xA82E);


/*
 *  RGB2HSV()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( void )
RGB2HSV(
  const RGBColor *  rColor,
  HSVColor *        hColor)                                   THREEWORDINLINE(0x3F3C, 0x0008, 0xA82E);



/*  GetColor() works with or without the Color Picker extension.*/
/*
 *  GetColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
GetColor(
  Point              where,
  ConstStr255Param   prompt,
  const RGBColor *   inColor,
  RGBColor *         outColor)                                THREEWORDINLINE(0x3F3C, 0x0009, 0xA82E);



/*  PickColor() requires the Color Picker extension (version 2.0 or greater).*/
/*
 *  PickColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ColorPickerLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
PickColor(ColorPickerInfo * theColorInfo)                     THREEWORDINLINE(0x3F3C, 0x0213, 0xA82E);


/*  NPickColor() requires the Color Picker extension (version 2.1 or greater).*/
/*
 *  NPickColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ColorPickerLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
NPickColor(NColorPickerInfo * theColorInfo)                   THREEWORDINLINE(0x3F3C, 0x0227, 0xA82E);



/* A suite of mid-level API calls have been deprecated.  Likely you never...  */
/* used them anyway.  They were removed from this file and should not be... */
/* used in the future as they are not gauranteed to be supported. */
/*
 *  NewColorChangedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( ColorChangedUPP )
NewColorChangedUPP(ColorChangedProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppColorChangedProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ColorChangedUPP) NewColorChangedUPP(ColorChangedProcPtr userRoutine) { return (ColorChangedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppColorChangedProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewColorChangedUPP(userRoutine) (ColorChangedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppColorChangedProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewNColorChangedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( NColorChangedUPP )
NewNColorChangedUPP(NColorChangedProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNColorChangedProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NColorChangedUPP) NewNColorChangedUPP(NColorChangedProcPtr userRoutine) { return (NColorChangedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNColorChangedProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNColorChangedUPP(userRoutine) (NColorChangedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNColorChangedProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewUserEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( UserEventUPP )
NewUserEventUPP(UserEventProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppUserEventProcInfo = 0x000000D0 };  /* pascal 1_byte Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(UserEventUPP) NewUserEventUPP(UserEventProcPtr userRoutine) { return (UserEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppUserEventProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewUserEventUPP(userRoutine) (UserEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppUserEventProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeColorChangedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeColorChangedUPP(ColorChangedUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeColorChangedUPP(ColorChangedUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeColorChangedUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeNColorChangedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeNColorChangedUPP(NColorChangedUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNColorChangedUPP(NColorChangedUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNColorChangedUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeUserEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeUserEventUPP(UserEventUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeUserEventUPP(UserEventUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeUserEventUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeColorChangedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
InvokeColorChangedUPP(
  long             userData,
  PMColor *        newColor,
  ColorChangedUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeColorChangedUPP(long userData, PMColor * newColor, ColorChangedUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppColorChangedProcInfo, userData, newColor); }
  #else
    #define InvokeColorChangedUPP(userData, newColor, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppColorChangedProcInfo, (userData), (newColor))
  #endif
#endif

/*
 *  InvokeNColorChangedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
InvokeNColorChangedUPP(
  long              userData,
  NPMColor *        newColor,
  NColorChangedUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeNColorChangedUPP(long userData, NPMColor * newColor, NColorChangedUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppNColorChangedProcInfo, userData, newColor); }
  #else
    #define InvokeNColorChangedUPP(userData, newColor, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppNColorChangedProcInfo, (userData), (newColor))
  #endif
#endif

/*
 *  InvokeUserEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InvokeUserEventUPP(
  EventRecord *  event,
  UserEventUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeUserEventUPP(EventRecord * event, UserEventUPP userUPP) { return (Boolean)CALL_ONE_PARAMETER_UPP(userUPP, uppUserEventProcInfo, event); }
  #else
    #define InvokeUserEventUPP(event, userUPP) (Boolean)CALL_ONE_PARAMETER_UPP((userUPP), uppUserEventProcInfo, (event))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewColorChangedProc(userRoutine)                    NewColorChangedUPP(userRoutine)
    #define NewNColorChangedProc(userRoutine)                   NewNColorChangedUPP(userRoutine)
    #define NewUserEventProc(userRoutine)                       NewUserEventUPP(userRoutine)
    #define CallColorChangedProc(userRoutine, userData, newColor) InvokeColorChangedUPP(userData, newColor, userRoutine)
    #define CallNColorChangedProc(userRoutine, userData, newColor) InvokeNColorChangedUPP(userData, newColor, userRoutine)
    #define CallUserEventProc(userRoutine, event)               InvokeUserEventUPP(event, userRoutine)
#endif /* CALL_NOT_IN_CARBON */



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

#endif /* __COLORPICKER__ */

