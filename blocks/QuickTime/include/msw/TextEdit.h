/*
     File:       TextEdit.h
 
     Contains:   TextEdit Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __TEXTEDIT__
#define __TEXTEDIT__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
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

typedef struct TERec                    TERec;
typedef TERec *                         TEPtr;
typedef TEPtr *                         TEHandle;
typedef CALLBACK_API_REGISTER68K( void , HighHookProcPtr, (const Rect *r, TEPtr pTE) );
typedef CALLBACK_API_REGISTER68K( Boolean , EOLHookProcPtr, (char theChar, TEPtr pTE, TEHandle hTE) );
typedef CALLBACK_API_REGISTER68K( void , CaretHookProcPtr, (const Rect *r, TEPtr pTE) );
typedef CALLBACK_API_REGISTER68K( unsigned short , WidthHookProcPtr, (unsigned short textLen, unsigned short textOffset, void *textBufferPtr, TEPtr pTE, TEHandle hTE) );
typedef CALLBACK_API_REGISTER68K( unsigned short , TextWidthHookProcPtr, (unsigned short textLen, unsigned short textOffset, void *textBufferPtr, TEPtr pTE, TEHandle hTE) );
typedef CALLBACK_API_REGISTER68K( unsigned short , NWidthHookProcPtr, (unsigned short styleRunLen, unsigned short styleRunOffset, short slop, short direction, void *textBufferPtr, short *lineStart, TEPtr pTE, TEHandle hTE) );
typedef CALLBACK_API_REGISTER68K( void , DrawHookProcPtr, (unsigned short textOffset, unsigned short drawLen, void *textBufferPtr, TEPtr pTE, TEHandle hTE) );
typedef CALLBACK_API_REGISTER68K( Boolean , HitTestHookProcPtr, (unsigned short styleRunLen, unsigned short styleRunOffset, unsigned short slop, void *textBufferPtr, TEPtr pTE, TEHandle hTE, unsigned short *pixelWidth, unsigned short *charOffset, Boolean *pixelInChar) );
typedef CALLBACK_API_REGISTER68K( void , TEFindWordProcPtr, (unsigned short currentPos, short caller, TEPtr pTE, TEHandle hTE, unsigned short *wordStart, unsigned short *wordEnd) );
typedef CALLBACK_API_REGISTER68K( void , TERecalcProcPtr, (TEPtr pTE, unsigned short changeLength, unsigned short *lineStart, unsigned short *firstChar, unsigned short *lastChar) );
typedef CALLBACK_API_REGISTER68K( void , TEDoTextProcPtr, (TEPtr pTE, unsigned short firstChar, unsigned short lastChar, short selector, GrafPtr *currentGrafPort, short *charPosition) );
typedef CALLBACK_API_REGISTER68K( Boolean , TEClickLoopProcPtr, (TEPtr pTE) );
typedef CALLBACK_API_REGISTER68K( Boolean , WordBreakProcPtr, (Ptr text, short charPos) );
/* 
    Important note about TEClickLoopProcPtr and WordBreakProcPtr

    At one point these were defined as returning the function result in the 
    condition code Z-bit.  This was correct, in that it was what the 68K
    implementation of TextEdit actually tested.  But, MixedMode had a different 
    idea of what returning a boolean in the Z-bit meant.  MixedMode was setting
    the Z-bit the complement of what was wanted.  
    
    Therefore, these ProcPtrs have been changed (back) to return the result in
    register D0.  It turns out that for register based routines, 
    MixedMode sets the Z-bit of the 68K emulator based on the contents 
    of the return result register.  Thus we can get the Z-bit set correctly.  
    
    But, when TextEdit is recoded in PowerPC, if it calls a 68K ClickLoop
    or WordBreak routine, register D0 had better have the result (in addition
    to the Z-bit). Therefore all 68K apps should make sure their ClickLoop or
    WordBreak routines set register D0 at the end.
*/

/* 
    There is no function to get/set the low-mem for FindWordHook at 0x07F8.
    This is because it is not a low-mem ProcPtr. That address is the entry
    in the OS TrapTable for trap 0xA0FE.  You can use Get/SetTrapAddress to 
    acccess it. 
*/

