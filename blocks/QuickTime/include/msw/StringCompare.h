/*
     File:       StringCompare.h
 
     Contains:   Public interfaces for String Comparison and related operations
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __STRINGCOMPARE__
#define __STRINGCOMPARE__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

#ifndef __SCRIPT__
#include <Script.h>
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

/*

    Here are the current System 7 routine names and the translations to the older forms.
    Please use the newer forms in all new code and migrate the older names out of existing
    code as maintenance permits.
    
    NEW NAME                    OLD NAME                    OBSOLETE FORM (no handle)
    
    CompareString (Str255)      IUCompPString (hp only)     IUCompString (hp only)
    CompareText (ptr/len)       IUMagPString                IUMagString
    IdenticalString (Str255)    IUEqualPString (hp only)    IUEqualString  (hp only)
    IdenticalText (ptr/len)     IUMagIDPString              IUMagIDString
    LanguageOrder               IULangOrder
    ScriptOrder                 IUScriptOrder
    StringOrder (Str255)        IUStringOrder (hp only)
    TextOrder (ptr/len)         IUTextOrder

    RelString
    CmpString (a only)                  
    EqualString (hp only)
    
    ReplaceText

    Carbon only supports the new names.  The old names are undefined for Carbon targets.

    InterfaceLib always has exported the old names.  For C macros have been defined to allow
    the use of the new names.  For Pascal and Assembly using the new names will result
    in link errors. 
    
*/

enum {
                                        /* Special language code values for Language Order*/
  systemCurLang                 = -2,   /* current (itlbLang) lang for system script*/
  systemDefLang                 = -3,   /* default (table) lang for system script*/
  currentCurLang                = -4,   /* current (itlbLang) lang for current script*/
  currentDefLang                = -5,   /* default lang for current script*/
  scriptCurLang                 = -6,   /* current (itlbLang) lang for specified script*/
  scriptDefLang                 = -7    /* default language for a specified script*/
};

/* obsolete names*/
enum {
  iuSystemCurLang               = systemCurLang,
  iuSystemDefLang               = systemDefLang,
  iuCurrentCurLang              = currentCurLang,
  iuCurrentDefLang              = currentDefLang,
  iuScriptCurLang               = scriptCurLang,
  iuScriptDefLang               = scriptDefLang
};


/*
 *  These routines are available in Carbon with the new names.
 */
/*
 *  [Mac]ReplaceText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacReplaceText ReplaceText
#endif
EXTERN_API( short )
MacReplaceText(
  Handle   baseText,
  Handle   substitutionText,
  Str15    key)                                               FOURWORDINLINE(0x2F3C, 0x820C, 0xFFDC, 0xA8B5);


/*
 *  ScriptOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
ScriptOrder(
  ScriptCode   script1,
  ScriptCode   script2)                                       THREEWORDINLINE(0x3F3C, 0x001E, 0xA9ED);


/*
 *  [Mac]CompareString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacCompareString CompareString
#endif
EXTERN_API( short )
MacCompareString(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr,
  Handle             itl2Handle);


/*
 *  IdenticalString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
IdenticalString(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr,
  Handle             itl2Handle);


/*
 *  StringOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
StringOrder(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr,
  ScriptCode         aScript,
  ScriptCode         bScript,
  LangCode           aLang,
  LangCode           bLang);


/*
 *  CompareText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
CompareText(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen,
  Handle        itl2Handle)                                   THREEWORDINLINE(0x3F3C, 0x001A, 0xA9ED);


/*
 *  IdenticalText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
IdenticalText(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen,
  Handle        itl2Handle)                                   THREEWORDINLINE(0x3F3C, 0x001C, 0xA9ED);


/*
 *  TextOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
TextOrder(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen,
  ScriptCode    aScript,
  ScriptCode    bScript,
  LangCode      aLang,
  LangCode      bLang)                                        THREEWORDINLINE(0x3F3C, 0x0022, 0xA9ED);


/*
 *  LanguageOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
LanguageOrder(
  LangCode   language1,
  LangCode   language2)                                       THREEWORDINLINE(0x3F3C, 0x0020, 0xA9ED);



/*
 *  These routines are available in InterfaceLib with old names.
 *  Macros are provided for C to allow source code use to the new names.
 */
