/*
     File:       PMDefinitions.h
 
     Contains:   Carbon Printing Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1998-2002 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __PMDEFINITIONS__
#define __PMDEFINITIONS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
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

/* Printing objects */
typedef const void *                    PMObject;
typedef struct OpaquePMDialog*          PMDialog;
typedef struct OpaquePMPrintSettings*   PMPrintSettings;
typedef struct OpaquePMPageFormat*      PMPageFormat;
typedef struct OpaquePMPrintContext*    PMPrintContext;
typedef struct OpaquePMPrintSession*    PMPrintSession;
typedef struct OpaquePMPrinter*         PMPrinter;
typedef struct OpaquePMServer*          PMServer;
enum {
  kPMCancel                     = 0x0080 /* user hit cancel button in dialog */
};

#define kPMNoData           NULL        /* for general use */
#define kPMDontWantSize     NULL        /* for parameters which return size information */
#define kPMDontWantData     NULL        /* for parameters which return data */
#define kPMDontWantBoolean  NULL        /* for parameters which take a boolean reference */
#define kPMNoReference      NULL        /* for parameters which take an address pointer */
/* for parameters which take a PrintSettings reference */
#define kPMNoPrintSettings              ((PMPrintSettings)NULL)
/* for parameters which take a PageFormat reference */
#define kPMNoPageFormat                 ((PMPageFormat)NULL)
/* for parameters which take a Server reference */
#define kPMServerLocal                  ((PMServer)NULL)
typedef UInt16 PMDestinationType;
enum {
  kPMDestinationInvalid         = 0,
  kPMDestinationPrinter         = 1,
  kPMDestinationFile            = 2,
  kPMDestinationFax             = 3,
  kPMDestinationPreview         = 4
};

#define kPMDestinationTypeDefault       kPMDestinationPrinter
typedef UInt32 PMTag;
enum {
                                        /* common tags */
  kPMCurrentValue               = FOUR_CHAR_CODE('curr'), /* current setting or value */
  kPMDefaultValue               = FOUR_CHAR_CODE('dflt'), /* default setting or value */
  kPMMinimumValue               = FOUR_CHAR_CODE('minv'), /* the minimum setting or value */
  kPMMaximumValue               = FOUR_CHAR_CODE('maxv'), /* the maximum setting or value */
                                        /* profile tags */
  kPMSourceProfile              = FOUR_CHAR_CODE('srcp'), /* source profile */
                                        /* resolution tags */
  kPMMinRange                   = FOUR_CHAR_CODE('mnrg'), /* Min range supported by a printer */
  kPMMaxRange                   = FOUR_CHAR_CODE('mxrg'), /* Max range supported by a printer */
  kPMMinSquareResolution        = FOUR_CHAR_CODE('mins'), /* Min with X and Y resolution equal */
  kPMMaxSquareResolution        = FOUR_CHAR_CODE('maxs'), /* Max with X and Y resolution equal */
  kPMDefaultResolution          = FOUR_CHAR_CODE('dftr') /* printer default resolution */
};


typedef UInt16 PMOrientation;
enum {
  kPMPortrait                   = 1,
  kPMLandscape                  = 2,
  kPMReversePortrait            = 3,    /* will revert to kPortrait for current drivers */
  kPMReverseLandscape           = 4     /* will revert to kLandscape for current drivers */
};

/* Printer states */
typedef UInt16 PMPrinterState;
enum {
  kPMPrinterIdle                = 3,
  kPMPrinterProcessing          = 4,
  kPMPrinterStopped             = 5
};

enum {
  kSizeOfTPrint                 = 120   /* size of old TPrint record */
};

typedef UInt16 PMColorMode;
enum {
  kPMBlackAndWhite              = 1,
  kPMGray                       = 2,
  kPMColor                      = 3,
  kPMColorModeDuotone           = 4,    /* 2 channels */
  kPMColorModeSpecialColor      = 5     /* to allow for special colors such as metalic, light cyan, etc. */
};

/* Constants to define the ColorSync Intents. These intents may be used */
/* to set an intent part way through a page or for an entire document. */
typedef UInt32 PMColorSyncIntent;
enum {
  kPMColorIntentUndefined       = 0x0000, /* User or application have not declared an intent, use the printer's default. */
  kPMColorIntentAutomatic       = 0x0001, /* Automatically match for photos and graphics anywhere on the page. */
  kPMColorIntentPhoto           = 0x0002, /* Use Photographic (cmPerceptual) intent for all contents. */
  kPMColorIntentBusiness        = 0x0004, /* Use Business Graphics (cmSaturation) intent for all contents. */
  kPMColorIntentRelColor        = 0x0008, /* Use Relative Colormetrics (Logo Colors) for the page. */
  kPMColorIntentAbsColor        = 0x0010, /* Use absolute colormetric for the page. */
  kPMColorIntentUnused          = 0xFFE0 /* Remaining bits unused at this time. */
};

