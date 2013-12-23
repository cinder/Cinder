/*
     File:       QuickdrawText.h
 
     Contains:   Quickdraw Text Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1983-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QUICKDRAWTEXT__
#define __QUICKDRAWTEXT__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __INTLRESOURCES__
#include <IntlResources.h>
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

/* new CGrafPort bottleneck ("newProc2") function, used in Unicode Text drawing */
/*
 *  StandardGlyphs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickDrawText 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
StandardGlyphs(
  void *      dataStream,
  ByteCount   size);



enum {
                                        /* CharToPixel directions */
  leftCaret                     = 0,    /*Place caret for left block*/
  rightCaret                    = -1,   /*Place caret for right block*/
  kHilite                       = 1     /*Direction is SysDirection*/
};

enum {
  smLeftCaret                   = 0,    /*Place caret for left block - obsolete */
  smRightCaret                  = -1,   /*Place caret for right block - obsolete */
  smHilite                      = 1     /*Direction is TESysJust - obsolete */
};

enum {
                                        /*Constants for styleRunPosition argument in PortionLine, DrawJustified, MeasureJustified, CharToPixel, and PixelToChar.*/
  onlyStyleRun                  = 0,    /* This is the only style run on the line */
  leftStyleRun                  = 1,    /* This is leftmost of multiple style runs on the line */
  rightStyleRun                 = 2,    /* This is rightmost of multiple style runs on the line */
  middleStyleRun                = 3,    /* There are multiple style runs on the line and this is neither the leftmost nor the rightmost. */
  smOnlyStyleRun                = 0,    /* obsolete */
  smLeftStyleRun                = 1,    /* obsolete */
  smRightStyleRun               = 2,    /* obsolete */
  smMiddleStyleRun              = 3     /* obsolete */
};

/* type for styleRunPosition parameter in PixelToChar etc. */
typedef short                           JustStyleCode;
/* Type for truncWhere parameter in TruncString, TruncText */
typedef short                           TruncCode;
enum {
                                        /* Constants for truncWhere argument in TruncString and TruncText */
  truncEnd                      = 0,    /* Truncate at end */
  truncMiddle                   = 0x4000, /* Truncate in middle */
  smTruncEnd                    = 0,    /* Truncate at end - obsolete */
  smTruncMiddle                 = 0x4000 /* Truncate in middle - obsolete */
};

enum {
                                        /* Constants for TruncString and TruncText results */
  notTruncated                  = 0,    /* No truncation was necessary */
  truncated                     = 1,    /* Truncation performed */
  truncErr                      = -1,   /* General error */
  smNotTruncated                = 0,    /* No truncation was necessary - obsolete */
  smTruncated                   = 1,    /* Truncation performed   - obsolete */
  smTruncErr                    = -1    /* General error - obsolete */
};

typedef SInt8                           StyledLineBreakCode;
enum {
  smBreakWord                   = 0,
  smBreakChar                   = 1,
  smBreakOverflow               = 2
};

/*QuickTime3.0*/
enum {
                                        /* Constants for txFlags (which used to be the pad field after txFace) */
  tfAntiAlias                   = 1 << 0,
  tfUnicode                     = 1 << 1
};


struct FontInfo {
  short               ascent;
  short               descent;
  short               widMax;
  short               leading;
};
typedef struct FontInfo                 FontInfo;

typedef short                           FormatOrder[1];
typedef FormatOrder *                   FormatOrderPtr;
/* FormatStatus was moved to TextUtils.i */
/* OffsetTable moved to IntlResources.i */

