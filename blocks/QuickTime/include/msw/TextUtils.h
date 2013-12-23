/*
     File:       TextUtils.h
 
     Contains:   Text Utilities Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __TEXTUTILS__
#define __TEXTUTILS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif

#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
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

/*

    Here are the current System 7 routine names and the translations to the older forms.
    Please use the newer forms in all new code and migrate the older names out of existing
    code as maintainance permits.
    
    NEW NAME                    OLD NAMEs                   OBSOLETE FORM (no script code)

    FindScriptRun
    FindWordBreaks                                          NFindWord, FindWord
    GetIndString            
    GetString
    Munger
    NewString               
    SetString               
    StyledLineBreak
    TruncString
    TruncText

    UpperString ($A054)         UprString, UprText
    UppercaseText               SCUpperText (a only)        UpperText ($A456)
    LowercaseText                                           LwrString, LowerText, LwrText ($A056)
    StripDiacritics                                         StripText ($A256)
    UppercaseStripDiacritics                                StripUpperText ($A656)


*/

/* TruncCode, StyledLineBreakCode, and truncation constants moved to QuickDrawText.i */
struct ScriptRunStatus {
  SInt8               script;
  SInt8               runVariant;
};
typedef struct ScriptRunStatus          ScriptRunStatus;
struct BreakTable {
  char                charTypes[256];
  short               tripleLength;
  short               triples[1];
};
typedef struct BreakTable               BreakTable;
typedef BreakTable *                    BreakTablePtr;
struct NBreakTable {
  SInt8               flags1;
  SInt8               flags2;
  short               version;
  short               classTableOff;
  short               auxCTableOff;
  short               backwdTableOff;
  short               forwdTableOff;
  short               doBackup;
  short               length;                 /* length of NBreakTable */
  char                charTypes[256];
  short               tables[1];
};
typedef struct NBreakTable              NBreakTable;
typedef NBreakTable *                   NBreakTablePtr;
/* The following functions are new names that work on 68k and PowerPC*/
/*
 *  Munger()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
Munger(
  Handle        h,
  long          offset,
  const void *  ptr1,
  long          len1,
  const void *  ptr2,
  long          len2)                                         ONEWORDINLINE(0xA9E0);


/*
 *  NewString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringHandle )
NewString(ConstStr255Param theString)                         ONEWORDINLINE(0xA906);


/*
 *  SetString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetString(
  StringHandle       theString,
  ConstStr255Param   strNew)                                  ONEWORDINLINE(0xA907);


/*
 *  GetString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringHandle )
GetString(short stringID)                                     ONEWORDINLINE(0xA9BA);


/*
 *  GetIndString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetIndString(
  Str255   theString,
  short    strListID,
  short    index);


#if CALL_NOT_IN_CARBON
/*
 *  setstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
setstring(
  StringHandle   theString,
  const char *   strNew);


/*
 *  newstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( StringHandle )
newstring(const char * theString);


/*
 *  getindstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
getindstring(
  char *  theString,
  short   strListID,
  short   index);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  FindWordBreaks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FindWordBreaks(
  Ptr             textPtr,
  short           textLength,
  short           offset,
  Boolean         leadingEdge,
  BreakTablePtr   breaks,
  OffsetTable     offsets,
  ScriptCode      script)                                     FOURWORDINLINE(0x2F3C, 0xC012, 0x001A, 0xA8B5);


/*
 *  LowercaseText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LowercaseText(
  Ptr          textPtr,
  short        len,
  ScriptCode   script)                                        SIXWORDINLINE(0x3F3C, 0x0000, 0x2F3C, 0x800A, 0xFFB6, 0xA8B5);


/*
 *  UppercaseText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UppercaseText(
  Ptr          textPtr,
  short        len,
  ScriptCode   script)                                        SIXWORDINLINE(0x3F3C, 0x0400, 0x2F3C, 0x800A, 0xFFB6, 0xA8B5);


/*
 *  StripDiacritics()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StripDiacritics(
  Ptr          textPtr,
  short        len,
  ScriptCode   script)                                        SIXWORDINLINE(0x3F3C, 0x0200, 0x2F3C, 0x800A, 0xFFB6, 0xA8B5);


/*
 *  UppercaseStripDiacritics()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UppercaseStripDiacritics(
  Ptr          textPtr,
  short        len,
  ScriptCode   script)                                        SIXWORDINLINE(0x3F3C, 0x0600, 0x2F3C, 0x800A, 0xFFB6, 0xA8B5);


/*
 *  FindScriptRun()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ScriptRunStatus )
FindScriptRun(
  Ptr     textPtr,
  long    textLen,
  long *  lenUsed)                                            FOURWORDINLINE(0x2F3C, 0x820C, 0x0026, 0xA8B5);


/*
    The following functions are old names, but are required for PowerPC builds
    because InterfaceLib exports these names, instead of the new ones.
*/