/*
    The following ProcPtrs cannot be written in or called from a high-level 
    language without the help of mixed mode or assembly glue because they 
    use the following parameter-passing conventions:

    typedef pascal void (*HighHookProcPtr)(const Rect *r, TEPtr pTE);
    typedef pascal void (*CaretHookProcPtr)(const Rect *r, TEPtr pTE);

        In:
            =>  r                       on stack
            =>  pTE                     A3.L
        Out:
            none

    typedef pascal Boolean (*EOLHookProcPtr)(char theChar, TEPtr pTE, TEHandle hTE);

        In:
            =>  theChar                 D0.B
            =>  pTE                     A3.L
            =>  hTE                     A4.L
        Out:
            <=  Boolean                 Z bit of the CCR

    typedef pascal unsigned short (*WidthHookProcPtr)(unsigned short textLen,
     unsigned short textOffset, void *textBufferPtr, TEPtr pTE, TEHandle hTE);
    typedef pascal unsigned short (*TextWidthHookProcPtr)(unsigned short textLen,
     unsigned short textOffset, void *textBufferPtr, TEPtr pTE, TEHandle hTE);

        In:
            =>  textLen                 D0.W
            =>  textOffset              D1.W
            =>  textBufferPtr           A0.L
            =>  pTE                     A3.L
            =>  hTE                     A4.L
        Out:
            <=  unsigned short          D1.W

    typedef pascal unsigned short (*NWidthHookProcPtr)(unsigned short styleRunLen,
     unsigned short styleRunOffset, short slop, short direction, void *textBufferPtr, 
     short *lineStart, TEPtr pTE, TEHandle hTE);

        In:
            =>  styleRunLen             D0.W
            =>  styleRunOffset          D1.W
            =>  slop                    D2.W (low)
            =>  direction               D2.W (high)
            =>  textBufferPtr           A0.L
            =>  lineStart               A2.L
            =>  pTE                     A3.L
            =>  hTE                     A4.L
        Out:
            <=  unsigned short          D1.W

    typedef pascal void (*DrawHookProcPtr)(unsigned short textOffset, unsigned short drawLen,
     void *textBufferPtr, TEPtr pTE, TEHandle hTE);

        In:
            =>  textOffset              D0.W
            =>  drawLen                 D1.W
            =>  textBufferPtr           A0.L
            =>  pTE                     A3.L
            =>  hTE                     A4.L
        Out:
            none

    typedef pascal Boolean (*HitTestHookProcPtr)(unsigned short styleRunLen,
     unsigned short styleRunOffset, unsigned short slop, void *textBufferPtr,
     TEPtr pTE, TEHandle hTE, unsigned short *pixelWidth, unsigned short *charOffset, 
     Boolean *pixelInChar);

        In:
            =>  styleRunLen             D0.W
            =>  styleRunOffset          D1.W
            =>  slop                    D2.W
            =>  textBufferPtr           A0.L
            =>  pTE                     A3.L
            =>  hTE                     A4.L
        Out:
            <=  pixelWidth              D0.W (low)
            <=  Boolean                 D0.W (high)
            <=  charOffset              D1.W
            <=  pixelInChar             D2.W

    typedef pascal void (*TEFindWordProcPtr)(unsigned short currentPos, short caller, 
     TEPtr pTE, TEHandle hTE, unsigned short *wordStart, unsigned short *wordEnd);

        In:
            =>  currentPos              D0.W
            =>  caller                  D2.W
            =>  pTE                     A3.L
            =>  hTE                     A4.L
        Out:
            <=  wordStart               D0.W
            <=  wordEnd                 D1.W

    typedef pascal void (*TERecalcProcPtr)(TEPtr pTE, unsigned short changeLength,
     unsigned short *lineStart, unsigned short *firstChar, unsigned short *lastChar);

        In:
            =>  pTE                     A3.L
            =>  changeLength            D7.W
        Out:
            <=  lineStart               D2.W
            <=  firstChar               D3.W
            <=  lastChar                D4.W

    typedef pascal void (*TEDoTextProcPtr)(TEPtr pTE, unsigned short firstChar, unsigned short lastChar,
                        short selector, GrafPtr *currentGrafPort, short *charPosition);

        In:
            =>  pTE                     A3.L
            =>  firstChar               D3.W
            =>  lastChar                D4.W
            =>  selector                D7.W
        Out:
            <=  currentGrafPort         A0.L
            <=  charPosition            D0.W
            
*/
typedef REGISTER_UPP_TYPE(HighHookProcPtr)                      HighHookUPP;
typedef REGISTER_UPP_TYPE(EOLHookProcPtr)                       EOLHookUPP;
typedef REGISTER_UPP_TYPE(CaretHookProcPtr)                     CaretHookUPP;
typedef REGISTER_UPP_TYPE(WidthHookProcPtr)                     WidthHookUPP;
typedef REGISTER_UPP_TYPE(TextWidthHookProcPtr)                 TextWidthHookUPP;
typedef REGISTER_UPP_TYPE(NWidthHookProcPtr)                    NWidthHookUPP;
typedef REGISTER_UPP_TYPE(DrawHookProcPtr)                      DrawHookUPP;
typedef REGISTER_UPP_TYPE(HitTestHookProcPtr)                   HitTestHookUPP;
typedef REGISTER_UPP_TYPE(TEFindWordProcPtr)                    TEFindWordUPP;
typedef REGISTER_UPP_TYPE(TERecalcProcPtr)                      TERecalcUPP;
typedef REGISTER_UPP_TYPE(TEDoTextProcPtr)                      TEDoTextUPP;
typedef REGISTER_UPP_TYPE(TEClickLoopProcPtr)                   TEClickLoopUPP;
typedef REGISTER_UPP_TYPE(WordBreakProcPtr)                     WordBreakUPP;
struct TERec {
  Rect                destRect;
  Rect                viewRect;
  Rect                selRect;
  short               lineHeight;
  short               fontAscent;
  Point               selPoint;
  short               selStart;
  short               selEnd;
  short               active;
  WordBreakUPP        wordBreak;              /* NOTE: This field is ignored on non-Roman systems and on Carbon (see IM-Text 2-60) */
  TEClickLoopUPP      clickLoop;
  long                clickTime;
  short               clickLoc;
  long                caretTime;
  short               caretState;
  short               just;
  short               teLength;
  Handle              hText;
  long                hDispatchRec;           /* added to replace recalBack & recalLines.  it's a handle anyway */
  short               clikStuff;
  short               crOnly;
  short               txFont;
  StyleField          txFace;                 /*StyleField occupies 16-bits, but only first 8-bits are used*/
  short               txMode;
  short               txSize;
  GrafPtr             inPort;
  HighHookUPP         highHook;
  CaretHookUPP        caretHook;
  short               nLines;
  short               lineStarts[16001];
};

enum {
                                        /* Justification (word alignment) styles */
  teJustLeft                    = 0,
  teJustCenter                  = 1,
  teJustRight                   = -1,
  teForceLeft                   = -2,   /* new names for the Justification (word alignment) styles */
  teFlushDefault                = 0,    /*flush according to the line direction */
  teCenter                      = 1,    /*center justify (word alignment) */
  teFlushRight                  = -1,   /*flush right for all scripts */
  teFlushLeft                   = -2    /*flush left for all scripts */
};

enum {
                                        /* Set/Replace style modes */
  fontBit                       = 0,    /*set font*/
  faceBit                       = 1,    /*set face*/
  sizeBit                       = 2,    /*set size*/
  clrBit                        = 3,    /*set color*/
  addSizeBit                    = 4,    /*add size mode*/
  toggleBit                     = 5     /*set faces in toggle mode*/
};

enum {
                                        /* TESetStyle/TEContinuousStyle modes */
  doFont                        = 1,    /* set font (family) number*/
  doFace                        = 2,    /*set character style*/
  doSize                        = 4,    /*set type size*/
  doColor                       = 8,    /*set color*/
  doAll                         = 15,   /*set all attributes*/
  addSize                       = 16,   /*adjust type size*/
  doToggle                      = 32    /*toggle mode for TESetStyle*/
};

enum {
                                        /* offsets into TEDispatchRec */
  EOLHook                       = 0,    /*[UniversalProcPtr] TEEOLHook*/
  DRAWHook                      = 4,    /*[UniversalProcPtr] TEWidthHook*/
  WIDTHHook                     = 8,    /*[UniversalProcPtr] TEDrawHook*/
  HITTESTHook                   = 12,   /*[UniversalProcPtr] TEHitTestHook*/
  nWIDTHHook                    = 24,   /*[UniversalProcPtr] nTEWidthHook*/
  TextWidthHook                 = 28    /*[UniversalProcPtr] TETextWidthHook*/
};

enum {
                                        /* selectors for TECustomHook */
  intEOLHook                    = 0,    /*TEIntHook value*/
  intDrawHook                   = 1,    /*TEIntHook value*/
  intWidthHook                  = 2,    /*TEIntHook value*/
  intHitTestHook                = 3,    /*TEIntHook value*/
  intNWidthHook                 = 6,    /*TEIntHook value for new version of WidthHook*/
  intTextWidthHook              = 7,    /*TEIntHook value for new TextWidthHook*/
  intInlineInputTSMTEPreUpdateHook = 8, /*TEIntHook value for TSMTEPreUpdateProcPtr callback*/
  intInlineInputTSMTEPostUpdateHook = 9 /*TEIntHook value for TSMTEPostUpdateProcPtr callback*/
};