typedef CALLBACK_API( Boolean , StyleRunDirectionProcPtr )(short styleRunIndex, void *dirParam);
typedef STACK_UPP_TYPE(StyleRunDirectionProcPtr)                StyleRunDirectionUPP;
/*
 *  NewStyleRunDirectionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( StyleRunDirectionUPP )
NewStyleRunDirectionUPP(StyleRunDirectionProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppStyleRunDirectionProcInfo = 0x00000390 };  /* pascal 1_byte Func(2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(StyleRunDirectionUPP) NewStyleRunDirectionUPP(StyleRunDirectionProcPtr userRoutine) { return (StyleRunDirectionUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppStyleRunDirectionProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewStyleRunDirectionUPP(userRoutine) (StyleRunDirectionUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppStyleRunDirectionProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeStyleRunDirectionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeStyleRunDirectionUPP(StyleRunDirectionUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeStyleRunDirectionUPP(StyleRunDirectionUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeStyleRunDirectionUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeStyleRunDirectionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeStyleRunDirectionUPP(
  short                 styleRunIndex,
  void *                dirParam,
  StyleRunDirectionUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeStyleRunDirectionUPP(short styleRunIndex, void * dirParam, StyleRunDirectionUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppStyleRunDirectionProcInfo, styleRunIndex, dirParam); }
  #else
    #define InvokeStyleRunDirectionUPP(styleRunIndex, dirParam, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppStyleRunDirectionProcInfo, (styleRunIndex), (dirParam))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewStyleRunDirectionProc(userRoutine)               NewStyleRunDirectionUPP(userRoutine)
    #define CallStyleRunDirectionProc(userRoutine, styleRunIndex, dirParam) InvokeStyleRunDirectionUPP(styleRunIndex, dirParam, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  Pixel2Char()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
Pixel2Char(
  Ptr        textBuf,
  short      textLen,
  short      slop,
  short      pixelWidth,
  Boolean *  leadingEdge)                                     FOURWORDINLINE(0x2F3C, 0x820E, 0x0014, 0xA8B5);


/*
 *  Char2Pixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
Char2Pixel(
  Ptr     textBuf,
  short   textLen,
  short   slop,
  short   offset,
  short   direction)                                          FOURWORDINLINE(0x2F3C, 0x820C, 0x0016, 0xA8B5);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  PixelToChar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
PixelToChar(
  Ptr             textBuf,
  long            textLength,
  Fixed           slop,
  Fixed           pixelWidth,
  Boolean *       leadingEdge,
  Fixed *         widthRemaining,
  JustStyleCode   styleRunPosition,
  Point           numer,
  Point           denom)                                      FOURWORDINLINE(0x2F3C, 0x8222, 0x002E, 0xA8B5);


/*
 *  CharToPixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
CharToPixel(
  Ptr             textBuf,
  long            textLength,
  Fixed           slop,
  long            offset,
  short           direction,
  JustStyleCode   styleRunPosition,
  Point           numer,
  Point           denom)                                      FOURWORDINLINE(0x2F3C, 0x821C, 0x0030, 0xA8B5);


/*
 *  DrawJustified()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DrawJustified(
  Ptr             textPtr,
  long            textLength,
  Fixed           slop,
  JustStyleCode   styleRunPosition,
  Point           numer,
  Point           denom)                                      FOURWORDINLINE(0x2F3C, 0x8016, 0x0032, 0xA8B5);


/*
 *  MeasureJustified()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MeasureJustified(
  Ptr             textPtr,
  long            textLength,
  Fixed           slop,
  Ptr             charLocs,
  JustStyleCode   styleRunPosition,
  Point           numer,
  Point           denom)                                      FOURWORDINLINE(0x2F3C, 0x801A, 0x0034, 0xA8B5);


/*
 *  PortionLine()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
PortionLine(
  Ptr             textPtr,
  long            textLen,
  JustStyleCode   styleRunPosition,
  Point           numer,
  Point           denom)                                      FOURWORDINLINE(0x2F3C, 0x8412, 0x0036, 0xA8B5);


/*
 *  HiliteText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HiliteText(
  Ptr           textPtr,
  short         textLength,
  short         firstOffset,
  short         secondOffset,
  OffsetTable   offsets)                                      FOURWORDINLINE(0x2F3C, 0x800E, 0x001C, 0xA8B5);


#if CALL_NOT_IN_CARBON
/*
 *  DrawJust()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
DrawJust(
  Ptr     textPtr,
  short   textLength,
  short   slop)                                               FOURWORDINLINE(0x2F3C, 0x8008, 0x001E, 0xA8B5);


/*
 *  MeasureJust()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
MeasureJust(
  Ptr     textPtr,
  short   textLength,
  short   slop,
  Ptr     charLocs)                                           FOURWORDINLINE(0x2F3C, 0x800C, 0x0020, 0xA8B5);


/*
 *  PortionText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Fixed )
PortionText(
  Ptr    textPtr,
  long   textLength)                                          FOURWORDINLINE(0x2F3C, 0x8408, 0x0024, 0xA8B5);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  VisibleLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
VisibleLength(
  Ptr    textPtr,
  long   textLength)                                          FOURWORDINLINE(0x2F3C, 0x8408, 0x0028, 0xA8B5);


/*
 *  GetFormatOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetFormatOrder(
  FormatOrderPtr         ordering,
  short                  firstFormat,
  short                  lastFormat,
  Boolean                lineRight,
  StyleRunDirectionUPP   rlDirProc,
  Ptr                    dirParam)                            FOURWORDINLINE(0x2F3C, 0x8012, 0xFFFC, 0xA8B5);


/*
 *  TextFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TextFont(short font)                                          ONEWORDINLINE(0xA887);


/*
 *  TextFace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TextFace(StyleParameter face)                                 ONEWORDINLINE(0xA888);


/*
 *  TextMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TextMode(short mode)                                          ONEWORDINLINE(0xA889);


/*
 *  TextSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TextSize(short size)                                          ONEWORDINLINE(0xA88A);


/*
 *  SpaceExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SpaceExtra(Fixed extra)                                       ONEWORDINLINE(0xA88E);


/*
 *  DrawChar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DrawChar(CharParameter ch)                                    ONEWORDINLINE(0xA883);


/*
 *  DrawString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DrawString(ConstStr255Param s)                                ONEWORDINLINE(0xA884);


/*
 *  [Mac]DrawText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacDrawText DrawText
#endif
EXTERN_API( void )
MacDrawText(
  const void *  textBuf,
  short         firstByte,
  short         byteCount)                                    ONEWORDINLINE(0xA885);


/*
 *  CharWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
CharWidth(CharParameter ch)                                   ONEWORDINLINE(0xA88D);


/*
 *  StringWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
StringWidth(ConstStr255Param s)                               ONEWORDINLINE(0xA88C);


/*
 *  TextWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
TextWidth(
  const void *  textBuf,
  short         firstByte,
  short         byteCount)                                    ONEWORDINLINE(0xA886);


/*
 *  MeasureText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MeasureText(
  short         count,
  const void *  textAddr,
  void *        charLocs)                                     ONEWORDINLINE(0xA837);


/*
 *  GetFontInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetFontInfo(FontInfo * info)                                  ONEWORDINLINE(0xA88B);


/*
 *  CharExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CharExtra(Fixed extra)                                        ONEWORDINLINE(0xAA23);


/*
 *  StdText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdText(
  short         count,
  const void *  textAddr,
  Point         numer,
  Point         denom)                                        ONEWORDINLINE(0xA882);


/*
 *  StdTxMeas()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
StdTxMeas(
  short         byteCount,
  const void *  textAddr,
  Point *       numer,
  Point *       denom,
  FontInfo *    info)                                         ONEWORDINLINE(0xA8ED);


/*
 *  StyledLineBreak()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StyledLineBreakCode )
StyledLineBreak(
  Ptr      textPtr,
  long     textLen,
  long     textStart,
  long     textEnd,
  long     flags,
  Fixed *  textWidth,
  long *   textOffset)                                        FOURWORDINLINE(0x2F3C, 0x821C, 0xFFFE, 0xA8B5);


/*
 *  TruncString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
TruncString(
  short       width,
  Str255      theString,
  TruncCode   truncWhere)                                     FOURWORDINLINE(0x2F3C, 0x8208, 0xFFE0, 0xA8B5);


/*
 *  TruncText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
TruncText(
  short       width,
  Ptr         textPtr,
  short *     length,
  TruncCode   truncWhere)                                     FOURWORDINLINE(0x2F3C, 0x820C, 0xFFDE, 0xA8B5);



#if CALL_NOT_IN_CARBON
/*
 *  drawstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
drawstring(const char * s);


/*
 *  stringwidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
stringwidth(const char * s);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  stdtext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
stdtext(
  short          count,
  const void *   textAddr,
  const Point *  numer,
  const Point *  denom);


/*
 *  SwapQDTextFlags()
 *  
 *  Discussion:
 *    Obsolete. Use QDSwapTextFlags instead (in Quickdraw.h).
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( UInt32 )
SwapQDTextFlags(UInt32 newFlags);



#if OLDROUTINENAMES
#define NPixel2Char(textBuf, textLen, slop, pixelWidth, leadingEdge, widthRemaining, styleRunPosition, numer, denom) \
    PixelToChar(textBuf, textLen, slop, pixelWidth, leadingEdge, widthRemaining, styleRunPosition, numer, denom) 
#define NChar2Pixel(textBuf, textLen, slop, offset, direction, styleRunPosition, numer, denom) \
    CharToPixel(textBuf, textLen, slop, offset, direction, styleRunPosition, numer, denom)
#define NDrawJust(textPtr, textLength, slop, styleRunPosition, numer, denom) \
    DrawJustified(textPtr, textLength, slop, styleRunPosition, numer, denom)
#define NMeasureJust(textPtr, textLength, slop, charLocs, styleRunPosition, numer, denom) \
    MeasureJustified(textPtr, textLength, slop, charLocs, styleRunPosition, numer, denom)
#define NPortionText(textPtr, textLen, styleRunPosition, numer, denom)\
    PortionLine(textPtr, textLen, styleRunPosition, numer, denom)
#endif  /* OLDROUTINENAMES */


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

#endif /* __QUICKDRAWTEXT__ */

