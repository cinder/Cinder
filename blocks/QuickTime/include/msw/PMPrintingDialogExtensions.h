/*
     File:       PMPrintingDialogExtensions.h
 
     Contains:   Mac OS X Printing Manager Print Dialog Extension Interfaces.
 
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

#ifndef __PMAPPLICATION__
#include <PMApplication.h>
#endif

#ifndef __PMTICKET__
#include <PMTicket.h>
#endif

#ifndef __PMPLUGINHEADER__
#include <PMPluginHeader.h>
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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Constants
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
Feature request flags...
*/
typedef UInt32 PMPDEFlags;
enum {
  kPMPDENoFlags                 = 0x00000000, /* Empty flag word. */
  kPMPDENoSummary               = 0x00000001, /* Won't provide summary string. */
  kPMPDEAllFlags                = (unsigned long)0xFFFFFFFF /* All flags set.*/
};

/*
    Parameters for printing events:
    
    kEventPrintingPDEResize
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamPDEHeight        typeUInt32
        
    When sent to the this event is sent to the PageSetup or Print Dialog, the PDE pane
    will be resized to match the new height.  The kEventParamDirectObject needs to be the 
    embedderUserPane that is passed into Initialize
*/
enum {
  kEventClassPrinting           = FOUR_CHAR_CODE('prnt'),
  kEventPrintingPDEResize       = FOUR_CHAR_CODE('rqsz'),
  kEventParamPDEHeight          = FOUR_CHAR_CODE('pdeh')
};

/*
Define Printing Dialog Extensions CFPlugIn constants.

First, the base string for all constants relating to Printing Dialog Extensions.
*/
#define kPMPDEBase                      "com.apple.printing.pde"
/*
Define the Type and Interface keys for Printing Dialog Extensions.
*/
#define kPMPDEAppType                   CFSTR("com.apple.printing.pde.app")
#define KPMPDEPrinterType               CFSTR("com.apple.printing.pde.pm")
#define kPMPDEInterface                 CFSTR("com.apple.printing.pde.ui")
/*
Define the Kind IDs for Universal and Standard Printing Dialog Extensions.
*/
/* Implemented Universal */
#define kPMPageAttributesKindID         CFSTR("com.apple.print.pde.PageAttributesKind")
#define kPMCopiesAndPagesPDEKindID      CFSTR("com.apple.print.pde.CopiesAndPagesKind")
#define kPMLayoutPDEKindID              CFSTR("com.apple.print.pde.LayoutUserOptionKind")
#define kPMOutputOptionsPDEKindID       CFSTR("com.apple.print.pde.OutputOptionsKind")
#define kPMDuplexPDEKindID              CFSTR("com.apple.print.pde.DuplexKind")
/* Unimplemented Universal */
#define kPMCoverPagePDEKindID           CFSTR("com.apple.print.pde.CoverPageKind")
#define kPMPaperSourcePDEKindID         CFSTR("com.apple.print.pde.PaperSourceKind")
#define kPMPriorityPDEKindID            CFSTR("com.apple.print.pde.PriorityKind")
#define kPMRotationScalingPDEKindID     CFSTR("com.apple.print.pde.RotationScalingKind")
/* Implemented Standard */
#define kPMPaperFeedPDEKindID           CFSTR("com.apple.print.pde.PaperFeedKind")
/* Unimplemented Standard */
#define kPMColorPDEKindID               CFSTR("com.apple.print.pde.ColorKind")
#define kPMQualityMediaPDEKindID        CFSTR("com.apple.print.pde.QualityMediaPDEKind")
/*  These constants are used by PDE's to obtain the PMTicketRef data for the corresponding
    piece of data.
*/
#define kPDE_PMPrintSettingsRef         CFSTR("PMPrintSettingsTicket")
#define kPDE_PMPageFormatRef            CFSTR("PMPageFormatTicket")
#define kPDE_PMJobTemplateRef           CFSTR("PMJobTemplateTicket")
#define kPDE_PMPrinterInfoRef           CFSTR("PMPrinterInfoTicket")

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Type Definitions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
Basic types...
*/
typedef struct OpaquePMPDERef*          PMPDERef;
typedef struct OpaquePMPDEContext*      PMPDEContext;
/* Just for testing this .... */
/*#define    TEST_PLUGINS_LINKED_IN     1 */
/* Type and Interface IDs. */
#define kDialogExtensionIntfIDStr       CFSTR("A996FD7E-B738-11D3-8519-0050E4603277")
#define kGeneralPageSetupDialogTypeIDStr  CFSTR("6E6ED964-B738-11D3-952F-0050E4603277")
#define kGeneralPrintDialogTypeIDStr    CFSTR("C1BF838E-B72A-11D3-9644-0050E4603277")
#define kAppPageSetupDialogTypeIDStr    CFSTR("B9A0DA98-E57F-11D3-9E83-0050E4603277")
#define kAppPrintDialogTypeIDStr        CFSTR("BCB07250-E57F-11D3-8CA6-0050E4603277")
#define kPrinterModuleTypeIDStr         CFSTR("BDB091F4-E57F-11D3-B5CC-0050E4603277")
/* Versioning info. */
/* Update following values when you append new APIs to the end of interface: */
#define kPDEBuildVersionMajor   1
#define kPDEBuildVersionMinor   0
/* Update following values when you obsolete some APIs in the interface. */
/* Since this will break the upward compatibility chain such practice is */
/* strongly discouraged - if it can be helped at all. */
#define kPDEBaseVersionMajor        1
#define kPDEBaseVersionMinor        0

