/*
     File:       PMTicket.h
 
     Contains:   Mac OS X Printing Manager Job Ticket Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFNUMBER__
#include <CFNumber.h>
#endif

#ifndef __CFDATE__
#include <CFDate.h>
#endif

#ifndef __PMERRORS__
#include <PMErrors.h>
#endif

#ifndef __PMDEFINITIONS__
#include <PMDefinitions.h>
#endif

#include <stdio.h>


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

/* SECTION I: DATA TYPES, STUCTURES, and CONSTANTS */
/* General opaque Ticket object reference. This is returned when you create a PageFormat, */
/* PrintSettings, DocumentInfo, JobInfo, or JobTicket. A list of Job Tickets or other */
/* tickets can be saved as a list of references. */
typedef struct OpaquePMTicketRef*       PMTicketRef;
typedef struct OpaquePMTemplateRef*     PMTemplateRef;
/* A few constants that will be useful in calling Job Ticket functions. */
enum {
  kPMUnlocked                   = false,
  kPMLocked                     = true
};

#define kPMDontFetchItem    NULL   /* Used to ask GetItem if an item exists. */
/* The following constant determines if the ticket code sets an item in the top level ticket */
/* or down in some sub-ticket within the top level ticket. If passed as the document number, */
/* the ticket code will simply set the item in the ticket passed. If the document number is */
/* non "kTopLevel" but the page number is "kTopLevel", the ticket code will set the item in the */
/* document level sub-ticket. If both parameters are non "kTopLevel", the ticket code will set */
/* the item in the correct sub ticket for the document and page. */
/* For now, all calls should use kPMTopLevel for document and page numbers, or the passed in */
/* docment and page numbers if they are available. In the future we expect to enable embedded */
/* document and page tickets. */
enum {
  kPMTopLevel                   = 0
};

/* There are several different types of tickets that use the same accessor functions and  */
/*  allow the programmer to use different keys to access the proper fields in each ticket. */
/*      A Job Ticket has the following structure: */
/*          Job Ticket */
/*              Job Ticket Fields */
/*              Page Format Ticket (required) */
/*                  Paper Info Ticket (required) */
/*              Print Settings Ticket (required) */
/*                  Paper Info Ticket (required) */
/* DOWNSTREAM TICKETS - Heading from PC or PJC to other modules. */
#define kPMJobTicket                    CFSTR("com.apple.print.JobTicket")
#define kPMDocumentTicket               CFSTR("com.apple.print.DocumentTicket")
#define kPMPageTicket                   CFSTR("com.apple.print.PageTicket")
#define kPMPageFormatTicket             CFSTR("com.apple.print.PageFormatTicket")
#define kPMPrintSettingsTicket          CFSTR("com.apple.print.PrintSettingsTicket")
#define kPMDestinationTicket            CFSTR("com.apple.print.DestinationTicket")
#define kPMConverterSetupTicket         CFSTR("com.apple.print.ConverterSetupTicket")
/* UPSTREAM TICKETS - Expected to be created by low level modules and passed up. */
#define kPMPrinterInfoTicket            CFSTR("com.apple.print.PrinterInfoTicket")
#define kPMModuleInfoTicket             CFSTR("com.apple.print.ModuleInfoTicket")
/* OTHER TICKETS, going either direction */
#define kPMTicketList                   CFSTR("com.apple.print.TicketList")
#define kPMPaperInfoTicket              CFSTR("com.apple.print.PaperInfoTicket")
/* An enum to keep track in a programmatic way of which type of ticket we're working */
/* with. These do not need to match up with the order of tickets listed in the ticket */
/* types in PMTicket.h because we convert from string to enum in an internal function. */
/* Warning: Don't use these tags when creating tickets.  Use the CFSTRs above. */
typedef SInt16 PMTicketType;
enum {
  kPMTicketTypeUnknown          = -1,
  kPMJobTicketType              = 1,
  kPMDocumentTicketType         = 2,
  kPMPageTicketType             = 3,
  kPMPageFormatTicketType       = 4,
  kPMPrintSettingsTicketType    = 5,
  kPMPrinterInfoTicketType      = 6,
  kPMDestinationTicketType      = 7,
  kPMConverterSetupTicketType   = 8,
  kPMModuleInfoTicketType       = 9,
  kPMTicketListType             = 10,
  kPMPaperInfoTicketType        = 11
};


/*
 *  PMPrintingPhaseType
 *  
 *  Discussion:
 *    An enum to demark where in the printing process we are. This
 *    value will be stored in the print settings ticket at the top
 *    level in the Job Ticket. It will be used to signal shift from one
 *    phase to the next and may be set by just about any part of the
 *    system. Users will want to check for ">" (greater than) or "<"
 *    (less than) conditions when possible, allowing for additional
 *    phases to be defined later on.
 */
typedef UInt16 PMPrintingPhaseType;
enum {
  kPMPhaseUnknown               = 0,    /* Not clear what phase we're in yet. */
  kPMPhasePreDialog             = 1,    /* Just before we execute the code to open the dialog. */
  kPMPhaseDialogsUp             = 2,    /* One of the dialogs is in front of the user. */
  kPMPhasePostDialogs           = 3,    /* Dialogs are down, not yet spooling. */
  kPMPhasePreAppDrawing         = 4,    /* Just before we begin to spool */
  kPMPhaseAppDrawing            = 5,    /* We're spooling drawing commands from the app. */
  kPMPhasePostAppDrawing        = 6,    /* Finished with spooling, not yet rendering or converting. */
  kPMPhasePreConversion         = 7,    /* Just before we begin to convert to PS, Raster, or other final format. */
  kPMPhaseConverting            = 8,    /* Converting from Spool file to final printer format. */
  kPMPhasePostConversion        = 9,    /* Done with printer ready data, waiting for completion. */
  kPMPhasePrinting              = 10    /* In the process of waiting for the printer. */
};

typedef SInt16                          PMTicketErrors;
/* BATCH PROCESSING ENUMS AND STRUCTURES  */
/* - Look in this section for help creating statically defined tickets. */
/* We have the ability to create a Ticket from an array of key/item pairs that define */
/* the keys and items for the entries in the ticket. To facilitate easy conversion of */
/* these key/item pairs, we've defined a few constants to help in the batch processing of */
/* these arrays. These are basically key types that are allowed for batch processing. */
typedef UInt16 PMTicketItemType;
enum {
  kPMItemInvalidType            = 0,
  kPMItemCStringType            = 1,    /* a C string pointed to by a char* */
  kPMItemSInt32Type             = 2,    /* a C signed 32 bit integer. */
  kPMItemBooleanType            = 3,    /* A Boolean */
  kPMItemCStrListType           = 4,    /* A list of c-strings. */
  kPMItemPMRectType             = 5,    /* A pointer to a PMRect */
  kPMItemSInt32ListType         = 6,    /* A pointer to a SInt32List */
  kPMItemPMRectListType         = 7     /* A pointer to a PMRectList */
};


