/*
     File:       IconStorage.h
 
     Contains:   Services to load and share icon family data.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2002 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ICONSTORAGE__
#define __ICONSTORAGE__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif




#if PRAGMA_ONCE
#pragma once
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

/* The following icon types can only be used as an icon element */
/* inside a 'icns' icon family */
enum {
  kThumbnail32BitData           = FOUR_CHAR_CODE('it32'),
  kThumbnail8BitMask            = FOUR_CHAR_CODE('t8mk')
};

enum {
  kHuge1BitMask                 = FOUR_CHAR_CODE('ich#'),
  kHuge4BitData                 = FOUR_CHAR_CODE('ich4'),
  kHuge8BitData                 = FOUR_CHAR_CODE('ich8'),
  kHuge32BitData                = FOUR_CHAR_CODE('ih32'),
  kHuge8BitMask                 = FOUR_CHAR_CODE('h8mk')
};

/* The following icon types can be used as a resource type */
/* or as an icon element type inside a 'icns' icon family */
enum {
  kLarge1BitMask                = FOUR_CHAR_CODE('ICN#'),
  kLarge4BitData                = FOUR_CHAR_CODE('icl4'),
  kLarge8BitData                = FOUR_CHAR_CODE('icl8'),
  kLarge32BitData               = FOUR_CHAR_CODE('il32'),
  kLarge8BitMask                = FOUR_CHAR_CODE('l8mk'),
  kSmall1BitMask                = FOUR_CHAR_CODE('ics#'),
  kSmall4BitData                = FOUR_CHAR_CODE('ics4'),
  kSmall8BitData                = FOUR_CHAR_CODE('ics8'),
  kSmall32BitData               = FOUR_CHAR_CODE('is32'),
  kSmall8BitMask                = FOUR_CHAR_CODE('s8mk'),
  kMini1BitMask                 = FOUR_CHAR_CODE('icm#'),
  kMini4BitData                 = FOUR_CHAR_CODE('icm4'),
  kMini8BitData                 = FOUR_CHAR_CODE('icm8')
};

/* Obsolete. Use names defined above. */
enum {
  large1BitMask                 = kLarge1BitMask,
  large4BitData                 = kLarge4BitData,
  large8BitData                 = kLarge8BitData,
  small1BitMask                 = kSmall1BitMask,
  small4BitData                 = kSmall4BitData,
  small8BitData                 = kSmall8BitData,
  mini1BitMask                  = kMini1BitMask,
  mini4BitData                  = kMini4BitData,
  mini8BitData                  = kMini8BitData
};

/*
    IconFamily 'icns' resources contain an entire IconFamily (all sizes and depths).  
   For custom icons, icns IconFamily resources of the custom icon resource ID are fetched first before
   the classic custom icons (individual 'ics#, ICN#, etc) are fetched.  If the fetch of the icns resource
   succeeds then the icns is looked at exclusively for the icon data.
   For custom icons, new icon features such as 32-bit deep icons are only fetched from the icns resource.
   This is to avoid incompatibilities with cut & paste of new style icons with an older version of the
   MacOS Finder.
   DriverGestalt is called with code kdgMediaIconSuite by IconServices after calling FSM to determine a
   driver icon for a particular device.  The result of the kdgMediaIconSuite call to DriverGestalt should
   be a pointer an an IconFamily.  In this manner driver vendors can provide rich, detailed drive icons
   instead of the 1-bit variety previously supported.
*/
enum {
  kIconFamilyType               = FOUR_CHAR_CODE('icns')
};


struct IconFamilyElement {
  OSType              elementType;            /* 'ICN#', 'icl8', etc...*/
  Size                elementSize;            /* Size of this element*/
  unsigned char       elementData[1];
};
typedef struct IconFamilyElement        IconFamilyElement;
struct IconFamilyResource {
  OSType              resourceType;           /* Always 'icns'*/
  Size                resourceSize;           /* Total size of this resource*/
  IconFamilyElement   elements[1];

};
typedef struct IconFamilyResource       IconFamilyResource;
typedef IconFamilyResource *            IconFamilyPtr;
typedef IconFamilyPtr *                 IconFamilyHandle;
/*  Icon Variants */
/* These can be used as an element of an 'icns' icon family */
/* or as a parameter to GetIconRefVariant */
enum {
  kTileIconVariant              = FOUR_CHAR_CODE('tile'),
  kRolloverIconVariant          = FOUR_CHAR_CODE('over'),
  kDropIconVariant              = FOUR_CHAR_CODE('drop'),
  kOpenIconVariant              = FOUR_CHAR_CODE('open'),
  kOpenDropIconVariant          = FOUR_CHAR_CODE('odrp')
};


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


#endif /* __ICONSTORAGE__ */