enum {
                                        /* feature or bit definitions for TEFeatureFlag */
  teFAutoScroll                 = 0,    /*00000001b*/
  teFTextBuffering              = 1,    /*00000010b*/
  teFOutlineHilite              = 2,    /*00000100b*/
  teFInlineInput                = 3,    /*00001000b */
  teFUseWhiteBackground         = 4,    /*00010000b */
  teFUseInlineInput             = 5,    /*00100000b */
  teFInlineInputAutoScroll      = 6     /*01000000b */
};

enum {
                                        /* feature or bit definitions for TEFeatureFlag -- Carbon only                */
                                        /* To avoid having to call TEIdle in Carbon apps, automatic idling can be activated   */
                                        /* via the following feature flag, but you must ensure that the destRect and/or     */
                                        /* GrafPort's origin be setup properly for drawing in a given TERec when       */
                                        /* the timer fires.    When this feature flag is set, TEIdle is a noop.          */
                                        /* Activate this feature flag before calling TEActivate.                 */
  teFIdleWithEventLoopTimer     = 7     /*10000000b */
};

enum {
                                        /* action for the new "bit (un)set" interface, TEFeatureFlag */
  teBitClear                    = 0,
  teBitSet                      = 1,    /*set the selector bit*/
  teBitTest                     = -1    /*no change; just return the current setting*/
};

enum {
                                        /*constants for identifying the routine that called FindWord */
  teWordSelect                  = 4,    /*clickExpand to select word*/
  teWordDrag                    = 8,    /*clickExpand to drag new word*/
  teFromFind                    = 12,   /*FindLine called it ($0C)*/
  teFromRecal                   = 16    /*RecalLines called it ($10)      obsolete */
};

enum {
                                        /*constants for identifying TEDoText selectors */
  teFind                        = 0,    /*TEDoText called for searching*/
  teHighlight                   = 1,    /*TEDoText called for highlighting*/
  teDraw                        = -1,   /*TEDoText called for drawing text*/
  teCaret                       = -2    /*TEDoText called for drawing the caret*/
};