/* A structure that holds a list of CString pointers, along with a definition for a constant list. */
struct CStrList {
  SInt32              count;                  /* Number of CString pointers in the array. */
  const char **       strArray;               /* Pointer to first CString pointer. */
};
typedef struct CStrList                 CStrList;
typedef const CStrList                  ConstCStrList;
/* A structure to hold a list of Signed 32 bit integers.  */
struct SInt32List {
  SInt32              count;                  /* Number of Signed 32 bit values in array. */
  const SInt32 *      sInt32Array;            /* Pointer to the first 32 bit value in array. */
};
typedef struct SInt32List               SInt32List;
typedef const SInt32List                ConstSInt32List;
/* A structure to hold a list of PMRect structures. PMRect is defined in PMDefinitions.h */
/* and is basically a set of four doubles (top, left, bottom, and right). */
struct PMRectList {
  SInt32              count;                  /* Number of PMRect pointers in the array. */
  const PMRect **     pmRectArray;            /* Pointer to the first PMRect pointer. */
};
typedef struct PMRectList               PMRectList;
typedef const PMRectList                ConstPMRectList;
/* CF types can't be statically initialized, but CString and Pascal strings can be. */
/* We have created utilities to convert from static C and Pascal based data into CF types. */
/* The structure below will serve as a statically defined "Ticket Item" that can then be */
/* converted into a job ticket item. An array of these structures would be made into a ticket. */
struct PMTicketItemStruct {
  char *              key;                    /* The CString for the key. */
  PMTicketItemType    itemType;               /* What type of item is being defined in union below. */
  Boolean             locked;                 /* Can other callers change this after it's entered in a ticket? */
  union {                                       /* The item data itself. */
    const void *        GenericData;          /* Generic data */
    const char *        cString;              /* kPMItemCStringType */
    SInt32              sInt32;               /* kPMItemSInt32Type */
    UInt32              boolean;              /* kPMItemBooleanType */
    ConstCStrList *     cStrlist;             /* kPMItemCStrListType */
    PMRect *            rect;                 /* kPMItemPMRectType */
    ConstSInt32List *   sInt32List;           /* kPMItemSInt32ListType */
    ConstPMRectList *   pmRectList;           /* kPMItemPMRectListType */
  }                       value;
};
typedef struct PMTicketItemStruct       PMTicketItemStruct;
/* END OF BATCH PROCESSING ENUMS AND STRUCTURES */
/* ENUMS for Ticket Values */
/* The values for kPMDuplexingKey */
enum {
  kPMDuplexNone                 = 1,
  kPMDuplexNoTumble             = 2,    /* Print on both sides of the paper, pages flip from left to right.*/
  kPMDuplexTumble               = 3,    /* Print on both sides of the paper, tumbling on so pages flip top to bottom.*/
  kPMSimplexTumble              = 4,    /* Print on only one side of the paper, but tumble the images while printing.*/
  kPMDuplexDefault              = kPMDuplexNone /* If the kPMDuplexingKey is not in a ticket then kPMDuplexDefault should be assumed.*/
};


/* Values for the kPMPSErrorHandlerKey */
enum {
  kPSNoErrorHandler             = 0,
  kPSErrorHandler               = 1
};

/* If the kPMCopyCollateKey is not present, then kPMCopyCollateDefault should be assumed. */
enum {
  kPMCopyCollateDefault         = true
};



/* END OF SECTION I: DATA TYPES, STUCTURES, and CONSTANTS */

/* SECTION II: TICKET KEYS */
/* - Look in this section for particular predefined keys for various tickets. */
/* Each key is defined to store it's data as a particular CF type, but there are some  */
/*  utilities defined to fetch data in different formats. For instance, it would be simple */
/*  to read in the Job Name as a CFString, or as a 'C' string. */
/* Ticket: PAPER INFO TICKET
    This ticket contains a single Paper Info set, with all the information necessary to 
    describe a paper type available from the Printer Module.
*/
#define kPMPaperInfoPrelude             "com.apple.print.PaperInfo."
/* CFString for the name of the paper displayed in UI */
#define kPMPaperNameStr                 "com.apple.print.PaperInfo.PMPaperName"
#define kPMPaperNameKey                 CFSTR("com.apple.print.PaperInfo.PMPaperName")
/* CFArray of 4 CFNumbers of kCFNumberDoubleType for paper size in points. */
#define kPMUnadjustedPaperRectStr       "com.apple.print.PaperInfo.PMUnadjustedPaperRect"
#define kPMUnadjustedPaperRectKey       CFSTR("com.apple.print.PaperInfo.PMUnadjustedPaperRect")
/* CFArray of 4 CFNumbers of kCFNumberDoubleType for page size within the paper, in points. */
#define kPMUnadjustedPageRectStr        "com.apple.print.PaperInfo.PMUnadjustedPageRect"
#define kPMUnadjustedPageRectKey        CFSTR("com.apple.print.PaperInfo.PMUnadjustedPageRect")
/* CFBoolean tells if we need to find a closest match for this paper */
#define kPMMatchPaperStr                "com.apple.print.PaperInfo.PMMatchPaper"
#define kPMMatchPaperKey                CFSTR("com.apple.print.PaperInfo.PMMatchPaper")