/* Interface layout - object and vtable: */
typedef CALLBACK_API_C( OSStatus , PMPDEPrologueProcPtr )(PMPDEContext *context, OSType *creator, CFStringRef *userOptionKind, CFStringRef *title, UInt32 *maxH, UInt32 *maxV);
typedef CALLBACK_API_C( OSStatus , PMPDEInitializeProcPtr )(PMPDEContext context, PMPDEFlags *flags, PMPDERef ref, ControlRef embedderUserPane, PMPrintSession printSession);
typedef CALLBACK_API_C( OSStatus , PMPDESyncProcPtr )(PMPDEContext context, PMPrintSession printSession, Boolean reinitializePlugIn);
typedef CALLBACK_API_C( OSStatus , PMPDEGetSummaryTextProcPtr )(PMPDEContext context, CFArrayRef *titleArray, CFArrayRef *summaryArray);
typedef CALLBACK_API_C( OSStatus , PMPDEOpenProcPtr )(PMPDEContext context);
typedef CALLBACK_API_C( OSStatus , PMPDECloseProcPtr )(PMPDEContext context);
typedef CALLBACK_API_C( OSStatus , PMPDETerminateProcPtr )(PMPDEContext context, OSStatus status);
struct PlugInIntfVTable {
                                              /* ---- API 'IMMUTABLE' Begin: ----*/

                                              /* The plugin header is required with all plugins.*/
                                              /* This must lead object's layout.*/
  PMPlugInHeader      plugInHeader;

                                              /* ---- API Version 1.0 Begin: ----*/

                                              /* Entry points for Printing Dialog Extension modules...*/
  PMPDEPrologueProcPtr  Prologue;
  PMPDEInitializeProcPtr  Initialize;
  PMPDESyncProcPtr    Sync;
  PMPDEGetSummaryTextProcPtr  GetSummaryText;
  PMPDEOpenProcPtr    Open;
  PMPDECloseProcPtr   Close;
  PMPDETerminateProcPtr  Terminate;
};
typedef struct PlugInIntfVTable         PlugInIntfVTable;
struct PlugInIntf {
                                              /* Plugin interface is an object containing vtable address:*/
  PlugInIntfVTable *  vtable;
};
typedef struct PlugInIntf               PlugInIntf;
/* Some commonly used routines used by PDEs */
/*
 *  PMCreateLocalizedPaperSizeCFString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CFStringRef )
PMCreateLocalizedPaperSizeCFString(
  PMTicketRef   listofPaperTickets,
  UInt16        paperToUse);


/* Lower level version of the same function, this time passing the single PaperInfo.*/
/*
 *  PMCreatePaperSizeCFString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CFStringRef )
PMCreatePaperSizeCFString(PMTicketRef selectedPaper);




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