typedef char                            Chars[32001];
typedef char *                          CharsPtr;
typedef CharsPtr *                      CharsHandle;
struct StyleRun {
  short               startChar;              /*starting character position*/
  short               styleIndex;             /*index in style table*/
};
typedef struct StyleRun                 StyleRun;
struct STElement {
  short               stCount;                /*number of runs in this style*/
  short               stHeight;               /*line height*/
  short               stAscent;               /*font ascent*/
  short               stFont;                 /*font (family) number*/
  StyleField          stFace;                 /*StyleField occupies 16-bits, but only first 8-bits are used */
  short               stSize;                 /*size in points*/
  RGBColor            stColor;                /*absolute (RGB) color*/
};
typedef struct STElement                STElement;
typedef STElement                       TEStyleTable[1777];
typedef STElement *                     STPtr;
typedef STPtr *                         STHandle;
struct LHElement {
  short               lhHeight;               /*maximum height in line*/
  short               lhAscent;               /*maximum ascent in line*/
};
typedef struct LHElement                LHElement;
typedef LHElement                       LHTable[8001];
typedef LHElement *                     LHPtr;
typedef LHPtr *                         LHHandle;
struct ScrpSTElement {
  long                scrpStartChar;          /*starting character position*/
  short               scrpHeight;
  short               scrpAscent;
  short               scrpFont;
  StyleField          scrpFace;               /*StyleField occupies 16-bits, but only first 8-bits are used*/
  short               scrpSize;
  RGBColor            scrpColor;
};
typedef struct ScrpSTElement            ScrpSTElement;
/* ARRAY [0..1600] OF ScrpSTElement */
typedef ScrpSTElement                   ScrpSTTable[1601];
struct StScrpRec {
  short               scrpNStyles;            /*number of styles in scrap*/
  ScrpSTTable         scrpStyleTab;           /*table of styles for scrap*/
};
typedef struct StScrpRec                StScrpRec;
typedef StScrpRec *                     StScrpPtr;
typedef StScrpPtr *                     StScrpHandle;
struct NullStRec {
  long                teReserved;             /*reserved for future expansion*/
  StScrpHandle        nullScrap;              /*handle to scrap style table*/
};
typedef struct NullStRec                NullStRec;
typedef NullStRec *                     NullStPtr;
typedef NullStPtr *                     NullStHandle;
struct TEStyleRec {
  short               nRuns;                  /*number of style runs*/
  short               nStyles;                /*size of style table*/
  STHandle            styleTab;               /*handle to style table*/
  LHHandle            lhTab;                  /*handle to line-height table*/
  long                teRefCon;               /*reserved for application use*/
  NullStHandle        nullStyle;              /*Handle to style set at null selection*/
  StyleRun            runs[8001];             /*ARRAY [0..8000] OF StyleRun*/
};
typedef struct TEStyleRec               TEStyleRec;
typedef TEStyleRec *                    TEStylePtr;
typedef TEStylePtr *                    TEStyleHandle;
struct TextStyle {
  short               tsFont;                 /*font (family) number*/
  StyleField          tsFace;                 /*StyleField occupies 16-bits, but only first 8-bits are used*/
  short               tsSize;                 /*size in point*/
  RGBColor            tsColor;                /*absolute (RGB) color*/
};
typedef struct TextStyle                TextStyle;
typedef TextStyle *                     TextStylePtr;
typedef TextStylePtr *                  TextStyleHandle;
typedef short                           TEIntHook;
/*
 *  NewHighHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HighHookUPP )
NewHighHookUPP(HighHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHighHookProcInfo = 0x0000000F };  /* SPECIAL_CASE_PROCINFO(0) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HighHookUPP) NewHighHookUPP(HighHookProcPtr userRoutine) { return (HighHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHighHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHighHookUPP(userRoutine) (HighHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHighHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewEOLHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( EOLHookUPP )
NewEOLHookUPP(EOLHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppEOLHookProcInfo = 0x0000001F };  /* SPECIAL_CASE_PROCINFO(1) */
  #ifdef __cplusplus
    inline DEFINE_API_C(EOLHookUPP) NewEOLHookUPP(EOLHookProcPtr userRoutine) { return (EOLHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppEOLHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewEOLHookUPP(userRoutine) (EOLHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppEOLHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCaretHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CaretHookUPP )
NewCaretHookUPP(CaretHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCaretHookProcInfo = 0x0000000F };  /* SPECIAL_CASE_PROCINFO(0) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CaretHookUPP) NewCaretHookUPP(CaretHookProcPtr userRoutine) { return (CaretHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCaretHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCaretHookUPP(userRoutine) (CaretHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCaretHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( WidthHookUPP )
NewWidthHookUPP(WidthHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppWidthHookProcInfo = 0x0000002F };  /* SPECIAL_CASE_PROCINFO(2) */
  #ifdef __cplusplus
    inline DEFINE_API_C(WidthHookUPP) NewWidthHookUPP(WidthHookProcPtr userRoutine) { return (WidthHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppWidthHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewWidthHookUPP(userRoutine) (WidthHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppWidthHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTextWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TextWidthHookUPP )
NewTextWidthHookUPP(TextWidthHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTextWidthHookProcInfo = 0x0000002F };  /* SPECIAL_CASE_PROCINFO(2) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TextWidthHookUPP) NewTextWidthHookUPP(TextWidthHookProcPtr userRoutine) { return (TextWidthHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTextWidthHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTextWidthHookUPP(userRoutine) (TextWidthHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTextWidthHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewNWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NWidthHookUPP )
NewNWidthHookUPP(NWidthHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNWidthHookProcInfo = 0x0000003F };  /* SPECIAL_CASE_PROCINFO(3) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NWidthHookUPP) NewNWidthHookUPP(NWidthHookProcPtr userRoutine) { return (NWidthHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNWidthHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNWidthHookUPP(userRoutine) (NWidthHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNWidthHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDrawHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DrawHookUPP )
NewDrawHookUPP(DrawHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDrawHookProcInfo = 0x0000004F };  /* SPECIAL_CASE_PROCINFO(4) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DrawHookUPP) NewDrawHookUPP(DrawHookProcPtr userRoutine) { return (DrawHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDrawHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDrawHookUPP(userRoutine) (DrawHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDrawHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHitTestHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HitTestHookUPP )
NewHitTestHookUPP(HitTestHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHitTestHookProcInfo = 0x0000005F };  /* SPECIAL_CASE_PROCINFO(5) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HitTestHookUPP) NewHitTestHookUPP(HitTestHookProcPtr userRoutine) { return (HitTestHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHitTestHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHitTestHookUPP(userRoutine) (HitTestHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHitTestHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTEFindWordUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TEFindWordUPP )
NewTEFindWordUPP(TEFindWordProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTEFindWordProcInfo = 0x0000006F };  /* SPECIAL_CASE_PROCINFO(6) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TEFindWordUPP) NewTEFindWordUPP(TEFindWordProcPtr userRoutine) { return (TEFindWordUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTEFindWordProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTEFindWordUPP(userRoutine) (TEFindWordUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTEFindWordProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTERecalcUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TERecalcUPP )
NewTERecalcUPP(TERecalcProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTERecalcProcInfo = 0x0000009F };  /* SPECIAL_CASE_PROCINFO(9) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TERecalcUPP) NewTERecalcUPP(TERecalcProcPtr userRoutine) { return (TERecalcUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTERecalcProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTERecalcUPP(userRoutine) (TERecalcUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTERecalcProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTEDoTextUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TEDoTextUPP )
NewTEDoTextUPP(TEDoTextProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTEDoTextProcInfo = 0x000000AF };  /* SPECIAL_CASE_PROCINFO(10) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TEDoTextUPP) NewTEDoTextUPP(TEDoTextProcPtr userRoutine) { return (TEDoTextUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTEDoTextProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTEDoTextUPP(userRoutine) (TEDoTextUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTEDoTextProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTEClickLoopUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TEClickLoopUPP )
NewTEClickLoopUPP(TEClickLoopProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTEClickLoopProcInfo = 0x0000F812 };  /* register 1_byte:D0 Func(4_bytes:A3) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TEClickLoopUPP) NewTEClickLoopUPP(TEClickLoopProcPtr userRoutine) { return (TEClickLoopUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTEClickLoopProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTEClickLoopUPP(userRoutine) (TEClickLoopUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTEClickLoopProcInfo, GetCurrentArchitecture())
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  NewWordBreakUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( WordBreakUPP )
NewWordBreakUPP(WordBreakProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppWordBreakProcInfo = 0x00029812 };  /* register 1_byte:D0 Func(4_bytes:A0, 2_bytes:D0) */
  #ifdef __cplusplus
    inline DEFINE_API_C(WordBreakUPP) NewWordBreakUPP(WordBreakProcPtr userRoutine) { return (WordBreakUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppWordBreakProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewWordBreakUPP(userRoutine) (WordBreakUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppWordBreakProcInfo, GetCurrentArchitecture())
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DisposeHighHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHighHookUPP(HighHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHighHookUPP(HighHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHighHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeEOLHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeEOLHookUPP(EOLHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeEOLHookUPP(EOLHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeEOLHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCaretHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCaretHookUPP(CaretHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCaretHookUPP(CaretHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCaretHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeWidthHookUPP(WidthHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeWidthHookUPP(WidthHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeWidthHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTextWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTextWidthHookUPP(TextWidthHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTextWidthHookUPP(TextWidthHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTextWidthHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeNWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeNWidthHookUPP(NWidthHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNWidthHookUPP(NWidthHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNWidthHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDrawHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDrawHookUPP(DrawHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDrawHookUPP(DrawHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDrawHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHitTestHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHitTestHookUPP(HitTestHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHitTestHookUPP(HitTestHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHitTestHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTEFindWordUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTEFindWordUPP(TEFindWordUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTEFindWordUPP(TEFindWordUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTEFindWordUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTERecalcUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTERecalcUPP(TERecalcUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTERecalcUPP(TERecalcUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTERecalcUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTEDoTextUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTEDoTextUPP(TEDoTextUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTEDoTextUPP(TEDoTextUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTEDoTextUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTEClickLoopUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTEClickLoopUPP(TEClickLoopUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTEClickLoopUPP(TEClickLoopUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTEClickLoopUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  DisposeWordBreakUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeWordBreakUPP(WordBreakUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeWordBreakUPP(WordBreakUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeWordBreakUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  InvokeHighHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeHighHookUPP(
  const Rect *  r,
  TEPtr         pTE,
  HighHookUPP   userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeHighHookUPP(const Rect * r, TEPtr pTE, HighHookUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppHighHookProcInfo, r, pTE); }
  #else
    #define InvokeHighHookUPP(r, pTE, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppHighHookProcInfo, (r), (pTE))
  #endif
#endif

/*
 *  InvokeEOLHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeEOLHookUPP(
  char        theChar,
  TEPtr       pTE,
  TEHandle    hTE,
  EOLHookUPP  userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeEOLHookUPP(char theChar, TEPtr pTE, TEHandle hTE, EOLHookUPP userUPP) { return (Boolean)CALL_THREE_PARAMETER_UPP(userUPP, uppEOLHookProcInfo, theChar, pTE, hTE); }
  #else
    #define InvokeEOLHookUPP(theChar, pTE, hTE, userUPP) (Boolean)CALL_THREE_PARAMETER_UPP((userUPP), uppEOLHookProcInfo, (theChar), (pTE), (hTE))
  #endif
#endif

/*
 *  InvokeCaretHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeCaretHookUPP(
  const Rect *  r,
  TEPtr         pTE,
  CaretHookUPP  userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeCaretHookUPP(const Rect * r, TEPtr pTE, CaretHookUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppCaretHookProcInfo, r, pTE); }
  #else
    #define InvokeCaretHookUPP(r, pTE, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppCaretHookProcInfo, (r), (pTE))
  #endif
#endif

/*
 *  InvokeWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( unsigned short )
InvokeWidthHookUPP(
  unsigned short  textLen,
  unsigned short  textOffset,
  void *          textBufferPtr,
  TEPtr           pTE,
  TEHandle        hTE,
  WidthHookUPP    userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(unsigned short) InvokeWidthHookUPP(unsigned short textLen, unsigned short textOffset, void * textBufferPtr, TEPtr pTE, TEHandle hTE, WidthHookUPP userUPP) { return (unsigned short)CALL_FIVE_PARAMETER_UPP(userUPP, uppWidthHookProcInfo, textLen, textOffset, textBufferPtr, pTE, hTE); }
  #else
    #define InvokeWidthHookUPP(textLen, textOffset, textBufferPtr, pTE, hTE, userUPP) (unsigned short)CALL_FIVE_PARAMETER_UPP((userUPP), uppWidthHookProcInfo, (textLen), (textOffset), (textBufferPtr), (pTE), (hTE))
  #endif
#endif

/*
 *  InvokeTextWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( unsigned short )
InvokeTextWidthHookUPP(
  unsigned short    textLen,
  unsigned short    textOffset,
  void *            textBufferPtr,
  TEPtr             pTE,
  TEHandle          hTE,
  TextWidthHookUPP  userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(unsigned short) InvokeTextWidthHookUPP(unsigned short textLen, unsigned short textOffset, void * textBufferPtr, TEPtr pTE, TEHandle hTE, TextWidthHookUPP userUPP) { return (unsigned short)CALL_FIVE_PARAMETER_UPP(userUPP, uppTextWidthHookProcInfo, textLen, textOffset, textBufferPtr, pTE, hTE); }
  #else
    #define InvokeTextWidthHookUPP(textLen, textOffset, textBufferPtr, pTE, hTE, userUPP) (unsigned short)CALL_FIVE_PARAMETER_UPP((userUPP), uppTextWidthHookProcInfo, (textLen), (textOffset), (textBufferPtr), (pTE), (hTE))
  #endif
#endif

/*
 *  InvokeNWidthHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( unsigned short )
InvokeNWidthHookUPP(
  unsigned short  styleRunLen,
  unsigned short  styleRunOffset,
  short           slop,
  short           direction,
  void *          textBufferPtr,
  short *         lineStart,
  TEPtr           pTE,
  TEHandle        hTE,
  NWidthHookUPP   userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(unsigned short) InvokeNWidthHookUPP(unsigned short styleRunLen, unsigned short styleRunOffset, short slop, short direction, void * textBufferPtr, short * lineStart, TEPtr pTE, TEHandle hTE, NWidthHookUPP userUPP) { return (unsigned short)CALL_EIGHT_PARAMETER_UPP(userUPP, uppNWidthHookProcInfo, styleRunLen, styleRunOffset, slop, direction, textBufferPtr, lineStart, pTE, hTE); }
  #else
    #define InvokeNWidthHookUPP(styleRunLen, styleRunOffset, slop, direction, textBufferPtr, lineStart, pTE, hTE, userUPP) (unsigned short)CALL_EIGHT_PARAMETER_UPP((userUPP), uppNWidthHookProcInfo, (styleRunLen), (styleRunOffset), (slop), (direction), (textBufferPtr), (lineStart), (pTE), (hTE))
  #endif
#endif

/*
 *  InvokeDrawHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDrawHookUPP(
  unsigned short  textOffset,
  unsigned short  drawLen,
  void *          textBufferPtr,
  TEPtr           pTE,
  TEHandle        hTE,
  DrawHookUPP     userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDrawHookUPP(unsigned short textOffset, unsigned short drawLen, void * textBufferPtr, TEPtr pTE, TEHandle hTE, DrawHookUPP userUPP) { CALL_FIVE_PARAMETER_UPP(userUPP, uppDrawHookProcInfo, textOffset, drawLen, textBufferPtr, pTE, hTE); }
  #else
    #define InvokeDrawHookUPP(textOffset, drawLen, textBufferPtr, pTE, hTE, userUPP) CALL_FIVE_PARAMETER_UPP((userUPP), uppDrawHookProcInfo, (textOffset), (drawLen), (textBufferPtr), (pTE), (hTE))
  #endif
#endif

/*
 *  InvokeHitTestHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeHitTestHookUPP(
  unsigned short    styleRunLen,
  unsigned short    styleRunOffset,
  unsigned short    slop,
  void *            textBufferPtr,
  TEPtr             pTE,
  TEHandle          hTE,
  unsigned short *  pixelWidth,
  unsigned short *  charOffset,
  Boolean *         pixelInChar,
  HitTestHookUPP    userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeHitTestHookUPP(unsigned short styleRunLen, unsigned short styleRunOffset, unsigned short slop, void * textBufferPtr, TEPtr pTE, TEHandle hTE, unsigned short * pixelWidth, unsigned short * charOffset, Boolean * pixelInChar, HitTestHookUPP userUPP) { return (Boolean)CALL_NINE_PARAMETER_UPP(userUPP, uppHitTestHookProcInfo, styleRunLen, styleRunOffset, slop, textBufferPtr, pTE, hTE, pixelWidth, charOffset, pixelInChar); }
  #else
    #define InvokeHitTestHookUPP(styleRunLen, styleRunOffset, slop, textBufferPtr, pTE, hTE, pixelWidth, charOffset, pixelInChar, userUPP) (Boolean)CALL_NINE_PARAMETER_UPP((userUPP), uppHitTestHookProcInfo, (styleRunLen), (styleRunOffset), (slop), (textBufferPtr), (pTE), (hTE), (pixelWidth), (charOffset), (pixelInChar))
  #endif
#endif

/*
 *  InvokeTEFindWordUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeTEFindWordUPP(
  unsigned short    currentPos,
  short             caller,
  TEPtr             pTE,
  TEHandle          hTE,
  unsigned short *  wordStart,
  unsigned short *  wordEnd,
  TEFindWordUPP     userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeTEFindWordUPP(unsigned short currentPos, short caller, TEPtr pTE, TEHandle hTE, unsigned short * wordStart, unsigned short * wordEnd, TEFindWordUPP userUPP) { CALL_SIX_PARAMETER_UPP(userUPP, uppTEFindWordProcInfo, currentPos, caller, pTE, hTE, wordStart, wordEnd); }
  #else
    #define InvokeTEFindWordUPP(currentPos, caller, pTE, hTE, wordStart, wordEnd, userUPP) CALL_SIX_PARAMETER_UPP((userUPP), uppTEFindWordProcInfo, (currentPos), (caller), (pTE), (hTE), (wordStart), (wordEnd))
  #endif
#endif

/*
 *  InvokeTERecalcUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeTERecalcUPP(
  TEPtr             pTE,
  unsigned short    changeLength,
  unsigned short *  lineStart,
  unsigned short *  firstChar,
  unsigned short *  lastChar,
  TERecalcUPP       userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeTERecalcUPP(TEPtr pTE, unsigned short changeLength, unsigned short * lineStart, unsigned short * firstChar, unsigned short * lastChar, TERecalcUPP userUPP) { CALL_FIVE_PARAMETER_UPP(userUPP, uppTERecalcProcInfo, pTE, changeLength, lineStart, firstChar, lastChar); }
  #else
    #define InvokeTERecalcUPP(pTE, changeLength, lineStart, firstChar, lastChar, userUPP) CALL_FIVE_PARAMETER_UPP((userUPP), uppTERecalcProcInfo, (pTE), (changeLength), (lineStart), (firstChar), (lastChar))
  #endif
#endif

/*
 *  InvokeTEDoTextUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeTEDoTextUPP(
  TEPtr           pTE,
  unsigned short  firstChar,
  unsigned short  lastChar,
  short           selector,
  GrafPtr *       currentGrafPort,
  short *         charPosition,
  TEDoTextUPP     userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeTEDoTextUPP(TEPtr pTE, unsigned short firstChar, unsigned short lastChar, short selector, GrafPtr * currentGrafPort, short * charPosition, TEDoTextUPP userUPP) { CALL_SIX_PARAMETER_UPP(userUPP, uppTEDoTextProcInfo, pTE, firstChar, lastChar, selector, currentGrafPort, charPosition); }
  #else
    #define InvokeTEDoTextUPP(pTE, firstChar, lastChar, selector, currentGrafPort, charPosition, userUPP) CALL_SIX_PARAMETER_UPP((userUPP), uppTEDoTextProcInfo, (pTE), (firstChar), (lastChar), (selector), (currentGrafPort), (charPosition))
  #endif
#endif

/*
 *  InvokeTEClickLoopUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeTEClickLoopUPP(
  TEPtr           pTE,
  TEClickLoopUPP  userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeTEClickLoopUPP(TEPtr pTE, TEClickLoopUPP userUPP) { return (Boolean)CALL_ONE_PARAMETER_UPP(userUPP, uppTEClickLoopProcInfo, pTE); }
  #else
    #define InvokeTEClickLoopUPP(pTE, userUPP) (Boolean)CALL_ONE_PARAMETER_UPP((userUPP), uppTEClickLoopProcInfo, (pTE))
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  InvokeWordBreakUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 InvokeWordBreakUPP(__A0, __D0, __A1)
#endif
EXTERN_API_C( Boolean )
InvokeWordBreakUPP(
  Ptr           text,
  short         charPos,
  WordBreakUPP  userUPP)                                      ONEWORDINLINE(0x4E91);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeWordBreakUPP(Ptr text, short charPos, WordBreakUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppWordBreakProcInfo, text, charPos); }
  #else
    #define InvokeWordBreakUPP(text, charPos, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppWordBreakProcInfo, (text), (charPos))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewHighHookProc(userRoutine)                        NewHighHookUPP(userRoutine)
    #define NewEOLHookProc(userRoutine)                         NewEOLHookUPP(userRoutine)
    #define NewCaretHookProc(userRoutine)                       NewCaretHookUPP(userRoutine)
    #define NewWidthHookProc(userRoutine)                       NewWidthHookUPP(userRoutine)
    #define NewTextWidthHookProc(userRoutine)                   NewTextWidthHookUPP(userRoutine)
    #define NewNWidthHookProc(userRoutine)                      NewNWidthHookUPP(userRoutine)
    #define NewDrawHookProc(userRoutine)                        NewDrawHookUPP(userRoutine)
    #define NewHitTestHookProc(userRoutine)                     NewHitTestHookUPP(userRoutine)
    #define NewTEFindWordProc(userRoutine)                      NewTEFindWordUPP(userRoutine)
    #define NewTERecalcProc(userRoutine)                        NewTERecalcUPP(userRoutine)
    #define NewTEDoTextProc(userRoutine)                        NewTEDoTextUPP(userRoutine)
    #define NewTEClickLoopProc(userRoutine)                     NewTEClickLoopUPP(userRoutine)
    #define NewWordBreakProc(userRoutine)                       NewWordBreakUPP(userRoutine)
    #define CallHighHookProc(userRoutine, r, pTE)               InvokeHighHookUPP(r, pTE, userRoutine)
    #define CallEOLHookProc(userRoutine, theChar, pTE, hTE)     InvokeEOLHookUPP(theChar, pTE, hTE, userRoutine)
    #define CallCaretHookProc(userRoutine, r, pTE)              InvokeCaretHookUPP(r, pTE, userRoutine)
    #define CallWidthHookProc(userRoutine, textLen, textOffset, textBufferPtr, pTE, hTE) InvokeWidthHookUPP(textLen, textOffset, textBufferPtr, pTE, hTE, userRoutine)
    #define CallTextWidthHookProc(userRoutine, textLen, textOffset, textBufferPtr, pTE, hTE) InvokeTextWidthHookUPP(textLen, textOffset, textBufferPtr, pTE, hTE, userRoutine)
    #define CallNWidthHookProc(userRoutine, styleRunLen, styleRunOffset, slop, direction, textBufferPtr, lineStart, pTE, hTE) InvokeNWidthHookUPP(styleRunLen, styleRunOffset, slop, direction, textBufferPtr, lineStart, pTE, hTE, userRoutine)
    #define CallDrawHookProc(userRoutine, textOffset, drawLen, textBufferPtr, pTE, hTE) InvokeDrawHookUPP(textOffset, drawLen, textBufferPtr, pTE, hTE, userRoutine)
    #define CallHitTestHookProc(userRoutine, styleRunLen, styleRunOffset, slop, textBufferPtr, pTE, hTE, pixelWidth, charOffset, pixelInChar) InvokeHitTestHookUPP(styleRunLen, styleRunOffset, slop, textBufferPtr, pTE, hTE, pixelWidth, charOffset, pixelInChar, userRoutine)
    #define CallTEFindWordProc(userRoutine, currentPos, caller, pTE, hTE, wordStart, wordEnd) InvokeTEFindWordUPP(currentPos, caller, pTE, hTE, wordStart, wordEnd, userRoutine)
    #define CallTERecalcProc(userRoutine, pTE, changeLength, lineStart, firstChar, lastChar) InvokeTERecalcUPP(pTE, changeLength, lineStart, firstChar, lastChar, userRoutine)
    #define CallTEDoTextProc(userRoutine, pTE, firstChar, lastChar, selector, currentGrafPort, charPosition) InvokeTEDoTextUPP(pTE, firstChar, lastChar, selector, currentGrafPort, charPosition, userRoutine)
    #define CallTEClickLoopProc(userRoutine, pTE)               InvokeTEClickLoopUPP(pTE, userRoutine)
    #define CallWordBreakProc(userRoutine, text, charPos)       InvokeWordBreakUPP(text, charPos, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

enum {
                                        /* feature bit 4 for TEFeatureFlag no longer in use */
  teFUseTextServices            = 4     /*00010000b */
};


#if OLDROUTINENAMES
/* action for the old C "bit (un)set" interface, TEFeatureFlag */
#define TEBitClear  teBitClear
#define TEBitSet    teBitSet
#define TEBitTest   teBitTest
#define teFAutoScr  teFAutoScroll
#define toglBit     toggleBit
#endif  /* OLDROUTINENAMES */

/*
 *  TEScrapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
TEScrapHandle(void)                                           TWOWORDINLINE(0x2EB8, 0x0AB4);



/*
 *  TEGetScrapLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
TEGetScrapLength(void);


#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#define TEGetScrapLength() ((long) * (unsigned short *) 0x0AB0)
#endif
#if CALL_NOT_IN_CARBON
/*
 *  TEInit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
TEInit(void)                                                  ONEWORDINLINE(0xA9CC);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  TENew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TEHandle )
TENew(
  const Rect *  destRect,
  const Rect *  viewRect)                                     ONEWORDINLINE(0xA9D2);


/*
 *  TEDispose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEDispose(TEHandle hTE)                                       ONEWORDINLINE(0xA9CD);


/*
 *  TESetText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetText(
  const void *  text,
  long          length,
  TEHandle      hTE)                                          ONEWORDINLINE(0xA9CF);


/*
 *  TEGetText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CharsHandle )
TEGetText(TEHandle hTE)                                       ONEWORDINLINE(0xA9CB);


/*
 *  TEIdle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEIdle(TEHandle hTE)                                          ONEWORDINLINE(0xA9DA);


/*
 *  TESetSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetSelect(
  long       selStart,
  long       selEnd,
  TEHandle   hTE)                                             ONEWORDINLINE(0xA9D1);


/*
 *  TEActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEActivate(TEHandle hTE)                                      ONEWORDINLINE(0xA9D8);


/*
 *  TEDeactivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEDeactivate(TEHandle hTE)                                    ONEWORDINLINE(0xA9D9);


/*
 *  TEKey()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEKey(
  CharParameter   key,
  TEHandle        hTE)                                        ONEWORDINLINE(0xA9DC);


/*
 *  TECut()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TECut(TEHandle hTE)                                           ONEWORDINLINE(0xA9D6);


/*
 *  TECopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TECopy(TEHandle hTE)                                          ONEWORDINLINE(0xA9D5);


/*
 *  TEPaste()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEPaste(TEHandle hTE)                                         ONEWORDINLINE(0xA9DB);


/*
 *  TEDelete()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEDelete(TEHandle hTE)                                        ONEWORDINLINE(0xA9D7);


/*
 *  TEInsert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEInsert(
  const void *  text,
  long          length,
  TEHandle      hTE)                                          ONEWORDINLINE(0xA9DE);


/*
 *  TESetAlignment()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetAlignment(
  short      just,
  TEHandle   hTE)                                             ONEWORDINLINE(0xA9DF);


/*
 *  TEUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEUpdate(
  const Rect *  rUpdate,
  TEHandle      hTE)                                          ONEWORDINLINE(0xA9D3);


/*
 *  TETextBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TETextBox(
  const void *  text,
  long          length,
  const Rect *  box,
  short         just)                                         ONEWORDINLINE(0xA9CE);


/*
 *  TEScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEScroll(
  short      dh,
  short      dv,
  TEHandle   hTE)                                             ONEWORDINLINE(0xA9DD);


/*
 *  TESelView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESelView(TEHandle hTE)                                       ONEWORDINLINE(0xA811);


/*
 *  TEPinScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEPinScroll(
  short      dh,
  short      dv,
  TEHandle   hTE)                                             ONEWORDINLINE(0xA812);


/*
 *  TEAutoView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEAutoView(
  Boolean    fAuto,
  TEHandle   hTE)                                             ONEWORDINLINE(0xA813);


/*
 *  TECalText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TECalText(TEHandle hTE)                                       ONEWORDINLINE(0xA9D0);


/*
 *  TEGetOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
TEGetOffset(
  Point      pt,
  TEHandle   hTE)                                             ONEWORDINLINE(0xA83C);


/*
 *  TEGetPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point )
TEGetPoint(
  short      offset,
  TEHandle   hTE)                                             THREEWORDINLINE(0x3F3C, 0x0008, 0xA83D);


/*
 *  TEClick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEClick(
  Point      pt,
  Boolean    fExtend,
  TEHandle   h)                                               ONEWORDINLINE(0xA9D4);


/*
 *  TEStyleNew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TEHandle )
TEStyleNew(
  const Rect *  destRect,
  const Rect *  viewRect)                                     ONEWORDINLINE(0xA83E);


/*
 *  TESetStyleHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetStyleHandle(
  TEStyleHandle   theHandle,
  TEHandle        hTE)                                        THREEWORDINLINE(0x3F3C, 0x0005, 0xA83D);


/*
 *  TEGetStyleHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TEStyleHandle )
TEGetStyleHandle(TEHandle hTE)                                THREEWORDINLINE(0x3F3C, 0x0004, 0xA83D);


/*
 *  TEGetStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEGetStyle(
  short        offset,
  TextStyle *  theStyle,
  short *      lineHeight,
  short *      fontAscent,
  TEHandle     hTE)                                           THREEWORDINLINE(0x3F3C, 0x0003, 0xA83D);


/*
 *  TEStylePaste()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEStylePaste(TEHandle hTE)                                    THREEWORDINLINE(0x3F3C, 0x0000, 0xA83D);


/*
 *  TESetStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetStyle(
  short              mode,
  const TextStyle *  newStyle,
  Boolean            fRedraw,
  TEHandle           hTE)                                     THREEWORDINLINE(0x3F3C, 0x0001, 0xA83D);


/*
 *  TEReplaceStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEReplaceStyle(
  short              mode,
  const TextStyle *  oldStyle,
  const TextStyle *  newStyle,
  Boolean            fRedraw,
  TEHandle           hTE)                                     THREEWORDINLINE(0x3F3C, 0x0002, 0xA83D);


/*
 *  TEGetStyleScrapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StScrpHandle )
TEGetStyleScrapHandle(TEHandle hTE)                           THREEWORDINLINE(0x3F3C, 0x0006, 0xA83D);


/*
 *  TEStyleInsert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEStyleInsert(
  const void *   text,
  long           length,
  StScrpHandle   hST,
  TEHandle       hTE)                                         THREEWORDINLINE(0x3F3C, 0x0007, 0xA83D);


/*
 *  TEGetHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
TEGetHeight(
  long       endLine,
  long       startLine,
  TEHandle   hTE)                                             THREEWORDINLINE(0x3F3C, 0x0009, 0xA83D);


/*
 *  TEContinuousStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
TEContinuousStyle(
  short *      mode,
  TextStyle *  aStyle,
  TEHandle     hTE)                                           THREEWORDINLINE(0x3F3C, 0x000A, 0xA83D);


/*
 *  TEUseStyleScrap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TEUseStyleScrap(
  long           rangeStart,
  long           rangeEnd,
  StScrpHandle   newStyles,
  Boolean        fRedraw,
  TEHandle       hTE)                                         THREEWORDINLINE(0x3F3C, 0x000B, 0xA83D);


/*
 *  TECustomHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TECustomHook(
  TEIntHook           which,
  UniversalProcPtr *  addr,
  TEHandle            hTE)                                    THREEWORDINLINE(0x3F3C, 0x000C, 0xA83D);


/*
 *  TENumStyles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
TENumStyles(
  long       rangeStart,
  long       rangeEnd,
  TEHandle   hTE)                                             THREEWORDINLINE(0x3F3C, 0x000D, 0xA83D);


/*
 *  TEFeatureFlag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
TEFeatureFlag(
  short      feature,
  short      action,
  TEHandle   hTE)                                             THREEWORDINLINE(0x3F3C, 0x000E, 0xA83D);


/*
 *  TEGetHiliteRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
TEGetHiliteRgn(
  RgnHandle   region,
  TEHandle    hTE)                                            THREEWORDINLINE(0x3F3C, 0x000F, 0xA83D);


/*
 *  TESetScrapLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetScrapLength(long length);


/*
 *  TEFromScrap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
TEFromScrap(void);


/*
 *  TEToScrap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
TEToScrap(void);


/*
 *  TESetClickLoop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetClickLoop(
  TEClickLoopUPP   clikProc,
  TEHandle         hTE);


#if CALL_NOT_IN_CARBON
/*
 *  TESetWordBreak()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
TESetWordBreak(
  WordBreakUPP   wBrkProc,
  TEHandle       hTE);









#endif  /* CALL_NOT_IN_CARBON */

/*
 *  TEGetDoTextHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TEDoTextUPP )
TEGetDoTextHook(void);


/*
 *  TESetDoTextHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetDoTextHook(TEDoTextUPP value);


/*
 *  TEGetRecalcHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TERecalcUPP )
TEGetRecalcHook(void);


/*
 *  TESetRecalcHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetRecalcHook(TERecalcUPP value);


/*
 *  TEGetFindWordHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TEFindWordUPP )
TEGetFindWordHook(void);


/*
 *  TESetFindWordHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetFindWordHook(TEFindWordUPP value);


/*
 *  TEGetScrapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
TEGetScrapHandle(void);


/*
 *  TESetScrapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
TESetScrapHandle(Handle value);




/* LMGetWordRedraw and LMSetWordRedraw were previously in LowMem.h  */
/* Deprecated for Carbon on MacOS X                                 */
/* This lomem is no longer used by the implementation of TextEdit   */
/* on MacOS X, so setting it will have no effect.                   */
/*
 *  LMGetWordRedraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 )
LMGetWordRedraw(void)                                         TWOWORDINLINE(0x1EB8, 0x0BA5);


/*
 *  LMSetWordRedraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetWordRedraw(UInt8 value)                                  TWOWORDINLINE(0x11DF, 0x0BA5);




#if CALL_NOT_IN_CARBON
/*
 *  teclick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
teclick(
  Point *    pt,
  Boolean    fExtend,
  TEHandle   h);


#endif  /* CALL_NOT_IN_CARBON */

#if OLDROUTINENAMES
#if CALL_NOT_IN_CARBON
#define TESetJust(just, hTE) TESetAlignment(just, hTE)
#define TextBox(text, length, box, just) TETextBox(text, length, box, just)
#define TEStylNew(destRect, viewRect) TEStyleNew(destRect, viewRect)
#define SetStylHandle(theHandle, hTE) TESetStyleHandle(theHandle, hTE)
#define SetStyleHandle(theHandle, hTE) TESetStyleHandle (theHandle, hTE)
#define GetStylHandle(hTE) TEGetStyleHandle(hTE)
#define GetStyleHandle(hTE) TEGetStyleHandle(hTE)
#define TEStylPaste(hTE) TEStylePaste(hTE)
#define GetStylScrap(hTE) TEGetStyleScrapHandle(hTE)
#define GetStyleScrap(hTE) TEGetStyleScrapHandle(hTE)
#define SetStylScrap(rangeStart, rangeEnd, newStyles, redraw, hTE) TEUseStyleScrap(rangeStart, rangeEnd, newStyles, redraw, hTE)
#define SetStyleScrap(rangeStart, rangeEnd, newStyles, redraw, hTE)  TEUseStyleScrap(rangeStart, rangeEnd, newStyles, redraw, hTE)
#define TEStylInsert(text, length, hST, hTE) TEStyleInsert(text, length, hST, hTE)
#define TESetScrapLen(length) TESetScrapLength(length)
#define TEGetScrapLen() TEGetScrapLength()
#define SetClikLoop(clikProc, hTE) TESetClickLoop(clikProc, hTE)
#define SetWordBreak(wBrkProc, hTE) TESetWordBreak(wBrkProc, hTE)
#endif  /* CALL_NOT_IN_CARBON */
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

#endif /* __TEXTEDIT__ */