/* Ticket: PAGE FORMAT TICKET
    Describes the application's drawing environment, including resolution, scaling, and
    orientation. All information in the page format ticket is saved with the user's document,
    so the single paper info stored inside the page format ticket describes the logical page
    an application may draw to. This logical page is then mapped to a physical page defined
    in the Print Settings Ticket.
*/
#define kPMPageFormatPrelude            "com.apple.print.PageFormat."
/* CFArray of 4 CFNumbers of kCFNumberDoubleType for scaled and rotated page rectangle. */
#define kPMAdjustedPaperRectStr         "com.apple.print.PageFormat.PMAdjustedPaperRect"
#define kPMAdjustedPaperRectKey         CFSTR("com.apple.print.PageFormat.PMAdjustedPaperRect")
/* CFArray of 4 CFNumbers of kCFNumberDoubleType for scaled and rotated page rectangle, in points. */
#define kPMAdjustedPageRectStr          "com.apple.print.PageFormat.PMAdjustedPageRect"
#define kPMAdjustedPageRectKey          CFSTR("com.apple.print.PageFormat.PMAdjustedPageRect")
/* CFNumber - kCFNumberDoubleType, Drawing resolution in horizontal direction. */
#define kPMDrawingResHorizontalStr      "com.apple.print.PageFormat.PMHorizontalRes"
#define kPMDrawingResHorizontalKey      CFSTR("com.apple.print.PageFormat.PMHorizontalRes")
/* CFNumber - kCFNumberDoubleType, Drawing resolution in vertical direction. */
#define kPMDrawingResVerticalStr        "com.apple.print.PageFormat.PMVerticalRes"
#define kPMDrawingResVerticalKey        CFSTR("com.apple.print.PageFormat.PMVerticalRes")
/* CFNumber - kCFNumberDoubleType, Horizontal scaling factor applied to original page size - 1 = 100% */
#define kPMPageScalingHorizontalStr     "com.apple.print.PageFormat.PMScaling"
#define kPMPageScalingHorizontalKey     CFSTR("com.apple.print.PageFormat.PMScaling")
/* CFNumber - kCFNumberDoubleType, Vertical scaling factor applied to original page size - 1 = 100%. */
#define kPMPageScalingVerticalStr       "com.apple.print.PageFormat.PMVerticalScaling"
#define kPMPageScalingVerticalKey       CFSTR("com.apple.print.PageFormat.PMVerticalScaling")
/* CFNumber - kCFNumberSInt32Type, PMOrientation, 1 = portrait, 2 = landscape, 3 = reverse landscape, 4 = reverse portrait. */
#define kPMPageOrientationStr           "com.apple.print.PageFormat.PMOrientation"
#define kPMPageOrientationKey           CFSTR("com.apple.print.PageFormat.PMOrientation")
/* CFData - Print record handle, allowing easy access during most calls. Not used when flattened. (OS8 Only) */
#define kPMPageBackupRecordHdlStr       "com.apple.print.PageFormat.BackupPrintRecordHandle"
#define kPMPageBackupRecordHdlKey       CFSTR("com.apple.print.PageFormat.BackupPrintRecordHandle")
/* CFData - Print record stored in complete form - used when flattening Ticket w/ record. (OS8 only) */
#define kPMPageBackupRecordDataStr      "com.apple.print.PageFormat.BackupPrintRecord"
#define kPMPageBackupRecordDataKey      CFSTR("com.apple.print.PageFormat.BackupPrintRecord")
/* CFData - Handle to the print record using for custom dialog calls. Not stored when flattened. (OS8 Only) */
#define kPMPageCustomDialogHdlStr       "com.apple.print.PageFormat.CustomDialogRecord"
#define kPMPageCustomDialogHdlKey       CFSTR("com.apple.print.PageFormat.CustomDialogRecord")
/* CFString - name of the formating printer */
#define kPMFormattingPrinterStr         "com.apple.print.PageFormat.FormattingPrinter"
#define kPMFormattingPrinterKey         CFSTR("com.apple.print.PageFormat.FormattingPrinter")
/* Ticket: PRINT SETTINGS TICKET
    Contains the user's print dialog selections. Includes items set by the application to
    further refine the output produced on the printer. Information in this ticket may also
    be saved with the decument, but are likely to be overwritten by any saved settings. 
    Logical page information is mapped to the physical page defined in this ticket.
*/
#define kPMPrintSettingsPrelude         "com.apple.print.PrintSettings."
/* CFNumber, kCFNumberSInt32Type kPMDestinationPrinter kPMDestinationFile kPMDestinationFax. */
#define kPMDestinationTypeStr           "com.apple.print.PrintSettings.PMDestinationType"
#define kPMDestinationTypeKey           CFSTR("com.apple.print.PrintSettings.PMDestinationType")
/* CFString - URL for the output filename. */
#define kPMOutputFilenameStr            "com.apple.print.PrintSettings.PMOutputFilename"
#define kPMOutputFilenameKey            CFSTR("com.apple.print.PrintSettings.PMOutputFilename")
/* CFNumber, kCFNumberSInt32Type, number of copies to print. */
#define kPMCopiesStr                    "com.apple.print.PrintSettings.PMCopies"
#define kPMCopiesKey                    CFSTR("com.apple.print.PrintSettings.PMCopies")
/* CFBoolean, Turns on collating */
#define kPMCopyCollateStr               "com.apple.print.PrintSettings.PMCopyCollate"
#define kPMCopyCollateKey               CFSTR("com.apple.print.PrintSettings.PMCopyCollate")
/* CFBoolean, If true, we print sheets back to front. All layout options are unaffected by reverse order. */
#define kPMReverseOrderStr              "com.apple.print.PrintSettings.PMReverseOrder"
#define kPMReverseOrderKey              CFSTR("com.apple.print.PrintSettings.PMReverseOrder")
/* CFArray of kCFNumberSInt32Type, each pair indicating valid range of pages that the application is able to print. Does not signify user choices. */
#define kPMPageRangeStr                 "com.apple.print.PrintSettings.PMPageRange"
#define kPMPageRangeKey                 CFSTR("com.apple.print.PrintSettings.PMPageRange")
/* CFNumber - kCFNumberSInt32Type, first page selected by user to print. */
#define kPMFirstPageStr                 "com.apple.print.PrintSettings.PMFirstPage"
#define kPMFirstPageKey                 CFSTR("com.apple.print.PrintSettings.PMFirstPage")
/* CFNumber - kCFNumberSInt32Type, last page selected by user to print. */
#define kPMLastPageStr                  "com.apple.print.PrintSettings.PMLastPage"
#define kPMLastPageKey                  CFSTR("com.apple.print.PrintSettings.PMLastPage")
/* CFBoolean - If true, we do borders. */
#define kPMBorderStr                    "com.apple.print.PrintSettings.PMBorder"
#define kPMBorderKey                    CFSTR("com.apple.print.PrintSettings.PMBorder")
/* CFNumber - kCFNumberSInt32Type, Enum (PMBorderType) */
#define kPMBorderTypeStr                "com.apple.print.PrintSettings.PMBorderType"
#define kPMBorderTypeKey                CFSTR("com.apple.print.PrintSettings.PMBorderType")
/* CFBoolean, Turns on N-Up layout. */
#define kPMLayoutNUpStr                 "com.apple.print.PrintSettings.PMLayoutNUp"
#define kPMLayoutNUpKey                 CFSTR("com.apple.print.PrintSettings.PMLayoutNUp")
/* CFNumber - kCFNumberSInt32Type, indicates number of layout rows. */
#define kPMLayoutRowsStr                "com.apple.print.PrintSettings.PMLayoutRows"
#define kPMLayoutRowsKey                CFSTR("com.apple.print.PrintSettings.PMLayoutRows")
/* CFNumber - kCFNumberSInt32Type, indicates number of layout columns. */
#define kPMLayoutColumnsStr             "com.apple.print.PrintSettings.PMLayoutColumns"
#define kPMLayoutColumnsKey             CFSTR("com.apple.print.PrintSettings.PMLayoutColumns")
/* CFNumber - kCFNumberSInt32Type, Enum (PMLayoutDirection) */
#define kPMLayoutDirectionStr           "com.apple.print.PrintSettings.PMLayoutDirection"
#define kPMLayoutDirectionKey           CFSTR("com.apple.print.PrintSettings.PMLayoutDirection")
/* CFNumber - kCFNumberSInt32Type, PMOrientation, 1 = portrait, 2 = landscape, etc. */
#define kPMLayoutTileOrientationStr     "com.apple.print.PrintSettings.PMLayoutTileOrientation"
#define kPMLayoutTileOrientationKey     CFSTR("com.apple.print.PrintSettings.PMLayoutTileOrientation")
/* CFNumber - kCFNumberSInt32Type, Enum, draft, normal, best */
#define kPMQualityStr                   "com.apple.print.PrintSettings.PMQuality"
#define kPMQualityKey                   CFSTR("com.apple.print.PrintSettings.PMQuality")
/* CFNumber - kCFNumberSInt32Type, Enum, draft, normal, best */
#define kPMPaperTypeStr                 "com.apple.print.PrintSettings.PMPaperType"
#define kPMPaperTypeKey                 CFSTR("com.apple.print.PrintSettings.PMPaperType")
/* CFNumber - kCFNumberSInt32Type, Enum, active = 0, pending, hold until, hold indefinitely, aborted, finished */
#define kPMJobStateStr                  "com.apple.print.PrintSettings.PMJobState"
#define kPMJobStateKey                  CFSTR("com.apple.print.PrintSettings.PMJobState")
/* CFDate - Time we expect to print the job. */
#define kPMJobHoldUntilTimeStr          "com.apple.print.PrintSettings.PMJobHoldUntilTime"
#define kPMJobHoldUntilTimeKey          CFSTR("com.apple.print.PrintSettings.PMJobHoldUntilTime")
/* CFNumber - kCFNumberSInt32Type, Enum, Low = 0, normal, urgent */
#define kPMJobPriorityStr               "com.apple.print.PrintSettings.PMJobPriority"
#define kPMJobPriorityKey               CFSTR("com.apple.print.PrintSettings.PMJobPriority")
/* CFNUmber - kCFNumberSInt32Type, Enum, paper sources. */
#define kPMPaperSourceStr               "com.apple.print.PrintSettings.PMPaperSource"
#define kPMPaperSourceKey               CFSTR("com.apple.print.PrintSettings.PMPaperSource")
/* CFNumber - kCFNumberSInt32Type, Enum, kPMDuplexNone,  kPMDuplexNoTumble, kPMDuplexTumble, kPMSimplexTumble */
#define kPMDuplexingStr                 "com.apple.print.PrintSettings.PMDuplexing"
#define kPMDuplexingKey                 CFSTR("com.apple.print.PrintSettings.PMDuplexing")
/* CFNumber - kCFNumberSInt32Type, Enum, B/W, Grayscale, Color, HiFi Color. */
#define kPMColorModeStr                 "com.apple.print.PrintSettings.PMColorMode"
#define kPMColorModeKey                 CFSTR("com.apple.print.PrintSettings.PMColorMode")
/* CFNumber - kCFNumberSInt32Type, ID of profile to use. */
#define kPMColorSyncProfileIDStr        "com.apple.print.PrintSettings.PMColorSyncProfileID"
#define kPMColorSyncProfileIDKey        CFSTR("com.apple.print.PrintSettings.PMColorSyncProfileID")
/* CFString - path of system profile. */
#define kPMColorSyncSystemProfilePathStr  "com.apple.print.PrintSettings.PMColorSyncSystemProfilePath"
#define kPMColorSyncSystemProfilePathKey  CFSTR("com.apple.print.PrintSettings.PMColorSyncSystemProfilePath")
/* CFNumber - kCFNumberDoubleType, Horizontal scaling factor applied to original page size. */
#define kPMPrintScalingHorizontalStr    "com.apple.print.PrintSettings.PMScaling"
#define kPMPrintScalingHorizontalKey    CFSTR("com.apple.print.PrintSettings.PMScaling")
/* CFNumber - kCFNumberDoubleType, Vertical scaling factor applied to original page size. */
#define kPMPrintScalingVerticalStr      "com.apple.print.PrintSettings.PMVerticalScaling"
#define kPMPrintScalingVerticalKey      CFSTR("com.apple.print.PrintSettings.PMVerticalScaling")
/* CFNumber - kCFNumberSInt32Type, Enum (PMScalingAlignment) */
#define kPMPrintScalingAlignmentStr     "com.apple.print.PrintSettings.PMScalingAlignment"
#define kPMPrintScalingAlignmentKey     CFSTR("com.apple.print.PrintSettings.PMScalingAlignment")
/* CFNumber - kCFNumberSInt32Type, PMOrientation, 1 = portrait, 2 = landscape, etc. */
#define kPMPrintOrientationStr          "com.apple.print.PrintSettings.PMOrientation"
#define kPMPrintOrientationKey          CFSTR("com.apple.print.PrintSettings.PMOrientation")
/* CFString - YES means the user clicked on the Preview button */
#define kPMPreviewStr                   "com.apple.print.PrintSettings.PMPreview"
#define kPMPreviewKey                   CFSTR("com.apple.print.PrintSettings.PMPreview")
/* CFData - Print record handle, allowing easy access during most calls. Not used when flattened. (OS8 Only) */
#define kPMPrintBackupRecordHdlStr      "com.apple.print.PrintSettings.BackupPrintRecordHandle"
#define kPMPrintBackupRecordHdlKey      CFSTR("com.apple.print.PrintSettings.BackupPrintRecordHandle")
/* CFData - Print record stored in complete form - used when flattening Ticket w/ record. (OS8 only) */
#define kPMPrintBackupRecordDataStr     "com.apple.print.PrintSettings.BackupPrintRecord"
#define kPMPrintBackupRecordDataKey     CFSTR("com.apple.print.PrintSettings.BackupPrintRecord")
/* CFData - Handle to the print record using for custom dialog calls. Not stored when flattened. (OS8 Only) */
#define kPMPrintCustomDialogHdlStr      "com.apple.print.PrintSettings.CustomDialogRecord"
#define kPMPrintCustomDialogHdlKey      CFSTR("com.apple.print.PrintSettings.CustomDialogRecord")
/* CFArray - main & option PPD key for input paper feed */
#define kPMPrimaryPaperFeedStr          "com.apple.print.PrintSettings.PMPrimaryPaperFeed"
#define kPMPrimaryPaperFeedKey          CFSTR("com.apple.print.PrintSettings.PMPrimaryPaperFeed")
/* CFArray - main & option PPD key for input paper feed */
#define kPMSecondaryPaperFeedStr        "com.apple.print.PrintSettings.PMSecondaryPaperFeed"
#define kPMSecondaryPaperFeedKey        CFSTR("com.apple.print.PrintSettings.PMSecondaryPaperFeed")
/* CFNumber - kCFNumberSInt32Type  */
#define kPMPSErrorHandlerStr            "com.apple.print.PrintSettings.PMPSErrorHandler"
#define kPMPSErrorHandlerKey            CFSTR("com.apple.print.PrintSettings.PMPSErrorHandler")
/* CFBoolean, Turns on PS error on screen notification.  */
#define kPMPSErrorOnScreenStr           "com.apple.print.PrintSettings.PMPSErrorOnScreen"
#define kPMPSErrorOnScreenKey           CFSTR("com.apple.print.PrintSettings.PMPSErrorOnScreen")
/* CFArray - main & option PPD key for tray switching */
#define kPMPSTraySwitchStr              "com.apple.print.PrintSettings.PMPSTraySwitch"
#define kPMPSTraySwitchKey              CFSTR("com.apple.print.PrintSettings.PMPSTraySwitch")
/* CFDictionary - main & option PPD keys for additional features */
#define kPMPPDDictStr                   "com.apple.print.PrintSettings.kPMPPDDictStr"
#define kPMPPDDictKey                   CFSTR("com.apple.print.PrintSettings.kPMPPDDictStr")
/* Ticket: PAGE TICKET
    Future Feature. Intended to hold Page Format and Print Settings ticket for a single
    page of the document. Not yet implemented.
*/
#define kPMPageTicketPrelude            "com.apple.print.PageTicket."
/* Ticket: DOCUMENT TICKET
    Future Feature. Intended to hold multiple Page Tickets and separate Page Format
    and Print Settings tickets for a single document. Not yet implemented. For now,
    the keys listed here will be included at the Job Ticket Level.
*/
#define kPMDocumentTicketPrelude        "com.apple.print.DocumentTicket."
/* CFString. Indicates format of data stored in the spool file. */
#define kPMSpoolFormatStr               "com.apple.print.DocumentTicket.PMSpoolFormat"
#define kPMSpoolFormatKey               CFSTR("com.apple.print.DocumentTicket.PMSpoolFormat")
/* CFString. Indicates format of data going to the Printer Module. */
#define kPMPrinterModuleFormatStr       "com.apple.print.DocumentTicket.PMDocPMInputFormat"
#define kPMPrinterModuleFormatKey       CFSTR("com.apple.print.DocumentTicket.PMDocPMInputFormat")
/* Ticket: JOB TICKET
    Holds a Page Format and Print Settings ticket, as well as a few other fields that 
    are Job Specific, such as information about the user that submitted the job. The
    fidelity bit controls what happens when the system encounters settings that can't
    be obeyed.
    In the future a Job Ticket may hold multiple document tickets in addition to the 
    Print Settings and Page Format tickets.
*/
#define kPMJobTicketPrelude             "com.apple.print.JobInfo."
/* CFString, The name of the job to be displayed in the queue window. */
#define kPMJobNameStr                   "com.apple.print.JobInfo.PMJobName"
#define kPMJobNameKey                   CFSTR("com.apple.print.JobInfo.PMJobName")
/* CFString, The application's name */
#define kPMApplicationNameStr           "com.apple.print.JobInfo.PMApplicationName"
#define kPMApplicationNameKey           CFSTR("com.apple.print.JobInfo.PMApplicationName")
/* CFNumber - kCFNumberSInt32Type, TBD how this is interpreted. */
#define kPMUserLanguageStr              "com.apple.print.JobInfo.PMUserLanguage"
#define kPMUserLanguageKey              CFSTR("com.apple.print.JobInfo.PMUserLanguage")
/* CFString - Name of the user who submitted the job. */
#define kPMJobOwnerStr                  "com.apple.print.JobInfo.PMJobOwner"
#define kPMJobOwnerKey                  CFSTR("com.apple.print.JobInfo.PMJobOwner")
/* CFDictionary - PMTemplateRef, actually. See PMTemplate.h */
#define kPMJobTemplateStr               "com.apple.print.JobInfo.PMJobTemplate"
#define kPMJobTemplateKey               CFSTR("com.apple.print.JobInfo.PMJobTemplate")
/* CFNumber, kCFNumberSInt32Type, Enum - Spooling, RIPing, etc. */
#define kPMPhaseStr                     "com.apple.print.JobInfo.PMPrintingPhase"
#define kPMPhaseKey                     CFSTR("com.apple.print.JobInfo.PMPrintingPhase")
/* CFString, Mime type from the kPMOutputTypeListKey array the printer module should generate. */
#define kPMOutputTypeStr                "com.apple.print.JobInfo.PMOutputType"
#define kPMOutputTypeKey                CFSTR("com.apple.print.JobInfo.PMOutputType")
/* Ticket: LIST TICKET
    Contains a list of other tickets. There is no restriction on the type of tickets
    that can be stored in a list, any ticket will do. 
*/
#define kPMTicketListPrelude            "com.apple.print.TicketList."