#if CALL_NOT_IN_CARBON
/*
 *  FindWord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
FindWord(
  Ptr             textPtr,
  short           textLength,
  short           offset,
  Boolean         leadingEdge,
  BreakTablePtr   breaks,
  OffsetTable     offsets)                                    FOURWORDINLINE(0x2F3C, 0x8012, 0x001A, 0xA8B5);


/*
 *  NFindWord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
NFindWord(
  Ptr              textPtr,
  short            textLength,
  short            offset,
  Boolean          leadingEdge,
  NBreakTablePtr   nbreaks,
  OffsetTable      offsets)                                   FOURWORDINLINE(0x2F3C, 0x8012, 0xFFE2, 0xA8B5);


/*
   On 68K machines, LwrText, LowerText, StripText, UpperText and StripUpperText
   return an error code in register D0, but System 7 PowerMacs do not emulate
   this properly, so checking D0 is unreliable.
*/

/*
 *  LwrText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter LwrText(__A0, __D0)
#endif
EXTERN_API( void )
LwrText(
  Ptr     textPtr,
  short   len)                                                ONEWORDINLINE(0xA056);


/*
 *  LowerText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter LowerText(__A0, __D0)
#endif
EXTERN_API( void )
LowerText(
  Ptr     textPtr,
  short   len)                                                ONEWORDINLINE(0xA056);


/*
 *  StripText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter StripText(__A0, __D0)
#endif
EXTERN_API( void )
StripText(
  Ptr     textPtr,
  short   len)                                                ONEWORDINLINE(0xA256);


/*
 *  UpperText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter UpperText(__A0, __D0)
#endif
EXTERN_API( void )
UpperText(
  Ptr     textPtr,
  short   len)                                                ONEWORDINLINE(0xA456);


/*
 *  StripUpperText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter StripUpperText(__A0, __D0)
#endif
EXTERN_API( void )
StripUpperText(
  Ptr     textPtr,
  short   len)                                                ONEWORDINLINE(0xA656);



/* The following are new names which are exported by InterfaceLib*/

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  UpperString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UpperString(
  Str255    theString,
  Boolean   diacSensitive);


/*
 *  upperstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
upperstring(
  char *    theString,
  Boolean   diacSensitive);


/* The following are macros which map old names to the names exported by InterfaceLib*/
#if OLDROUTINENAMES
#define UprString(theString, diacSensitive)  \
         UpperString(theString, diacSensitive)
#endif  /* OLDROUTINENAMES */

/* Old routine name but no new names are mapped to it:*/
#if CALL_NOT_IN_CARBON
/*
 *  UprText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter UprText(__A0, __D0)
#endif
EXTERN_API( void )
UprText(
  Ptr     textPtr,
  short   len)                                                ONEWORDINLINE(0xA054);


#endif  /* CALL_NOT_IN_CARBON */

/*
    Functions for converting between C and Pascal Strings
    (Previously in Strings.h)
    
    Note: CopyPascalStringToC, CopyCStringToPascal, c2pstrcpy, and p2cstrcpy
          are written to allow inplace conversion.  That is, the src and dst
          parameters can point to the memory location.  These functions
          are available in CarbonLib and CarbonAccessors.o.
          
    Note: c2pstr, C2PStr, p2cstr, and P2CStr are all deprecated.  These functions
          only do inplace conversion and often require casts to call them.  This can
          cause bugs because you can easily cast away a const and change the 
          contents of a read-only buffer.  These functions are available
          in InterfaceLib, or when building for Carbon if you #define OLDP2C,
          then they are available as a macro.
    
*/
/*
 *  c2pstrcpy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
c2pstrcpy(
  Str255        dst,
  const char *  src);


/*
 *  p2cstrcpy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
p2cstrcpy(
  char *             dst,
  ConstStr255Param   src);


/*
 *  CopyPascalStringToC()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CopyPascalStringToC(
  ConstStr255Param   src,
  char *             dst);


/*
 *  CopyCStringToPascal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CopyCStringToPascal(
  const char *  src,
  Str255        dst);


#if CALL_NOT_IN_CARBON
/*
 *  c2pstr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( StringPtr )
c2pstr(char * aStr);


/*
 *  C2PStr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( StringPtr )
C2PStr(Ptr cString);


/*
 *  p2cstr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( char * )
p2cstr(StringPtr aStr);


/*
 *  P2CStr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr )
P2CStr(StringPtr pString);


#endif  /* CALL_NOT_IN_CARBON */

#if !TARGET_OS_MAC
    /* Added for QuickTime 3.0 */
    #define C2PStr(a)  (StringPtr)c2pstr((Ptr)(a))
    #define P2CStr(a)  (Ptr)p2cstr(a)

    #define CopyPascalStringToC(src,dst) p2cstrcpy(dst,src)
    #define CopyCStringToPascal(src,dst) c2pstrcpy(dst,src)
#endif
#if TARGET_OS_MAC && TARGET_API_MAC_CARBON && OLDP2C
    /* macros to help source code that uses deprecated inplace  */
    /* conversion routines to compiler for carbon */
    #define p2cstr(aStr) (p2cstrcpy((char *) aStr, aStr) , (char *) aStr)
    #define c2pstr(aStr) (c2pstrcpy((StringPtr)aStr, aStr) , (StringPtr) aStr)

    #define C2PStr(a)  (StringPtr)c2pstr((Ptr)(a))
    #define P2CStr(a)  (Ptr)p2cstr(a)
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

#endif /* __TEXTUTILS__ */

