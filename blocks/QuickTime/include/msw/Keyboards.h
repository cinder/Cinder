/*
     File:       Keyboards.h
 
     Contains:   Keyboard API.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __KEYBOARDS__
#define __KEYBOARDS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CFBASE__
#include <CFBase.h>
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

/*----------------------------------------------------------------------------------*/
/* Keyboard API constants                                                           */
/*----------------------------------------------------------------------------------*/
/* Keyboard API Trap Number. Should be moved to Traps.i */
enum {
  _KeyboardDispatch             = 0xAA7A
};

/* Gestalt selector and values for the Keyboard API */
enum {
  gestaltKeyboardsAttr          = FOUR_CHAR_CODE('kbds'),
  gestaltKBPS2Keyboards         = 1,
  gestaltKBPS2SetIDToAny        = 2,
  gestaltKBPS2SetTranslationTable = 4
};

/* Keyboard API Error Codes */
/*
   I stole the range blow from the empty space in the Allocation project but should
   be updated to the officially registered range.
*/
enum {
  errKBPS2KeyboardNotAvailable  = -30850,
  errKBIlligalParameters        = -30851,
  errKBFailSettingID            = -30852,
  errKBFailSettingTranslationTable = -30853,
  errKBFailWritePreference      = -30854
};


/*
 *  PhysicalKeyboardLayoutType
 *  
 *  Summary:
 *    Physical keyboard layout types indicate the physical keyboard
 *    layout. They are returned by the KBGetLayoutType API.
 */
typedef UInt32 PhysicalKeyboardLayoutType;
enum {

  /*
   * A JIS keyboard layout type.
   */
  kKeyboardJIS                  = FOUR_CHAR_CODE('JIS '),

  /*
   * An ANSI keyboard layout type.
   */
  kKeyboardANSI                 = FOUR_CHAR_CODE('ANSI'),

  /*
   * An ISO keyboard layout type.
   */
  kKeyboardISO                  = FOUR_CHAR_CODE('ISO '),

  /*
   * An unknown physical keyboard layout type.
   */
  kKeyboardUnknown              = kUnknownType /* '????'*/
};


/*----------------------------------------------------------------------------------*/
/* Keyboard API types                                                               */
/*----------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------*/
/* Keyboard API routines                                                            */
/*----------------------------------------------------------------------------------*/

#if CALL_NOT_IN_CARBON
/*
 *  KBInitialize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
KBInitialize(void)                                            THREEWORDINLINE(0x303C, 0x0000, 0xAA7A);


/*
 *  KBSetupPS2Keyboard()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
KBSetupPS2Keyboard(
  SInt16   keyboardType,
  SInt8 *  alternativeTable)                                  THREEWORDINLINE(0x303C, 0x0001, 0xAA7A);


/*
 *  KBGetPS2KeyboardID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
KBGetPS2KeyboardID(SInt16 * keyboardType)                     THREEWORDINLINE(0x303C, 0x0002, 0xAA7A);


/*
 *  KBIsPS2KeyboardConnected()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
KBIsPS2KeyboardConnected(void)                                THREEWORDINLINE(0x303C, 0x0003, 0xAA7A);


/*
 *  KBIsPS2KeyboardEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
KBIsPS2KeyboardEnabled(void)                                  THREEWORDINLINE(0x303C, 0x0004, 0xAA7A);


/*
 *  KBGetPS2KeyboardAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 )
KBGetPS2KeyboardAttributes(void)                              THREEWORDINLINE(0x303C, 0x0005, 0xAA7A);


/*
 *  KBSetKCAPForPS2Keyboard()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
KBSetKCAPForPS2Keyboard(Handle kcapHandle)                    THREEWORDINLINE(0x303C, 0x0006, 0xAA7A);


/*
 *  KBSetupPS2KeyboardFromLayoutType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
KBSetupPS2KeyboardFromLayoutType(OSType layoutType)           THREEWORDINLINE(0x303C, 0x0008, 0xAA7A);


/*
 *  KBGetPS2KeyboardLayoutType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
KBGetPS2KeyboardLayoutType(OSType * layoutType)               THREEWORDINLINE(0x303C, 0x0009, 0xAA7A);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  KBGetLayoutType()
 *  
 *  Summary:
 *    Returns the physical keyboard layout type.
 *  
 *  Parameters:
 *    
 *    iKeyboardType:
 *      The keyboard type ID.  LMGetKbdType().
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeyboardsLib 1.0 and later
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSType )
KBGetLayoutType(SInt16 iKeyboardType)                         THREEWORDINLINE(0x303C, 0x0007, 0xAA7A);



/*
 *  KeyboardLayoutRef
 *  
 *  Summary:
 *    The opaque keyboard layout contains information about a keyboard
 *    layout. It is used with the keyboard layout APIs.
 *  
 *  Discussion:
 *    KeyboardLayoutRef APIs follow CoreFoundation function naming
 *    convention. You mustn't release any references you get from APIs
 *    named "Get."
 */
typedef struct OpaqueKeyboardLayoutRef*  KeyboardLayoutRef;

/*
 *  KeyboardLayoutPropertyTag
 *  
 *  Summary:
 *    Keyboard layout property tags specify the value you want to
 *    retrieve. They are used with the KLGetKeyboardLayoutProperty API.
 */
typedef UInt32 KeyboardLayoutPropertyTag;
enum {

  /*
   * The keyboard layout data (const void *).  It is used with the
   * KeyTranslate API.
   */
  kKLKCHRData                   = 0,

  /*
   * The keyboard layout data (const void *).  It is used with the
   * UCKeyTranslate API.
   */
  kKLuchrData                   = 1,

