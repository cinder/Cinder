/*
     File:       ListManagerComponent.h
 
     Contains:   QuickTime Interfaces.
 
     Version:    Technology: QuickTime 6.0
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1990-2002 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __LISTMANAGERCOMPONENT__
#define __LISTMANAGERCOMPONENT__

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __LISTS__
#include <Lists.h>
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
  kLISTComponentType            = FOUR_CHAR_CODE('list'),
  kLISTComponentSubType         = FOUR_CHAR_CODE('list')
};

#if CALL_NOT_IN_CARBON
/*
 *  LISTCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTCreate(
  ComponentInstance   ci,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0001, 0x7000, 0xA82A);


/*
 *  LISTLDispose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLDispose(
  ComponentInstance   list,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0002, 0x7000, 0xA82A);


/*
 *  LISTLAddRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLAddRow(
  ComponentInstance   list,
  short               count,
  short               rowNum,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x0003, 0x7000, 0xA82A);


/*
 *  LISTLDelRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLDelRow(
  ComponentInstance   list,
  short               count,
  short               rowNum,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x0004, 0x7000, 0xA82A);


/*
 *  LISTLSetDrawingMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLSetDrawingMode(
  ComponentInstance   list,
  Boolean             drawIt,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0006, 0x0005, 0x7000, 0xA82A);


/*
 *  LISTLAutoScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLAutoScroll(
  ComponentInstance   list,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0006, 0x7000, 0xA82A);


/*
 *  LISTLActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLActivate(
  ComponentInstance   list,
  Boolean             act,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0006, 0x0007, 0x7000, 0xA82A);


/*
 *  LISTLAddToCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLAddToCell(
  ComponentInstance   list,
  const void *        dataPtr,
  short               dataLen,
  Cell                theCell,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x000E, 0x0008, 0x7000, 0xA82A);


/*
 *  LISTLClrCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLClrCell(
  ComponentInstance   list,
  Cell                theCell,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x0009, 0x7000, 0xA82A);


/*
 *  LISTLSetCell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLSetCell(
  ComponentInstance   list,
  const void *        dataPtr,
  short               dataLen,
  Cell                theCell,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x000E, 0x000A, 0x7000, 0xA82A);


/*
 *  LISTLSetSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLSetSelect(
  ComponentInstance   list,
  Boolean             setIt,
  Cell                theCell,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x000A, 0x000B, 0x7000, 0xA82A);


/*
 *  LISTLDraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLDraw(
  ComponentInstance   list,
  Cell                theCell,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x000C, 0x7000, 0xA82A);


/*
 *  LISTGetVisibleBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTGetVisibleBounds(
  ComponentInstance   list,
  Rect *              bounds,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x000D, 0x7000, 0xA82A);


/*
 *  LISTLScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
LISTLScroll(
  ComponentInstance   list,
  short               dh,
  short               dv,
  ListHandle          lHandle)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x000E, 0x7000, 0xA82A);




/* selectors for component calls */
enum {
    kLISTCreateSelect                          = 0x0001,
    kLISTLDisposeSelect                        = 0x0002,
    kLISTLAddRowSelect                         = 0x0003,
    kLISTLDelRowSelect                         = 0x0004,
    kLISTLSetDrawingModeSelect                 = 0x0005,
    kLISTLAutoScrollSelect                     = 0x0006,
    kLISTLActivateSelect                       = 0x0007,
    kLISTLAddToCellSelect                      = 0x0008,
    kLISTLClrCellSelect                        = 0x0009,
    kLISTLSetCellSelect                        = 0x000A,
    kLISTLSetSelectSelect                      = 0x000B,
    kLISTLDrawSelect                           = 0x000C,
    kLISTGetVisibleBoundsSelect                = 0x000D,
    kLISTLScrollSelect                         = 0x000E
};
#endif  /* CALL_NOT_IN_CARBON */


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

#endif /* __LISTMANAGERCOMPONENT__ */