#if CALL_NOT_IN_CARBON
/*
 *  IUMagPString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUMagPString(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen,
  Handle        itl2Handle)                                   THREEWORDINLINE(0x3F3C, 0x001A, 0xA9ED);


/*
 *  IUMagIDPString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUMagIDPString(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen,
  Handle        itl2Handle)                                   THREEWORDINLINE(0x3F3C, 0x001C, 0xA9ED);


/*
 *  IUTextOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUTextOrder(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen,
  ScriptCode    aScript,
  ScriptCode    bScript,
  LangCode      aLang,
  LangCode      bLang)                                        THREEWORDINLINE(0x3F3C, 0x0022, 0xA9ED);


/*
 *  IULangOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IULangOrder(
  LangCode   language1,
  LangCode   language2)                                       THREEWORDINLINE(0x3F3C, 0x0020, 0xA9ED);


/*
 *  IUScriptOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUScriptOrder(
  ScriptCode   script1,
  ScriptCode   script2)                                       THREEWORDINLINE(0x3F3C, 0x001E, 0xA9ED);


/*
 *  IUMagString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUMagString(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen)                                         THREEWORDINLINE(0x3F3C, 0x000A, 0xA9ED);


/*
 *  IUMagIDString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUMagIDString(
  const void *  aPtr,
  const void *  bPtr,
  short         aLen,
  short         bLen)                                         THREEWORDINLINE(0x3F3C, 0x000C, 0xA9ED);


#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  IUCompPString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUCompPString(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr,
  Handle             itl2Handle);


/*
 *  IUEqualPString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUEqualPString(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr,
  Handle             itl2Handle);


/*
 *  IUStringOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUStringOrder(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr,
  ScriptCode         aScript,
  ScriptCode         bScript,
  LangCode           aLang,
  LangCode           bLang);


/*
 *  IUCompString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUCompString(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr);


/*
 *  IUEqualString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
IUEqualString(
  ConstStr255Param   aStr,
  ConstStr255Param   bStr);


#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
#if TARGET_OS_MAC
    #define CompareString(aStr, bStr, itl2Handle) \
             IUCompPString(aStr, bStr, itl2Handle)
#endif
#define CompareText(aPtr, bPtr, aLen, bLen, itl2Handle) \
         IUMagPString(aPtr, bPtr, aLen, bLen, itl2Handle)
#define IdenticalString(aStr, bStr, itl2Handle) \
         IUEqualPString(aStr, bStr, itl2Handle)
#define IdenticalText(aPtr, bPtr, aLen, bLen, itl2Handle) \
         IUMagIDPString(aPtr, bPtr, aLen, bLen, itl2Handle)
#define StringOrder(aStr, bStr, aScript, bScript, aLang, bLang) \
         IUStringOrder(aStr, bStr, aScript, bScript, aLang, bLang)
#define TextOrder(aPtr, bPtr, aLen, bLen, aScript, bScript, aLang, bLang) \
         IUTextOrder(aPtr, bPtr, aLen, bLen, aScript, bScript, aLang, bLang)
#define LanguageOrder(language1, language2) \
         IULangOrder(language1, language2)
#endif /* CALL_NOT_IN_CARBON */
#if CALL_NOT_IN_CARBON
/*
 *  iucomppstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
iucomppstring(
  const char *  aStr,
  const char *  bStr,
  Handle        intlHandle);


/*
 *  iuequalpstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
iuequalpstring(
  const char *  aStr,
  const char *  bStr,
  Handle        intlHandle);


/*
 *  iustringorder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
iustringorder(
  const char *  aStr,
  const char *  bStr,
  ScriptCode    aScript,
  ScriptCode    bScript,
  LangCode      aLang,
  LangCode      bLang);


/*
 *  iucompstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
iucompstring(
  const char *  aStr,
  const char *  bStr);


/*
 *  iuequalstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
iuequalstring(
  const char *  aStr,
  const char *  bStr);


#endif  /* CALL_NOT_IN_CARBON */


/*
 *  RelString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
RelString(
  ConstStr255Param   str1,
  ConstStr255Param   str2,
  Boolean            caseSensitive,
  Boolean            diacSensitive);


/*
 *  EqualString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
EqualString(
  ConstStr255Param   str1,
  ConstStr255Param   str2,
  Boolean            caseSensitive,
  Boolean            diacSensitive);


/*
 *  relstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( short )
relstring(
  const char *  str1,
  const char *  str2,
  Boolean       caseSensitive,
  Boolean       diacSensitive);


#if CALL_NOT_IN_CARBON
/*
 *  equalstring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
equalstring(
  const char *  str1,
  const char *  str2,
  Boolean       caseSensitive,
  Boolean       diacSensitive);


#endif  /* CALL_NOT_IN_CARBON */




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STRINGCOMPARE__ */