  /*
   * The keyboard layout identifier (SInt32).
   */
  kKLIdentifier                 = 2,

  /*
   * The keyboard layout icon (IconRef).
   */
  kKLIcon                       = 3,

  /*
   * The localized keyboard layout name (CFStringRef).
   */
  kKLLocalizedName              = 4,

  /*
   * The keyboard layout name (CFStringRef).
   */
  kKLName                       = 5,

  /*
   * The keyboard layout group identifier (SInt32).
   */
  kKLGroupIdentifier            = 6,

  /*
   * The keyboard layout kind (SInt32).
   */
  kKLKind                       = 7
};


/*
 *  KeyboardLayoutKind
 *  
 *  Summary:
 *    Keyboard layout kinds indicate available keyboard layout formats.
 */
typedef SInt32 KeyboardLayoutKind;
enum {

  /*
   * Both KCHR and uchr formats are available.
   */
  kKLKCHRuchrKind               = 0,

  /*
   * Only KCHR format is avaiable.
   */
  kKLKCHRKind                   = 1,

  /*
   * Only uchr format is available.
   */
  kKLuchrKind                   = 2
};


/*
 *  KeyboardLayoutIdentifier
 *  
 *  Summary:
 *    Keyboard layout identifiers specify particular keyboard layouts.
 */
typedef SInt32 KeyboardLayoutIdentifier;
enum {
  kKLUSKeyboard                 = 0
};

/* iterate keyboard layouts*/

/*
 *  KLGetKeyboardLayoutCount()
 *  
 *  Summary:
 *    Returns the number of keyboard layouts.
 *  
 *  Parameters:
 *    
 *    oCount:
 *      On exit, the number of keyboard layouts
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetKeyboardLayoutCount(CFIndex * oCount);


/*
 *  KLGetKeyboardLayoutAtIndex()
 *  
 *  Summary:
 *    Retrieves the keyboard layout at the given index.
 *  
 *  Parameters:
 *    
 *    iIndex:
 *      The index of the keyboard layout to retrieve. If the index is
 *      outside the index space of the keyboard layouts (0 to N-1
 *      inclusive, where N is the count of the keyboard layouts), the
 *      behavior is undefined.
 *    
 *    oKeyboardLayout:
 *      On exit, the keyboard layout with the given index.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetKeyboardLayoutAtIndex(
  CFIndex              iIndex,
  KeyboardLayoutRef *  oKeyboardLayout);


/* NOTE: "Indexed" is a wrong name, please use "AtIndex"...*/
/*
 *  KLGetIndexedKeyboardLayout()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetIndexedKeyboardLayout(
  CFIndex              iIndex,
  KeyboardLayoutRef *  oKeyboardLayout);


/* get keyboard layout info*/

/*
 *  KLGetKeyboardLayoutProperty()
 *  
 *  Summary:
 *    Retrives property value for the given keyboard layout and tag.
 *  
 *  Parameters:
 *    
 *    iKeyboardLayout:
 *      The keyboard layout to be queried. If this parameter is not a
 *      valid KeyboardLayoutRef, the behavior is undefined.
 *    
 *    iPropertyTag:
 *      The property tag.
 *    
 *    oValue:
 *      On exit, the property value for the given keyboard layout and
 *      tag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetKeyboardLayoutProperty(
  KeyboardLayoutRef           iKeyboardLayout,
  KeyboardLayoutPropertyTag   iPropertyTag,
  const void **               oValue);


/* get keyboard layout with identifier or name*/

/*
 *  KLGetKeyboardLayoutWithIdentifier()
 *  
 *  Summary:
 *    Retrieves the keyboard layout with the given identifier.
 *  
 *  Discussion:
 *    For now, the identifier is in the range of SInt16 which is
 *    compatible with the Resource Manager resource ID. However, it
 *    will become an arbitrary SInt32 value at some point, so do not
 *    assume it is in SInt16 range or falls into the "script range" of
 *    the resource IDs.
 *  
 *  Parameters:
 *    
 *    iIdentifier:
 *      The keyboard layout identifier.
 *    
 *    oKeyboardLayout:
 *      On exit, the keyboard layout with the given identifier.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetKeyboardLayoutWithIdentifier(
  SInt32               iIdentifier,
  KeyboardLayoutRef *  oKeyboardLayout);


/*
 *  KLGetKeyboardLayoutWithName()
 *  
 *  Summary:
 *    Retrieves the keyboard layout with the given name.
 *  
 *  Parameters:
 *    
 *    iName:
 *      The keyboard layout name.
 *    
 *    oKeyboardLayout:
 *      On exit, the keyboard layout with the given name.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetKeyboardLayoutWithName(
  CFStringRef          iName,
  KeyboardLayoutRef *  oKeyboardLayout);


/* get/set current keyboard layout of the current group identifier*/

/*
 *  KLGetCurrentKeyboardLayout()
 *  
 *  Summary:
 *    Retrieves the current keyboard layout.
 *  
 *  Parameters:
 *    
 *    oKeyboardLayout:
 *      On exit, the current keyboard layout.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLGetCurrentKeyboardLayout(KeyboardLayoutRef * oKeyboardLayout);


/*
 *  KLSetCurrentKeyboardLayout()
 *  
 *  Summary:
 *    Sets the current keyboard layout.
 *  
 *  Parameters:
 *    
 *    iKeyboardLayout:
 *      The keyboard layout.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
KLSetCurrentKeyboardLayout(KeyboardLayoutRef iKeyboardLayout);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARDS__ */