/* Ticket: Printer Info Ticket
    Created by the Printer Module to describe features and settings of the current
    printer. This is information largely used by the Printer Module itself and the Print
    Dialog Extensions associated with the PM. All "range" type values that would be 
    verified at dialog time should go in a separate Template.
*/
#define kPMPrinterInfoPrelude           "com.apple.print.PrinterInfo."
/* CFString, Full name of the printer. */
#define kPMPrinterLongNameStr           "com.apple.print.PrinterInfo.PMPrinterLongName"
#define kPMPrinterLongNameKey           CFSTR("com.apple.print.PrinterInfo.PMPrinterLongName")
/* CFString, Shorter name to use when you have less space. */
#define kPMPrinterShortNameStr          "com.apple.print.PrinterInfo.PMPrinterShortName"
#define kPMPrinterShortNameKey          CFSTR("com.apple.print.PrinterInfo.PMPrinterShortName")
/* CFString, Product name is used for ???? */
#define kPMMakeAndModelNameStr          "com.apple.print.PrinterInfo.PMMakeAndModelName"
#define kPMMakeAndModelNameKey          CFSTR("com.apple.print.PrinterInfo.PMMakeAndModelName")
/* CFData, Product address.  */
#define kPMPrinterAddressStr            "com.apple.print.PrinterInfo.PMPrinterAddress"
#define kPMPrinterAddressKey            CFSTR("com.apple.print.PrinterInfo.PMPrinterAddress")
/* CFBoolean, If Non-zero, printer/PM can do color. */
#define kPMSupportsColorStr             "com.apple.print.PrinterInfo.PMSupportsColor"
#define kPMSupportsColorKey             CFSTR("com.apple.print.PrinterInfo.PMSupportsColor")
/* CFBoolean, If Non-zero, printer/PM can do copies */
#define kPMDoesCopiesStr                "com.apple.print.PrinterInfo.PMDoesCopies"
#define kPMDoesCopiesKey                CFSTR("com.apple.print.PrinterInfo.PMDoesCopies")
/* CFBoolean, If Non-zero, printer/PM can collate. */
#define kPMDoesCopyCollateStr           "com.apple.print.PrinterInfo.PMDoesCopyCollate"
#define kPMDoesCopyCollateKey           CFSTR("com.apple.print.PrinterInfo.PMDoesCopyCollate")
/* CFBoolean, If Non-zero, printer/PM can reverse the printing order. */
#define kPMDoesReverseOrderStr          "com.apple.print.PrinterInfo.PMDoesReverseOrderK"
#define kPMDoesReverseOrderKey          CFSTR("com.apple.print.PrinterInfo.PMDoesReverseOrderK")
/* CFArray of CFStrings indicating file types. See PMDefinitions.h for complete list. */
#define kPMInputFileTypeListStr         "com.apple.print.PrinterInfo.PMInputFileTypeList"
#define kPMInputFileTypeListKey         CFSTR("com.apple.print.PrinterInfo.PMInputFileTypeList")
/* CFArray of CFStrings indicating the MIME type for the data is can send to an IO module. */
#define kPMOutputTypeListStr            "com.apple.print.PrinterInfo.PMOutputTypeList"
#define kPMOutputTypeListKey            CFSTR("com.apple.print.PrinterInfo.PMOutputTypeList")
/* Postscript printing related tags */
/* CFNumber - kCFNumberSInt32Type, indicates PostScript Language level. Contains one of the values from the enum below */
#define kPMPostScriptLevelStr           "com.apple.print.PrinterInfo.PMPostScriptLevel"
#define kPMPostScriptLevelKey           CFSTR("com.apple.print.PrinterInfo.PMPostScriptLevel")
/* CFString e.g."2040.113"*/
#define kPMPostScriptVersionStr         "com.apple.print.PrinterInfo.PMPSVersion"
#define kPMPostScriptVersionKey         CFSTR("com.apple.print.PrinterInfo.PMPSVersion")
/* CFNumber - kCFNumberSInt32Type, contains the PostScript revision which is an integer.*/
#define kPMPostScriptRevisionStr        "com.apple.print.PrinterInfo.PMPSRevision"
#define kPMPostScriptRevisionKey        CFSTR("com.apple.print.PrinterInfo.PMPSRevision")
/* CFNumber - kCFNumberSInt32Type, contains one of the values from the enum below.*/
#define kPMPostScriptTrueTypeFontRasterizerStr  "com.apple.print.PrinterInfo.PMPSTTRasterizer"
#define kPMPostScriptTrueTypeFontRasterizerKey  CFSTR("com.apple.print.PrinterInfo.PMPSTTRasterizer")

