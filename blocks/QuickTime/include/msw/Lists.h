/*
     File:       Lists.h
 
     Contains:   List Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __LISTS__
#define __LISTS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
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

typedef Point                           Cell;
typedef Rect                            ListBounds;
typedef char                            DataArray[32001];
typedef char *                          DataPtr;
typedef DataPtr *                       DataHandle;
typedef CALLBACK_API( short , ListSearchProcPtr )(Ptr aPtr, Ptr bPtr, short aLen, short bLen);
typedef CALLBACK_API_REGISTER68K( Boolean , ListClickLoopProcPtr, (void) );
typedef STACK_UPP_TYPE(ListSearchProcPtr)                       ListSearchUPP;
typedef REGISTER_UPP_TYPE(ListClickLoopProcPtr)                 ListClickLoopUPP;
#if !TARGET_OS_MAC
/* QuickTime 3.0 */
typedef long                            ListNotification;
enum {
  listNotifyNothing             = FOUR_CHAR_CODE('nada'), /* No (null) notification*/
  listNotifyClick               = FOUR_CHAR_CODE('clik'), /* Control was clicked*/
  listNotifyDoubleClick         = FOUR_CHAR_CODE('dblc'), /* Control was double-clicked*/
  listNotifyPreClick            = FOUR_CHAR_CODE('pclk') /* Control about to be clicked*/
};

#endif  /* !TARGET_OS_MAC */

struct ListRec {
  Rect                rView;                  /* in Carbon use Get/SetListViewBounds*/
  GrafPtr             port;                   /* in Carbon use Get/SetListPort*/
  Point               indent;                 /* in Carbon use Get/SetListCellIndent*/
  Point               cellSize;               /* in Carbon use Get/SetListCellSize*/
  ListBounds          visible;                /* in Carbon use GetListVisibleCells*/
  ControlRef          vScroll;                /* in Carbon use GetListVerticalScrollBar*/
  ControlRef          hScroll;                /* in Carbon use GetListHorizontalScrollBar*/
  SInt8               selFlags;               /* in Carbon use Get/SetListSelectionFlags*/
  Boolean             lActive;                /* in Carbon use LActivate, GetListActive*/
  SInt8               lReserved;              /* not supported in Carbon */
  SInt8               listFlags;              /* in Carbon use Get/SetListFlags */
  long                clikTime;               /* in Carbon use Get/SetListClickTime*/
  Point               clikLoc;                /* in Carbon use GetListClickLocation*/
  Point               mouseLoc;               /* in Carbon use GetListMouseLocation*/
  ListClickLoopUPP    lClickLoop;             /* in Carbon use Get/SetListClickLoop*/
  Cell                lastClick;              /* in Carbon use SetListLastClick*/
  long                refCon;                 /* in Carbon use Get/SetListRefCon*/
  Handle              listDefProc;            /* not supported in Carbon */
  Handle              userHandle;             /* in Carbon use Get/SetListUserHandle*/
  ListBounds          dataBounds;             /* in Carbon use GetListDataBounds*/
  DataHandle          cells;                  /* in Carbon use LGet/SetCell*/
  short               maxIndex;               /* in Carbon use LGet/SetCell*/
  short               cellArray[1];           /* in Carbon use LGet/SetCell*/
};
typedef struct ListRec                  ListRec;
typedef ListRec *                       ListPtr;
typedef ListPtr *                       ListHandle;
/* ListRef is obsolete.  Use ListHandle. */
typedef ListHandle                      ListRef;


enum {
                                        /* ListRec.listFlags bits*/
  lDrawingModeOffBit            = 3,
  lDoVAutoscrollBit             = 1,
  lDoHAutoscrollBit             = 0
};

enum {
                                        /* ListRec.listFlags masks*/
  lDrawingModeOff               = 8,
  lDoVAutoscroll                = 2,
  lDoHAutoscroll                = 1
};


enum {
                                        /* ListRec.selFlags bits*/
  lOnlyOneBit                   = 7,
  lExtendDragBit                = 6,
  lNoDisjointBit                = 5,
  lNoExtendBit                  = 4,
  lNoRectBit                    = 3,
  lUseSenseBit                  = 2,
  lNoNilHiliteBit               = 1
};


enum {
                                        /* ListRec.selFlags masks*/
  lOnlyOne                      = -128,
  lExtendDrag                   = 64,
  lNoDisjoint                   = 32,
  lNoExtend                     = 16,
  lNoRect                       = 8,
  lUseSense                     = 4,
  lNoNilHilite                  = 2
};