/* Print quality modes "standard options" */
typedef UInt32 PMQualityMode;
enum {
  kPMQualityLowest              = 0x0000, /* Absolute lowest print quality */
  kPMQualityInkSaver            = 0x0001, /* Saves ink but may be slower */
  kPMQualityDraft               = 0x0004, /* Print at highest speed, ink used is secondary consideration */
  kPMQualityNormal              = 0x0008, /* Print in printers "general usage" mode for good balance between quality and speed */
  kPMQualityPhoto               = 0x000B, /* Optimize quality of photos on the page. Speed is not a concern */
  kPMQualityBest                = 0x000D, /* Get best quality output for all objects and photos. */
  kPMQualityHighest             = 0x000F /* Absolute highest quality attained from a printers */
};


/* Constants for our "standard" paper types */
typedef UInt32 PMPaperType;
enum {
  kPMPaperTypeUnknown           = 0x0000, /* Not sure yet what paper type we have. */
  kPMPaperTypePlain             = 0x0001, /* Plain paper */
  kPMPaperTypeCoated            = 0x0002, /* Has a special coating for sharper images and text */
  kPMPaperTypePremium           = 0x0003, /* Special premium coated paper */
  kPMPaperTypeGlossy            = 0x0004, /* High gloss special coating */
  kPMPaperTypeTransparency      = 0x0005, /* Used for overheads */
  kPMPaperTypeTShirt            = 0x0006 /* Used to iron on t-shirts */
};

/* Scaling alignment: */
typedef UInt16 PMScalingAlignment;
enum {
  kPMScalingPinTopLeft          = 1,
  kPMScalingPinTopRight         = 2,
  kPMScalingPinBottomLeft       = 3,
  kPMScalingPinBottomRight      = 4,
  kPMScalingCenterOnPaper       = 5,
  kPMScalingCenterOnImgArea     = 6
};

/* Duplex binding directions: */
typedef UInt16 PMDuplexBinding;
enum {
  kPMDuplexBindingLeftRight     = 1,
  kPMDuplexBindingTopDown       = 2
};

/* Layout directions: */
typedef UInt16 PMLayoutDirection;
enum {
                                        /* Horizontal-major directions: */
  kPMLayoutLeftRightTopBottom   = 1,    /* English reading direction. */
  kPMLayoutLeftRightBottomTop   = 2,
  kPMLayoutRightLeftTopBottom   = 3,
  kPMLayoutRightLeftBottomTop   = 4,    /* Vertical-major directions: */
  kPMLayoutTopBottomLeftRight   = 5,
  kPMLayoutTopBottomRightLeft   = 6,
  kPMLayoutBottomTopLeftRight   = 7,
  kPMLayoutBottomTopRightLeft   = 8
};

/* Page borders: */
typedef UInt16 PMBorderType;
enum {
  kPMBorderSingleHairline       = 1,
  kPMBorderDoubleHairline       = 2,
  kPMBorderSingleThickline      = 3,
  kPMBorderDoubleThickline      = 4
};

/* Useful Constants for PostScript Injection */
enum {
  kPSPageInjectAllPages         = -1,   /* specifies to inject on all pages */
  kPSInjectionMaxDictSize       = 5     /* maximum size of a dictionary used for PSInjection */
};

/* PostScript Injection values for kPSInjectionPlacementKey */
typedef UInt16 PSInjectionPlacement;
enum {
  kPSInjectionBeforeSubsection  = 1,
  kPSInjectionAfterSubsection   = 2,
  kPSInjectionReplaceSubsection = 3
};

/* PostScript Injection values for kPSInjectionSectionKey */
typedef SInt32 PSInjectionSection;
enum {
                                        /* Job */
  kInjectionSectJob             = 1,    /* CoverPage */
  kInjectionSectCoverPage       = 2
};