/* Enum for the possible font rasterizers. */
enum {
  kPMPSTTRasterizerUnknown      = 0,    /*  unknown*/
  kPMPSTTRasterizerNone         = 1,    /*  none*/
  kPMPSTTRasterizerAccept68K    = 2,    /*  accept 68k*/
  kPMPSTTRasterizerType42       = 3     /*  type 42*/
};

/* Enum for possible PostScript Language Levels. */
enum {
  kPMPSTargetLanguageLevel2and3 = -3,   /* Level 2 Compatible, may take advantage of Level 3 features*/
  kPMPSTargetLanguageLevel1and2 = -2,   /* Level 1 Compatible, may take advantage of Level 2 and 3 features*/
  kPMPSTargetLanguageLevelUnknown = -1, /* language level of target is unknown*/
  kPMPSTargetLanguageLevel1     = 1,    /* level 1*/
  kPMPSTargetLanguageLevel2     = 2,    /* level 2*/
  kPMPSTargetLanguageLevel3     = 3,    /* level 3*/
  kPMPSTargetLanguageLevelDefault = kPMPSTargetLanguageLevelUnknown /* default*/
};


/* Note, we've removed the PMLanguageInfoKey because it's not valid to store strings or structures, everything needs to be a CFType.*/
/* CFNumber - kCFNumberSInt32Type, Memory on the Printer itself. */
#define kPMTotalMemInstalledStr         "com.apple.print.PrinterInfo.PMTotalMemInstalled"
#define kPMTotalMemInstalledKey         CFSTR("com.apple.print.PrinterInfo.PMTotalMemInstalled")
/* CFNumber - kCFNumberSInt32Type, Remaining memory available for use. */
#define kPMTotalMemAvailableStr         "com.apple.print.PrinterInfo.PMTotalMemAvailable"
#define kPMTotalMemAvailableKey         CFSTR("com.apple.print.PrinterInfo.PMTotalMemAvailable")
/* PPD related tags */
/* CFString, PPD file name or other description file. */
#define kPMDescriptionFileStr           "com.apple.print.PrinterInfo.PMDescriptionFile"
#define kPMDescriptionFileKey           CFSTR("com.apple.print.PrinterInfo.PMDescriptionFile")
/* CFData, Compiled PPD file to ease parsing after the fact. Not sure we need. */
#define kPMCompiledPPDStr               "com.apple.print.PrinterInfo.PMCompiledPPD"
#define kPMCompiledPPDKey               CFSTR("com.apple.print.PrinterInfo.PMCompiledPPD")
/* ColorSync related tags */
/* CFString, representing a CFUUID - must be unique per device */
#define kPMColorDeviceIDStr             "com.apple.print.PrinterInfo.PMColorDeviceID"
#define kPMColorDeviceIDKey             CFSTR("com.apple.print.PrinterInfo.PMColorDeviceID")
/* CFArray - of CFDicts, one for each factory profile. */
#define kPMColorSyncProfilesStr         "com.apple.print.PrinterInfo.PMColorSyncProfiles"
#define kPMColorSyncProfilesKey         CFSTR("com.apple.print.PrinterInfo.PMColorSyncProfiles")
/* Installable options string. */
/* CFData, Installable options from PPD file. */
#define kPMInstallableOptionStr         "com.apple.print.PrinterInfo.PMInstallableOption"
#define kPMInstallableOptionKey         CFSTR("com.apple.print.PrinterInfo.PMInstallableOption")
/* Suggested app drawing resolutions */
/* CFArray -  array of pairs of kCFNumberDoubleType - One pair for each recommended resolution*/
#define kPMPrinterSuggestedResStr       "com.apple.print.PrinterInfo.PMPrinterSuggestedRes"
#define kPMPrinterSuggestedResKey       CFSTR("com.apple.print.PrinterInfo.PMPrinterSuggestedRes")
/* Min and max range of resolutions */
/* CFArray - 2 CFNumber, kCFNumberDoubleType, min resolution of printer */
#define kPMPrinterMinResStr             "com.apple.print.PrinterInfo.PMPrinterMinRes"
#define kPMPrinterMinResKey             CFSTR("com.apple.print.PrinterInfo.PMPrinterMinRes")
/* CFArray - 2 CFNumber, kCFNumberDoubleType, max resolution of printer */
#define kPMPrinterMaxResStr             "com.apple.print.PrinterInfo.PMPrinterMaxRes"
#define kPMPrinterMaxResKey             CFSTR("com.apple.print.PrinterInfo.PMPrinterMaxRes")
/* driver creator code */
/* CFNumber - kCFNumberSInt32Type creator signiture  for  driver*/
#define kPMDriverCreatorStr             "com.apple.print.PrinterInfo.PMDriverCreator"
#define kPMDriverCreatorKey             CFSTR("com.apple.print.PrinterInfo.PMDriverCreator")
/* Printer Resident font data handle */
/* CFData, Printer resident fonts from printer query. */
#define kPMPrinterFontStr               "com.apple.print.PrinterInfo.Printer Fonts"
#define kPMPrinterFontKey               CFSTR("com.apple.print.PrinterInfo.Printer Fonts")
/* CFBoolean - PJCIsPostScriptDriver*/
#define kPMPrinterIsPostScriptDriverStr  "com.apple.print.PrinterInfo.PMIsPostScriptDriver"
#define kPMPrinterIsPostScriptDriverKey  CFSTR("com.apple.print.PrinterInfo.PMIsPostScriptDriver")
#define kPMIsBinaryOKStr                "com.apple.print.PrinterInfo.PMIsBinaryOK"
#define kPMIsBinaryOKKey                CFSTR("com.apple.print.PrinterInfo.PMIsBinaryOK")
/* CFBoolean - Can the comm channel take bytes with the high bit set?*/
#define kPM8BitCommStr                  "com.apple.print.PrinterInfo.PM8BitComm"
#define kPM8BitCommKey                  CFSTR("com.apple.print.PrinterInfo.PM8BitComm")
#define kPMTransparentCommStr           "com.apple.print.PrinterInfo.PMTransparentComm"
#define kPMTransparentCommKey           CFSTR("com.apple.print.PrinterInfo.PMTransparentComm")
/* Ticket: Converter Setup Ticket
    Contains controls for converter. Generally, the Printer Module and Job Manager will set
    tags in this ticket to control the various settings of the conversion process. 
*/
#define kPMConverterSetupPrelude        "com.apple.print.ConverterSetup."
/* CFBoolean, turns on banding if it's available. */
#define kPMBandingRequestedStr          "com.apple.print.ConverterSetup.PMBandingRequested"
#define kPMBandingRequestedKey          CFSTR("com.apple.print.ConverterSetup.PMBandingRequested")
/* CFNumber, number of scan lines needed for each band. Could be whole page, in which case banding is disabled. */
#define kPMRequiredBandHeightStr        "com.apple.print.ConverterSetup.PMRequiredBandHeight"
#define kPMRequiredBandHeightKey        CFSTR("com.apple.print.ConverterSetup.PMRequiredBandHeight")
/* CFBoolean. If true, the printer module wants converter to switch between b/w bands and color bands when possible. */
#define kPMDepthSwitchingEnabledStr     "com.apple.print.ConverterSetup.PMDepthSwitching"
#define kPMDepthSwitchingEnabledKey     CFSTR("com.apple.print.ConverterSetup.PMDepthSwitching")
/* CFBoolean. If true, the printer module wants converter to skip over white space if possible. */
#define kPMWhiteSkippingEnabledStr      "com.apple.print.ConverterSetup.PMWhiteSpaceSkipping"
#define kPMWhiteSkippingEnabledKey      CFSTR("com.apple.print.ConverterSetup.PMWhiteSpaceSkipping")
/* CFNumber, CFNumberDoubleType indicating horizontal final rendering resolution. */
#define kPMConverterResHorizontalStr    "com.apple.print.ConverterSetup.PMConversionResHorizontal"
#define kPMConverterResHorizontalKey    CFSTR("com.apple.print.ConverterSetup.PMConversionResHorizontal")
/* CFNumber, CFNumberDoubleType indicating vertical final rendering resolution. */
#define kPMConverterResVerticalStr      "com.apple.print.ConverterSetup.PMConversionResVertcial"
#define kPMConverterResVerticalKey      CFSTR("com.apple.print.ConverterSetup.PMConversionResVertcial")
/* CFNumber, CFNumberLongType, indicates the pixel format requested of the converter. */
#define kPMRequestedPixelFormatStr      "com.apple.print.ConverterSetup.PMPixelFormat"
#define kPMRequestedPixelFormatKey      CFSTR("com.apple.print.ConverterSetup.PMPixelFormat")
/* CFNumber, CFNumberLongType, indicates the pixel layout requested of the converter. */
#define kPMRequestedPixelLayoutStr      "com.apple.print.ConverterSetup.PMPixelLayout"
#define kPMRequestedPixelLayoutKey      CFSTR("com.apple.print.ConverterSetup.PMPixelLayout")
/* CFNumber, kCFNumberSInt32Type, the profile ID for the profile to be used with this job. */
#define kPMCVColorSyncProfileIDStr      "com.apple.print.ConverterSetup.PMProfileID"
#define kPMCVColorSyncProfileIDKey      CFSTR("com.apple.print.ConverterSetup.PMProfileID")

