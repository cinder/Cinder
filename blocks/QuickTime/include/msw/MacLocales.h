/*
     File:       MacLocales.h
 
     Contains:   Types & prototypes for locale functions
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1998-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACLOCALES__
#define __MACLOCALES__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
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
   -------------------------------------------------------------------------------------------------
   TYPES & CONSTANTS
   -------------------------------------------------------------------------------------------------
*/

typedef struct OpaqueLocaleRef*         LocaleRef;
typedef UInt32                          LocalePartMask;
enum {
                                        /* bit set requests the following:*/
  kLocaleLanguageMask           = 1L << 0, /* ISO 639-1 or -2 language code (2 or 3 letters)*/
  kLocaleLanguageVariantMask    = 1L << 1, /* custom string for language variant*/
  kLocaleScriptMask             = 1L << 2, /* ISO 15924 script code (2 letters)*/
  kLocaleScriptVariantMask      = 1L << 3, /* custom string for script variant*/
  kLocaleRegionMask             = 1L << 4, /* ISO 3166 country/region code (2 letters)*/
  kLocaleRegionVariantMask      = 1L << 5, /* custom string for region variant*/
  kLocaleAllPartsMask           = 0x0000003F /* all of the above*/
};

typedef FourCharCode                    LocaleOperationClass;
/* constants for LocaleOperationClass are in UnicodeUtilities interfaces*/
typedef FourCharCode                    LocaleOperationVariant;
struct LocaleAndVariant {
  LocaleRef           locale;
  LocaleOperationVariant  opVariant;
};
typedef struct LocaleAndVariant         LocaleAndVariant;

typedef UInt32                          LocaleNameMask;
enum {
                                        /* bit set requests the following:*/
  kLocaleNameMask               = 1L << 0, /* name of locale*/
  kLocaleOperationVariantNameMask = 1L << 1, /* name of LocaleOperationVariant*/
  kLocaleAndVariantNameMask     = 0x00000003 /* all of the above*/
};

/*
   -------------------------------------------------------------------------------------------------
   FUNCTION PROTOTYPES
   -------------------------------------------------------------------------------------------------
*/

/* Convert to or from LocaleRefs (and related utilities)*/

/*
 *  LocaleRefFromLangOrRegionCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleRefFromLangOrRegionCode(
  LangCode     lang,
  RegionCode   region,
  LocaleRef *  locale);


/*
 *  LocaleRefFromLocaleString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleRefFromLocaleString(
  const char   localeString[],
  LocaleRef *  locale);


/*
 *  LocaleRefGetPartString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleRefGetPartString(
  LocaleRef        locale,
  LocalePartMask   partMask,
  ByteCount        maxStringLen,
  char             partString[]);


/*
 *  LocaleStringToLangAndRegionCodes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleStringToLangAndRegionCodes(
  const char    localeString[],
  LangCode *    lang,
  RegionCode *  region);


/* Enumerate locales for a LocaleOperationClass */
/*
 *  LocaleOperationCountLocales()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleOperationCountLocales(
  LocaleOperationClass   opClass,
  ItemCount *            localeCount);


/*
 *  LocaleOperationGetLocales()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleOperationGetLocales(
  LocaleOperationClass   opClass,
  ItemCount              maxLocaleCount,
  ItemCount *            actualLocaleCount,
  LocaleAndVariant       localeVariantList[]);


/* Get names for a locale (or a region's language)*/

/*
 *  LocaleGetName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleGetName(
  LocaleRef                locale,
  LocaleOperationVariant   opVariant,
  LocaleNameMask           nameMask,
  LocaleRef                displayLocale,
  UniCharCount             maxNameLen,
  UniCharCount *           actualNameLen,
  UniChar                  displayName[]);


/*
 *  LocaleCountNames()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleCountNames(
  LocaleRef                locale,
  LocaleOperationVariant   opVariant,
  LocaleNameMask           nameMask,
  ItemCount *              nameCount);


/*
 *  LocaleGetIndName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleGetIndName(
  LocaleRef                locale,
  LocaleOperationVariant   opVariant,
  LocaleNameMask           nameMask,
  ItemCount                nameIndex,
  UniCharCount             maxNameLen,
  UniCharCount *           actualNameLen,
  UniChar                  displayName[],
  LocaleRef *              displayLocale);


/*
 *  LocaleGetRegionLanguageName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleGetRegionLanguageName(
  RegionCode   region,
  Str255       languageName);


/* Get names for a LocaleOperationClass*/
/*
 *  LocaleOperationGetName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleOperationGetName(
  LocaleOperationClass   opClass,
  LocaleRef              displayLocale,
  UniCharCount           maxNameLen,
  UniCharCount *         actualNameLen,
  UniChar                displayName[]);


/*
 *  LocaleOperationCountNames()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleOperationCountNames(
  LocaleOperationClass   opClass,
  ItemCount *            nameCount);


/*
 *  LocaleOperationGetIndName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in LocalesLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
LocaleOperationGetIndName(
  LocaleOperationClass   opClass,
  ItemCount              nameIndex,
  UniCharCount           maxNameLen,
  UniCharCount *         actualNameLen,
  UniChar                displayName[],
  LocaleRef *            displayLocale);



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

#endif /* __MACLOCALES__ */