/* PostScript Injection values for kPSInjectionSubSectionKey */
typedef SInt32 PSInjectionSubsection;
enum {
  kInjectionSubPSAdobe          = 1,    /* %!PS-Adobe           */
  kInjectionSubPSAdobeEPS       = 2,    /* %!PS-Adobe-3.0 EPSF-3.0    */
  kInjectionSubBoundingBox      = 3,    /* BoundingBox          */
  kInjectionSubEndComments      = 4,    /* EndComments          */
  kInjectionSubOrientation      = 5,    /* Orientation          */
  kInjectionSubPages            = 6,    /* Pages            */
  kInjectionSubPageOrder        = 7,    /* PageOrder          */
  kInjectionSubBeginProlog      = 8,    /* BeginProlog          */
  kInjectionSubEndProlog        = 9,    /* EndProlog          */
  kInjectionSubBeginSetup       = 10,   /* BeginSetup          */
  kInjectionSubEndSetup         = 11,   /* EndSetup             */
  kInjectionSubBeginDefaults    = 12,   /* BeginDefaults       */
  kInjectionSubEndDefaults      = 13,   /* EndDefaults          */
  kInjectionSubDocFonts         = 14,   /* DocumentFonts       */
  kInjectionSubDocNeededFonts   = 15,   /* DocumentNeededFonts       */
  kInjectionSubDocSuppliedFonts = 16,   /* DocumentSuppliedFonts  */
  kInjectionSubDocNeededRes     = 17,   /* DocumentNeededResources    */
  kInjectionSubDocSuppliedRes   = 18,   /* DocumentSuppliedResources*/
  kInjectionSubDocCustomColors  = 19,   /* DocumentCustomColors     */
  kInjectionSubDocProcessColors = 20,   /* DocumentProcessColors  */
  kInjectionSubPlateColor       = 21,   /* PlateColor          */
  kInjectionSubPageTrailer      = 22,   /* PageTrailer           */
  kInjectionSubTrailer          = 23,   /* Trailer              */
  kInjectionSubEOF              = 24,   /* EOF                 */
  kInjectionSubBeginFont        = 25,   /* BeginFont          */
  kInjectionSubEndFont          = 26,   /* EndFont              */
  kInjectionSubBeginResource    = 27,   /* BeginResource       */
  kInjectionSubEndResource      = 28,   /* EndResource          */
  kInjectionSubPage             = 29,   /* Page               */
  kInjectionSubBeginPageSetup   = 30,   /* BeginPageSetup        */
  kInjectionSubEndPageSetup     = 31    /* EndPageSetup          */
};


/* Description types */
#define kPMPPDDescriptionType           CFSTR("PMPPDDescriptionType")
/* Document format strings */
#define kPMDocumentFormatDefault        CFSTR("com.apple.documentformat.default")
#define kPMDocumentFormatPDF            CFSTR("application/pdf")
#define kPMDocumentFormatPICT           CFSTR("application/vnd.apple.printing-pict")
#define kPMDocumentFormatPICTPS         CFSTR("application/vnd.apple.printing-pict-ps")
#define kPMDocumentFormatPostScript     CFSTR("application/postscript")
/* Graphic context strings */
#define kPMGraphicsContextDefault       CFSTR("com.apple.graphicscontext.default")
#define kPMGraphicsContextQuickdraw     CFSTR("com.apple.graphicscontext.quickdraw")
#define kPMGraphicsContextCoreGraphics  CFSTR("com.apple.graphicscontext.coregraphics")
/* Data format strings */
#define kPMDataFormatPS                 kPMDocumentFormatPostScript
#define kPMDataFormatPDF                kPMDocumentFormatPDF
#define kPMDataFormatPICT               kPMDocumentFormatPICT
#define kPMDataFormatPICTwPS            kPMDocumentFormatPICTPS
/* PostScript Injection Dictionary Keys */
#define kPSInjectionSectionKey          CFSTR("section")
#define kPSInjectionSubSectionKey       CFSTR("subsection")
#define kPSInjectionPageKey             CFSTR("page")
#define kPSInjectionPlacementKey        CFSTR("place")
#define kPSInjectionPostScriptKey       CFSTR("psdata")

/* OSStatus return codes */
enum {
  kPMNoError                    = noErr,
  kPMGeneralError               = -30870,
  kPMOutOfScope                 = -30871, /* an API call is out of scope */
  kPMInvalidParameter           = paramErr, /* a required parameter is missing or invalid */
  kPMNoDefaultPrinter           = -30872, /* no default printer selected */
  kPMNotImplemented             = -30873, /* this API call is not supported */
  kPMNoSuchEntry                = -30874, /* no such entry */
  kPMInvalidPrintSettings       = -30875, /* the printsettings reference is invalid */
  kPMInvalidPageFormat          = -30876, /* the pageformat reference is invalid */
  kPMValueOutOfRange            = -30877, /* a value passed in is out of range */
  kPMLockIgnored                = -30878 /* the lock value was ignored */
};

enum {
  kPMInvalidPrintSession        = -30879, /* the print session is invalid */
  kPMInvalidPrinter             = -30880, /* the printer reference is invalid */
  kPMObjectInUse                = -30881 /* the object is in use */
};


enum {
  kPMPrintAllPages              = -1
};

enum {
  kPMUnlocked                   = false,
  kPMLocked                     = true
};


struct PMRect {
  double              top;
  double              left;
  double              bottom;
  double              right;
};
typedef struct PMRect                   PMRect;
struct PMResolution {
  double              hRes;
  double              vRes;
};
typedef struct PMResolution             PMResolution;
struct PMLanguageInfo {
  Str32               level;
  Str32               version;
  Str32               release;
};
typedef struct PMLanguageInfo           PMLanguageInfo;

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


#endif /* __PMDEFINITIONS__ */