enum {
                                        /* LDEF messages*/
  lInitMsg                      = 0,
  lDrawMsg                      = 1,
  lHiliteMsg                    = 2,
  lCloseMsg                     = 3
};

/*
   StandardIconListCellDataRec is the cell data format for
   use with the standard icon list (kListDefStandardIconType).
*/
struct StandardIconListCellDataRec {
  Handle              iconHandle;
  short               font;
  short               face;
  short               size;
  Str255              name;
};
typedef struct StandardIconListCellDataRec StandardIconListCellDataRec;
typedef StandardIconListCellDataRec *   StandardIconListCellDataPtr;


typedef CALLBACK_API( void , ListDefProcPtr )(short lMessage, Boolean lSelect, Rect *lRect, Cell lCell, short lDataOffset, short lDataLen, ListHandle lHandle);
typedef STACK_UPP_TYPE(ListDefProcPtr)                          ListDefUPP;
/*
 *  NewListSearchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ListSearchUPP )
NewListSearchUPP(ListSearchProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppListSearchProcInfo = 0x00002BE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 2_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ListSearchUPP) NewListSearchUPP(ListSearchProcPtr userRoutine) { return (ListSearchUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppListSearchProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewListSearchUPP(userRoutine) (ListSearchUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppListSearchProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewListClickLoopUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ListClickLoopUPP )
NewListClickLoopUPP(ListClickLoopProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppListClickLoopProcInfo = 0x00000012 };  /* register 1_byte:D0 Func() */
  #ifdef __cplusplus
    inline DEFINE_API_C(ListClickLoopUPP) NewListClickLoopUPP(ListClickLoopProcPtr userRoutine) { return (ListClickLoopUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppListClickLoopProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewListClickLoopUPP(userRoutine) (ListClickLoopUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppListClickLoopProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewListDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ListDefUPP )
NewListDefUPP(ListDefProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppListDefProcInfo = 0x000EBD80 };  /* pascal no_return_value Func(2_bytes, 1_byte, 4_bytes, 4_bytes, 2_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ListDefUPP) NewListDefUPP(ListDefProcPtr userRoutine) { return (ListDefUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppListDefProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewListDefUPP(userRoutine) (ListDefUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppListDefProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeListSearchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeListSearchUPP(ListSearchUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeListSearchUPP(ListSearchUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeListSearchUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeListClickLoopUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeListClickLoopUPP(ListClickLoopUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeListClickLoopUPP(ListClickLoopUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeListClickLoopUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeListDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeListDefUPP(ListDefUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeListDefUPP(ListDefUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeListDefUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeListSearchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( short )
InvokeListSearchUPP(
  Ptr            aPtr,
  Ptr            bPtr,
  short          aLen,
  short          bLen,
  ListSearchUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(short) InvokeListSearchUPP(Ptr aPtr, Ptr bPtr, short aLen, short bLen, ListSearchUPP userUPP) { return (short)CALL_FOUR_PARAMETER_UPP(userUPP, uppListSearchProcInfo, aPtr, bPtr, aLen, bLen); }
  #else
    #define InvokeListSearchUPP(aPtr, bPtr, aLen, bLen, userUPP) (short)CALL_FOUR_PARAMETER_UPP((userUPP), uppListSearchProcInfo, (aPtr), (bPtr), (aLen), (bLen))
  #endif
#endif

/*
 *  InvokeListClickLoopUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 InvokeListClickLoopUPP(__A0)
#endif
EXTERN_API_C( Boolean )
InvokeListClickLoopUPP(ListClickLoopUPP userUPP)              ONEWORDINLINE(0x4E90);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeListClickLoopUPP(ListClickLoopUPP userUPP) { return (Boolean)CALL_ZERO_PARAMETER_UPP(userUPP, uppListClickLoopProcInfo); }
  #else
    #define InvokeListClickLoopUPP(userUPP) (Boolean)CALL_ZERO_PARAMETER_UPP((userUPP), uppListClickLoopProcInfo)
  #endif
#endif

/*
 *  InvokeListDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeListDefUPP(
  short       lMessage,
  Boolean     lSelect,
  Rect *      lRect,
  Cell        lCell,
  short       lDataOffset,
  short       lDataLen,
  ListHandle  lHandle,
  ListDefUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeListDefUPP(short lMessage, Boolean lSelect, Rect * lRect, Cell lCell, short lDataOffset, short lDataLen, ListHandle lHandle, ListDefUPP userUPP) { CALL_SEVEN_PARAMETER_UPP(userUPP, uppListDefProcInfo, lMessage, lSelect, lRect, lCell, lDataOffset, lDataLen, lHandle); }
  #else
    #define InvokeListDefUPP(lMessage, lSelect, lRect, lCell, lDataOffset, lDataLen, lHandle, userUPP) CALL_SEVEN_PARAMETER_UPP((userUPP), uppListDefProcInfo, (lMessage), (lSelect), (lRect), (lCell), (lDataOffset), (lDataLen), (lHandle))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewListSearchProc(userRoutine)                      NewListSearchUPP(userRoutine)
    #define NewListClickLoopProc(userRoutine)                   NewListClickLoopUPP(userRoutine)
    #define NewListDefProc(userRoutine)                         NewListDefUPP(userRoutine)
    #define CallListSearchProc(userRoutine, aPtr, bPtr, aLen, bLen) InvokeListSearchUPP(aPtr, bPtr, aLen, bLen, userRoutine)
    #define CallListClickLoopProc(userRoutine)                  InvokeListClickLoopUPP(userRoutine)
    #define CallListDefProc(userRoutine, lMessage, lSelect, lRect, lCell, lDataOffset, lDataLen, lHandle) InvokeListDefUPP(lMessage, lSelect, lRect, lCell, lDataOffset, lDataLen, lHandle, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

enum {
  kListDefProcPtr               = 0,
  kListDefUserProcType          = kListDefProcPtr,
  kListDefStandardTextType      = 1,
  kListDefStandardIconType      = 2
};

typedef UInt32                          ListDefType;
struct ListDefSpec {
  ListDefType         defType;
  union {
    ListDefUPP          userProc;
  }                       u;
};
typedef struct ListDefSpec              ListDefSpec;
typedef ListDefSpec *                   ListDefSpecPtr;
/*
 *  CreateCustomList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateCustomList(
  const Rect *         rView,
  const ListBounds *   dataBounds,
  Point                cellSize,
  const ListDefSpec *  theSpec,
  WindowRef            theWindow,
  Boolean              drawIt,
  Boolean              hasGrow,
  Boolean              scrollHoriz,
  Boolean              scrollVert,
  ListHandle *         outList);



#if !TARGET_OS_MAC
/* QuickTime 3.0 */
typedef CALLBACK_API( void , ListNotificationProcPtr )(ListHandle theList, ListNotification notification, long param);
typedef ListNotificationProcPtr         ListNotificationUPP;
#if CALL_NOT_IN_CARBON
/*
 *  LSetNotificationCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
LSetNotificationCallback(
  ListNotificationProcPtr   callBack,
  ListHandle                lHandle);


/*
 *  GetListVisibleBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
GetListVisibleBounds(
  ListHandle   theList,
  Rect *       visibleBounds);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !TARGET_OS_MAC */

/*
 *  LNew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ListHandle )
LNew(
  const Rect *        rView,
  const ListBounds *  dataBounds,
  Point               cSize,
  short               theProc,
  WindowRef           theWindow,
  Boolean             drawIt,
  Boolean             hasGrow,
  Boolean             scrollHoriz,
  Boolean             scrollVert)                             THREEWORDINLINE(0x3F3C, 0x0044, 0xA9E7);


/*
 *  LDispose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LDispose(ListHandle lHandle)                                  THREEWORDINLINE(0x3F3C, 0x0028, 0xA9E7);


/*
 *  LAddColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
LAddColumn(
  short        count,
  short        colNum,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0004, 0xA9E7);


/*
 *  LAddRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
LAddRow(
  short        count,
  short        rowNum,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0008, 0xA9E7);


/*
 *  LDelColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LDelColumn(
  short        count,
  short        colNum,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0020, 0xA9E7);


/*
 *  LDelRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LDelRow(
  short        count,
  short        rowNum,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0024, 0xA9E7);


/*
 *  LGetSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
LGetSelect(
  Boolean      next,
  Cell *       theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x003C, 0xA9E7);


/*
 *  LLastClick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Cell )
LLastClick(ListHandle lHandle)                                THREEWORDINLINE(0x3F3C, 0x0040, 0xA9E7);


/*
 *  LNextCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
LNextCell(
  Boolean      hNext,
  Boolean      vNext,
  Cell *       theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0048, 0xA9E7);


/*
 *  LSearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
LSearch(
  const void *    dataPtr,
  short           dataLen,
  ListSearchUPP   searchProc,
  Cell *          theCell,
  ListHandle      lHandle)                                    THREEWORDINLINE(0x3F3C, 0x0054, 0xA9E7);


/*
 *  LSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LSize(
  short        listWidth,
  short        listHeight,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0060, 0xA9E7);


/*
 *  LSetDrawingMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LSetDrawingMode(
  Boolean      drawIt,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x002C, 0xA9E7);


/*
 *  LScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LScroll(
  short        dCols,
  short        dRows,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0050, 0xA9E7);


/*
 *  LAutoScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LAutoScroll(ListHandle lHandle)                               THREEWORDINLINE(0x3F3C, 0x0010, 0xA9E7);


/*
 *  LUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LUpdate(
  RgnHandle    theRgn,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0064, 0xA9E7);


/*
 *  LActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LActivate(
  Boolean      act,
  ListHandle   lHandle)                                       TWOWORDINLINE(0x4267, 0xA9E7);


/*
 *  LCellSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LCellSize(
  Point        cSize,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0014, 0xA9E7);


/*
 *  LClick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
LClick(
  Point            pt,
  EventModifiers   modifiers,
  ListHandle       lHandle)                                   THREEWORDINLINE(0x3F3C, 0x0018, 0xA9E7);


/*
 *  LAddToCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LAddToCell(
  const void *  dataPtr,
  short         dataLen,
  Cell          theCell,
  ListHandle    lHandle)                                      THREEWORDINLINE(0x3F3C, 0x000C, 0xA9E7);


/*
 *  LClrCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LClrCell(
  Cell         theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x001C, 0xA9E7);


/*
 *  LGetCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LGetCell(
  void *       dataPtr,
  short *      dataLen,
  Cell         theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0038, 0xA9E7);


/*
 *  LRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LRect(
  Rect *       cellRect,
  Cell         theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x004C, 0xA9E7);


/*
 *  LSetCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LSetCell(
  const void *  dataPtr,
  short         dataLen,
  Cell          theCell,
  ListHandle    lHandle)                                      THREEWORDINLINE(0x3F3C, 0x0058, 0xA9E7);


/*
 *  LSetSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LSetSelect(
  Boolean      setIt,
  Cell         theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x005C, 0xA9E7);


/*
 *  LDraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LDraw(
  Cell         theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0030, 0xA9E7);


/*
 *  LGetCellDataLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LGetCellDataLocation(
  short *      offset,
  short *      len,
  Cell         theCell,
  ListHandle   lHandle)                                       THREEWORDINLINE(0x3F3C, 0x0034, 0xA9E7);


/* Routines available in Carbon only*/

/*
 *  RegisterListDefinition()
 *  
 *  Summary:
 *    Registers a binding between a resource ID and a list definition
 *    function.
 *  
 *  Discussion:
 *    In the Mac OS 8.x List Manager, a 'ldes' resource can contain an
 *    embedded LDEF procID that is used by the List Manager as the
 *    resource ID of an 'LDEF' resource to measure and draw the list.
 *    Since LDEFs can no longer be packaged as code resources on
 *    Carbon, the procID can no longer refer directly to an LDEF
 *    resource. However, using RegisterListDefinition you can instead
 *    specify a UniversalProcPtr pointing to code in your application
 *    code fragment.
 *  
 *  Parameters:
 *    
 *    inResID:
 *      An LDEF proc ID, as used in a 'ldes' resource.
 *    
 *    inDefSpec:
 *      Specifies the ListDefUPP that should be used for lists with the
 *      given LDEF procID.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RegisterListDefinition(
  SInt16           inResID,
  ListDefSpecPtr   inDefSpec);


#if CALL_NOT_IN_CARBON
#if CALL_NOT_IN_CARBON
/*
 *  SetListDefinitionProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
SetListDefinitionProc(
  SInt16       resID,
  ListDefUPP   defProc);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* CALL_NOT_IN_CARBON */

#if !TARGET_OS_MAC
/* QuickTime 3.0 */
#if CALL_NOT_IN_CARBON
/*
 *  LSetLDEF()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
LSetLDEF(
  ListDefProcPtr   proc,
  ListHandle       lHandle);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !TARGET_OS_MAC */

#if CALL_NOT_IN_CARBON
/*
 *  laddtocell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
laddtocell(
  const void *  dataPtr,
  short         dataLen,
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lclrcell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lclrcell(
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lgetcelldatalocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lgetcelldatalocation(
  short *       offset,
  short *       len,
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lgetcell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lgetcell(
  void *        dataPtr,
  short *       dataLen,
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lnew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( ListHandle )
lnew(
  const Rect *        rView,
  const ListBounds *  dataBounds,
  Point *             cSize,
  short               theProc,
  WindowRef           theWindow,
  Boolean             drawIt,
  Boolean             hasGrow,
  Boolean             scrollHoriz,
  Boolean             scrollVert);


/*
 *  lrect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lrect(
  Rect *        cellRect,
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lsetcell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lsetcell(
  const void *  dataPtr,
  short         dataLen,
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lsetselect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lsetselect(
  Boolean       setIt,
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  ldraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
ldraw(
  const Cell *  theCell,
  ListHandle    lHandle);


/*
 *  lclick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
lclick(
  Point *          pt,
  EventModifiers   modifiers,
  ListHandle       lHandle);


/*
 *  lcellsize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
lcellsize(
  Point *      cSize,
  ListHandle   lHandle);


#endif  /* CALL_NOT_IN_CARBON */

#if OLDROUTINENAMES
#define LDoDraw(drawIt, lHandle) LSetDrawingMode(drawIt, lHandle)
#define LFind(offset, len, theCell, lHandle) LGetCellDataLocation(offset, len, theCell, lHandle)
#if CGLUESUPPORTED
#define lfind(offset, len, theCell, lHandle) lgetcelldatalocation(offset, len, theCell, lHandle)
#endif
#endif  /* OLDROUTINENAMES */

#if ACCESSOR_CALLS_ARE_FUNCTIONS
/* Getters */
/*
 *  GetListViewBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Rect * )
GetListViewBounds(
  ListHandle   list,
  Rect *       view);


/*
 *  GetListPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CGrafPtr )
GetListPort(ListHandle list);


/*
 *  GetListCellIndent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
GetListCellIndent(
  ListHandle   list,
  Point *      indent);


/*
 *  GetListCellSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
GetListCellSize(
  ListHandle   list,
  Point *      size);


/*
 *  GetListVisibleCells()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ListBounds * )
GetListVisibleCells(
  ListHandle    list,
  ListBounds *  visible);


/*
 *  GetListVerticalScrollBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ControlRef )
GetListVerticalScrollBar(ListHandle list);


/*
 *  GetListHorizontalScrollBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ControlRef )
GetListHorizontalScrollBar(ListHandle list);


/*
 *  GetListActive()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
GetListActive(ListHandle list);


/*
 *  GetListClickTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
GetListClickTime(ListHandle list);


/*
 *  GetListClickLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
GetListClickLocation(
  ListHandle   list,
  Point *      click);


/*
 *  GetListMouseLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
GetListMouseLocation(
  ListHandle   list,
  Point *      mouse);


/*
 *  GetListClickLoop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ListClickLoopUPP )
GetListClickLoop(ListHandle list);


/*
 *  GetListRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
GetListRefCon(ListHandle list);


/*
 *  GetListDefinition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
GetListDefinition(ListHandle list);


/*
 *  GetListUserHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
GetListUserHandle(ListHandle list);


/*
 *  GetListDataBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ListBounds * )
GetListDataBounds(
  ListHandle    list,
  ListBounds *  bounds);


/*
 *  GetListDataHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DataHandle )
GetListDataHandle(ListHandle list);


/*
 *  GetListFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OptionBits )
GetListFlags(ListHandle list);


/*
 *  GetListSelectionFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OptionBits )
GetListSelectionFlags(ListHandle list);


/* Setters */
/*
 *  SetListViewBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListViewBounds(
  ListHandle    list,
  const Rect *  view);


/*
 *  SetListPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListPort(
  ListHandle   list,
  CGrafPtr     port);


/*
 *  SetListCellIndent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListCellIndent(
  ListHandle   list,
  Point *      indent);


/*
 *  SetListClickTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListClickTime(
  ListHandle   list,
  SInt32       time);


/*
 *  SetListClickLoop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListClickLoop(
  ListHandle         list,
  ListClickLoopUPP   clickLoop);


/*
 *  SetListLastClick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListLastClick(
  ListHandle   list,
  Cell *       lastClick);


/*
 *  SetListRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListRefCon(
  ListHandle   list,
  SInt32       refCon);


/*
 *  SetListUserHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListUserHandle(
  ListHandle   list,
  Handle       userHandle);


/*
 *  SetListFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListFlags(
  ListHandle   list,
  OptionBits   listFlags);


/*
 *  SetListSelectionFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetListSelectionFlags(
  ListHandle   list,
  OptionBits   selectionFlags);


#endif  /* ACCESSOR_CALLS_ARE_FUNCTIONS */


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

#endif /* __LISTS__ */