/* END OF SECTION II: TICKET KEYS */
/* SECTION III, FUNCTIONS: */
/* Create, or copy a Ticket. As a Ticket is created, its type is stored in the object itself */
/* and used to help the programmer make sure they are adding the correct fields to the object. */
/*
 *  PMTicketCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketCreate(
  CFAllocatorRef   allocator,
  CFStringRef      ticketType,
  PMTicketRef *    newTicket);


/*
 *  PMTicketCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketCopy(
  CFAllocatorRef   allocator,
  PMTicketRef      sourceTicket,
  PMTicketRef *    destinationTicket);


/* Retain, release, or get retain count of a Ticket. The behavior is modelled after */
/* CoreFoundation's CFRetain, CFRelease, and CFGetRetainCount APIs. */
/*
 *  PMTicketGetRetainCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetRetainCount(
  PMTicketRef   ticket,
  CFIndex *     retainCount);


/*
 *  PMTicketRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketRetain(PMTicketRef ticket);


/*
 *  PMTicketRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketRelease(PMTicketRef ticket);



/*
 *  PMTicketReleaseAndClear()
 *  
 *  Discussion:
 *    PMTicketReleaseAndClear does the same thing as PMTicketRelease
 *    except that it sets the passed in pointer to NULL, which we think
 *    is good programming practice. Other references to the ticket may
 *    still be valid, but this one is no longer expected to be used.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketReleaseAndClear(PMTicketRef * ticket);




/*
 *  PMTicketGetLockedState()
 *  
 *  Discussion:
 *    Tickets may be locked by the printing system, so we provide a
 *    function to determine if the ticket is indeed locked. Any
 *    attempted changes to a locked ticket will result in
 *    "kPMTicketLocked" error.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetLockedState(
  PMTicketRef   ticket,
  Boolean *     lockedState);



/*
 *  PMTicketConfirmTicket()
 *  
 *  Discussion:
 *    To confirm that we have a real ticket, this short function gets
 *    called.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketConfirmTicket(PMTicketRef ticket);



/*
 *  PMTicketValidate()
 *  
 *  Discussion:
 *    We validate an entire ticket by calling our PMTicketValidate
 *    function, passing a template which contains constraints for all
 *    the values in the ticket. Those entries in the ticket that don't
 *    have constraints will not be checked, so developers can add their
 *    own custom data. For now we have to verify a single ticket at a
 *    time. Future updates will allow the caller to confirm an entire
 *    tree of tickets.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketValidate(
  PMTicketRef     ticket,
  PMTemplateRef   verifyingTemplate,
  CFArrayRef *    invalidItems);



/* A couple of utility functions to find out what type of ticket is references, as well */
/* as access to the allocator function passed in for a ticket. We also provide a function */
/* to check the API version for a ticket, to protect against future conversion problems */
/* from old to new tickets. */
/*
 *  PMTicketGetType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetType(
  PMTicketRef    ticket,
  CFStringRef *  ticketType);


/*
 *  PMTicketGetEnumType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetEnumType(
  PMTicketRef     ticket,
  PMTicketType *  ticketType);


/*
 *  PMTicketGetAllocator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetAllocator(
  PMTicketRef       ticket,
  CFAllocatorRef *  allocator);


/*
 *  PMTicketGetAPIVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetAPIVersion(
  PMTicketRef    ticket,
  CFStringRef *  apiVersion);



/* To store a Ticket object it needs to be converted to XML, a serialization that */
/* creates human readable text. If the data is to be written to file anyway, a second */
/* function is provided to go "all the way." */
/*
 *  PMTicketToXML()
 *  
 *  Discussion:
 *    Convert to XML
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketToXML(
  PMTicketRef   ticket,
  CFDataRef *   anXMLTicket);


/*
 *  PMTicketWriteXMLToFile()
 *  
 *  Discussion:
 *    Convert to XML and write to file
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketWriteXMLToFile(
  PMTicketRef   ticket,
  const char *  path);


/*
 *  PMTicketWriteXML()
 *  
 *  Discussion:
 *    Convert to XML and write to FILE stream
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketWriteXML(
  PMTicketRef   ticket,
  FILE *        xmlFile);


/*
 *  PMXMLToTicket()
 *  
 *  Discussion:
 *    Convert from XML
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMXMLToTicket(
  CFAllocatorRef   allocator,
  CFDataRef        anXMLTicket,
  PMTicketRef *    ticket,
  CFStringRef *    conversionError);


/*
 *  PMTicketReadXMLFromFile()
 *  
 *  Discussion:
 *    Read from XML file
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketReadXMLFromFile(
  CFAllocatorRef   allocator,
  const char *     path,
  PMTicketRef *    ticket,
  CFStringRef *    errorString);



/* To add an item to a ticket, the "SetItem" call for the specific data type is used whenever  */
/* possible. This adds a new item if one doesn't exist and updates a previous item if it  */
/* already exists. The "locked" field will determine if subsequent updates are allowed. */
/* Strings: */
/* CFString */
/*
 *  PMTicketSetCFString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFString(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  CFStringRef   item,
  Boolean       locked);


/* Pascal String */
/*
 *  PMTicketSetPString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetPString(
  PMTicketRef      ticket,
  CFStringRef      clientID,
  CFStringRef      key,
  ConstStringPtr   value,
  Boolean          locked);


/* 'C' String */
/*
 *  PMTicketSetCString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCString(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  const char *  value,
  Boolean       locked);


/* Array of 'C' Strings */
/*
 *  PMTicketSetCStringArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCStringArray(
  PMTicketRef    ticket,
  CFStringRef    clientID,
  CFStringRef    key,
  const char **  cStringArray,
  UInt32         count,
  Boolean        locked);



/* Numbers: */
/* CFNumber */
/*
 *  PMTicketSetCFNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFNumber(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  CFNumberRef   item,
  Boolean       locked);


/* SInt32 */
/*
 *  PMTicketSetSInt32()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetSInt32(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  SInt32        value,
  Boolean       locked);


/* Array of SInt32 */
/*
 *  PMTicketSetSInt32Array()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetSInt32Array(
  PMTicketRef     ticket,
  CFStringRef     clientID,
  CFStringRef     key,
  const SInt32 *  sInt32Array,
  UInt32          count,
  Boolean         locked);


/* UInt32 */
/* NOTE: The underlying code uses SInt32s in the CF code, so this function can't set -1 */
/* as the largest unsigned number. Please avoid setting the high bit */
/*
 *  PMTicketSetUInt32()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetUInt32(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  UInt32        value,
  Boolean       locked);


/* Array of UInt32 */
/* NOTE: The underlying code uses SInt32s in the CF code, so this function can't set -1 */
/* as the largest unsigned number. Please avoid setting the high bit */
/*
 *  PMTicketSetUInt32Array()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetUInt32Array(
  PMTicketRef     ticket,
  CFStringRef     clientID,
  CFStringRef     key,
  const UInt32 *  uInt32Array,
  UInt32          count,
  Boolean         locked);


/* Double */
/*
 *  PMTicketSetDouble()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetDouble(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  double        value,
  Boolean       locked);


/* Array of doubles */
/*
 *  PMTicketSetDoubleArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetDoubleArray(
  PMTicketRef     ticket,
  CFStringRef     clientID,
  CFStringRef     key,
  const double *  doubleArray,
  UInt32          count,
  Boolean         changeable);


/* PMResolution - Array of 2 Doubles */
/*
 *  PMTicketSetPMResolution()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetPMResolution(
  PMTicketRef     ticket,
  CFStringRef     clientID,
  CFStringRef     key,
  PMResolution *  value,
  Boolean         locked);


/* Array of PMResolution */
/*
 *  PMTicketSetPMResolutionArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetPMResolutionArray(
  PMTicketRef     ticket,
  CFStringRef     clientID,
  CFStringRef     key,
  PMResolution *  pmResolutionArray,
  UInt32          count,
  Boolean         locked);


/* PMRect - Array of 4 Doubles */
/*
 *  PMTicketSetPMRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetPMRect(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  PMRect *      value,
  Boolean       locked);


/* Array of PMRects */
/*
 *  PMTicketSetPMRectArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetPMRectArray(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  PMRect *      pmRectArray,
  UInt32        count,
  Boolean       locked);


/* Raw Data */
/* CFData */
/*
 *  PMTicketSetCFData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFData(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  CFDataRef     item,
  Boolean       locked);


/* UInt8 - 'C' Bytes */
/*
 *  PMTicketSetBytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetBytes(
  PMTicketRef    ticket,
  CFStringRef    clientID,
  CFStringRef    key,
  const UInt8 *  data,
  UInt32         size,
  Boolean        locked);


/* Booleans */
/* CFBoolean */
/*
 *  PMTicketSetCFBoolean()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFBoolean(
  PMTicketRef    ticket,
  CFStringRef    clientID,
  CFStringRef    key,
  CFBooleanRef   item,
  Boolean        locked);


/* 'C' Boolean */
/*
 *  PMTicketSetBoolean()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetBoolean(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  Boolean       value,
  Boolean       locked);



/* Dates */
/*
 *  PMTicketSetCFDate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFDate(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  CFDateRef     item,
  Boolean       locked);


/* Arrays */
/*
 *  PMTicketSetCFArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFArray(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  CFArrayRef    item,
  Boolean       locked);


/* Dictionaries */
/*
 *  PMTicketSetCFDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetCFDictionary(
  PMTicketRef       ticket,
  CFStringRef       clientID,
  CFStringRef       key,
  CFDictionaryRef   item,
  Boolean           locked);


/* Templates, which are defined to be opaque. */
/*
 *  PMTicketSetTemplate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetTemplate(
  PMTicketRef     ticket,
  CFStringRef     clientID,
  CFStringRef     key,
  PMTemplateRef   item,
  Boolean         locked);


/* Generic CF Type */
/*
 *  PMTicketSetItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetItem(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key,
  CFTypeRef     item,
  Boolean       locked);



/*
 *  PMTicketSetMetaItem()
 *  
 *  Discussion:
 *    To add an item that doesn't need to be stored in the XML file,
 *    use the PMTicketSetMetaItem() call. It allows simple addition of
 *    an item, but that item won't be written to XML when the rest of
 *    the ticket is stored. By definition, all meta items are not
 *    locked, simply because we have no easy mechanism for keeping
 *    track of a locked state for a meta item. Our intent is to allow a
 *    caller to add items temporarily to a ticket, perhaps to keep
 *    track of the "default" ticket in a list of tickets, or the
 *    "current" paper info in a list of papers.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetMetaItem(
  PMTicketRef   ticket,
  CFStringRef   key,
  CFTypeRef     item);



/*
 *  PMTicketFillFromArray()
 *  
 *  Discussion:
 *    Convert an array of static item structures into entries for a
 *    ticket and add them one at a time to the ticket. Could fail part
 *    way through.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketFillFromArray(
  PMTicketRef                 ticket,
  CFStringRef                 clientID,
  const PMTicketItemStruct *  items,
  UInt32                      itemCount);



/*
 *  PMTicketCopyItem()
 *  
 *  Discussion:
 *    If an item must be copied from one ticket to another, use the
 *    "CopyItem" call to make the simple transfer. This will update the
 *    recorded date and client ID for the item. The "locked" field will
 *    determine if subsequent updates may be made.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketCopyItem(
  PMTicketRef   sourceTicket,
  PMTicketRef   destTicket,
  CFStringRef   clientID,
  CFStringRef   key,
  Boolean       locked);



/* A couple of functions that will help in managing the "locked" state of any ticket */
/* item. These functions only work for items directly stored in the current ticket, not */
/* for items in sub-tickets. */
/*
 *  PMTicketIsItemLocked()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketIsItemLocked(
  PMTicketRef   ticket,
  CFStringRef   key,
  Boolean *     locked);


/*
 *  PMTicketLockItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketLockItem(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key);


/*
 *  PMTicketUnlockItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketUnlockItem(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key);



/*
 *  PMTicketDeleteItem()
 *  
 *  Discussion:
 *    Disable an item using PMTicketDelete. Future callers will see
 *    "ItemNotFound" errors. This call actually tags a new set of
 *    structures to the end of an item's dictionary, in order to keep a
 *    history of the item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketDeleteItem(
  PMTicketRef   ticket,
  CFStringRef   clientID,
  CFStringRef   key);


/*
 *  PMTicketReleaseItem()
 *  
 *  Discussion:
 *    Release an item (quite different from delete) when you want it
 *    completely removed from the ticket. This can only be done if the
 *    item is unlocked.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketReleaseItem(
  PMTicketRef   ticket,
  CFStringRef   key);


/*
 *  PMTicketContainsItem()
 *  
 *  Discussion:
 *    Fetching an item back out of the ticket requires the "GetItem"
 *    call for the particular type of data you're fetching. In some
 *    cases, the caller doesn't know what type of data is being
 *    returned, so we have a generic "GetItem" call, but for all other
 *    cases developers are encouraged to use the specific call for the
 *    data they need, thus enabling type checking and reducing errors.
 *    Returns true if item exists.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
PMTicketContainsItem(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key);


/* Strings */
/* CFString */
/*
 *  PMTicketGetCFString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFString(
  PMTicketRef    ticket,
  UInt32         nodeIndex1,
  UInt32         nodeIndex2,
  CFStringRef    key,
  CFStringRef *  item);


/* Pascal String */
/*
 *  PMTicketGetPString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetPString(
  PMTicketRef        ticket,
  UInt32             nodeIndex1,
  UInt32             nodeIndex2,
  CFStringRef        key,
  UInt32             bufferSize,
  CFStringEncoding   encoding,
  StringPtr          value);


/* 'C' String */
/*
 *  PMTicketGetCString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCString(
  PMTicketRef        ticket,
  UInt32             nodeIndex1,
  UInt32             nodeIndex2,
  CFStringRef        key,
  UInt32             bufferSize,
  CFStringEncoding   encoding,
  char *             value);



/* Numbers */
/* CFNumber */
/*
 *  PMTicketGetCFNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFNumber(
  PMTicketRef    ticket,
  UInt32         nodeIndex1,
  UInt32         nodeIndex2,
  CFStringRef    key,
  CFNumberRef *  item);


/* SInt32 */
/*
 *  PMTicketGetSInt32()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetSInt32(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  SInt32 *      value);


/* UInt32 */
/*
 *  PMTicketGetUInt32()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetUInt32(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  UInt32 *      value);


/* Double */
/*
 *  PMTicketGetDouble()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetDouble(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  double *      value);


/* PMResolution - Array of 2 doubles */
/*
 *  PMTicketGetPMResolution()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetPMResolution(
  PMTicketRef     ticket,
  UInt32          nodeIndex1,
  UInt32          nodeIndex2,
  CFStringRef     key,
  PMResolution *  res);


/*
 *  PMTicketGetIndexPMResolution()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetIndexPMResolution(
  PMTicketRef     ticket,
  UInt32          nodeIndex1,
  UInt32          nodeIndex2,
  CFStringRef     key,
  UInt32          index,
  PMResolution *  res);


/* PMRect - Array of 4 doubles */
/*
 *  PMTicketGetPMRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetPMRect(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  PMRect *      value);



/* Raw Data */
/* CFData */
/*
 *  PMTicketGetCFData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFData(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  CFDataRef *   item);


/* UInt8 - 'C' Bytes */
/* NOTE: Caller is expected to call twice if size is unknown, passing null for data pointer */
/* and a valid pointer for the size before allocating space and calling back with both pointers */
/* set, this time with valid data pointer and valid size pointer pointing to # of bytes requested. */
/*
 *  PMTicketGetBytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetBytes(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  UInt8 *       data,
  UInt32 *      size);


/* Booleans */
/* CFBoolean */
/*
 *  PMTicketGetCFBoolean()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFBoolean(
  PMTicketRef     ticket,
  UInt32          nodeIndex1,
  UInt32          nodeIndex2,
  CFStringRef     key,
  CFBooleanRef *  item);


/* 'C' Boolean  */
/*
 *  PMTicketGetBoolean()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetBoolean(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  Boolean *     value);


/* Dates */
/*
 *  PMTicketGetCFDate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFDate(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  CFDateRef *   item);


/* Non-numeric Arrays */
/*
 *  PMTicketGetCFArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFArray(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  CFArrayRef *  item);


/* Dictionaries */
/*
 *  PMTicketGetCFDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetCFDictionary(
  PMTicketRef        ticket,
  UInt32             nodeIndex1,
  UInt32             nodeIndex2,
  CFStringRef        key,
  CFDictionaryRef *  item);


/* PMTemplates, which are defined to be opaque. */
/*
 *  PMTicketCreateTemplate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketCreateTemplate(
  PMTicketRef      ticket,
  UInt32           nodeIndex1,
  UInt32           nodeIndex2,
  CFStringRef      key,
  PMTemplateRef *  item);



/* The generic calls for regular items and meta items. See notes on "SetMetaItem" for more details. */
/*
 *  PMTicketGetItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetItem(
  PMTicketRef   ticket,
  UInt32        nodeIndex1,
  UInt32        nodeIndex2,
  CFStringRef   key,
  CFTypeRef *   item);


/* Meta items don't get saved to XML. */
/*
 *  PMTicketGetMetaItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetMetaItem(
  PMTicketRef   ticket,
  CFStringRef   key,
  CFTypeRef *   item);



/* Any ticket can contain another ticket, allowing us to store multiple PageFormat tickets */
/* (one for each page) and multiple Document Info tickets (for multi-document jobs). The following */
/* functions allow us to put a ticket into another ticket as a sub-ticket, and retreive it */
/* later on. */
/*
 *  PMTicketSetTicket()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketSetTicket(
  PMTicketRef   ticket,
  PMTicketRef   ticketToAdd,
  UInt32        index);


/*
 *  PMTicketContainsTicket()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketContainsTicket(
  PMTicketRef   ticket,
  CFStringRef   requestedType,
  UInt32        index,
  Boolean *     exists);


/*
 *  PMTicketGetTicket()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetTicket(
  PMTicketRef    ticket,
  CFStringRef    requestedType,
  UInt32         index,
  PMTicketRef *  retreivedTicket);


/*
 *  PMTicketRemoveTicket()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketRemoveTicket(
  PMTicketRef   ticket,
  CFStringRef   typeToRemove,
  UInt32        index);


/*
 *  PMTicketGetPPDDict()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
PMTicketGetPPDDict(
  PMTicketRef               ticket,
  UInt32                    nodeIndex1,
  UInt32                    nodeIndex2,
  CFMutableDictionaryRef *  dict);





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
