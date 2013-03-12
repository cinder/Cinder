/*
     File:       PMErrors.h
 
     Contains:   Mac OS X Printing Manager Error Codes.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
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

enum {
                                        /* general purpose printing error codes used by various printing modules */
  kPMAllocationFailure          = memFullErr, /* out of memory error */
  kPMInternalError              = -30870, /* internal printing error */
  kPMInvalidIndex               = -30882, /* invalid index in array */
  kPMStringConversionFailure    = -30883, /* error converting a string */
  kPMXMLParseError              = -30884, /* error parsing XML data */
  kPMInvalidJobTemplate         = -30885, /* invalid job template */
  kPMInvalidPrinterInfo         = -30886, /* invalid printer info ticket */
  kPMInvalidConnection          = -30887, /* invalid connection type */
  kPMInvalidKey                 = -30888, /* invalid key in ticket or template or dictionary */
  kPMInvalidValue               = -30889, /* invalid value in ticket or template or dictionary */
  kPMInvalidAllocator           = -30890, /* invalid memory allocator */
  kPMInvalidTicket              = -30891, /* invalid job ticket */
  kPMInvalidItem                = -30892, /* invalid item in ticket or template or dictionary */
  kPMInvalidType                = -30893, /* invalid type in ticket or template or dictionary */
  kPMInvalidReply               = -30894, /* invalid reply from a remote server/client */
  kPMInvalidFileType            = -30895, /* invalid file type in queue */
  kPMInvalidObject              = -30896, /* invalid object or internal error */
                                        /* Print Job Creator and Printing Dialog Extension error codes (-9500 to -9540) */
  kPMNoDefaultItem              = -9500, /* unused; to be removed */
  kPMNoDefaultSettings          = -9501, /* unused; to be removed */
  kPMInvalidPDEContext          = -9530, /* invalid printing dialog extension context */
  kPMDontSwitchPDEError         = -9531, /* tells the pjc not to switch panels */
  kPMUnableToFindProcess        = -9532, /* unable to find the Finder.app process */
                                        /* PrintCenter and Printer Browser error codes (-9540 to -9579) */
  kPMInvalidPBMRef              = -9540, /* invalid printer browser module reference.*/
  kPMNoSelectedPrinters         = -9541, /* no selected printers or error getting selection.*/
  kPMInvalidLookupSpec          = -9542, /* error retrieving lookup specification. */
  kPMSyncRequestFailed          = -9543, /* error handling sync request. */
  kPMEditRequestFailed          = -9544, /* error handling request to update Edit menu */
  kPMPrBrowserNoUI              = -9545, /* got UI function call with no UI present. */
                                        /* Job Ticket error codes (-9580 to -9619) */
  kPMTicketTypeNotFound         = -9580, /* we can't find the ticket type in our ticket. */
  kPMUpdateTicketFailed         = -9581, /* attempt to update ticket to current API failed. */
  kPMValidateTicketFailed       = -9582, /* ticket has at least one key that's invalid. */
  kPMSubTicketNotFound          = -9583, /* sub ticket requested is not stored in this ticket. */
  kPMInvalidSubTicket           = -9584, /* unable to add the requested sub-ticket. */
  kPMDeleteSubTicketFailed      = -9585, /* sub ticket could not be deleted. */
  kPMItemIsLocked               = -9586, /* item's locked flag was true when attempt made to update. */
  kPMTicketIsLocked             = -9587, /* caller may not change a locked ticket. */
  kPMTemplateIsLocked           = -9588, /* caller can't change the template. */
  kPMKeyNotFound                = -9589, /* the requested update is for a key that doesn't exist. */
  kPMKeyNotUnique               = -9590, /* the key passed in already exists in the ticket, can't make a new one. */
  kPMUnknownDataType            = -9591, /* couldn't determine proper CF type for the value passed in. */
                                        /* ClientPrintingLib (-9620 to -9629) */
  kPMCreateMessageFailed        = -9620, /* could not create message */
  kPMServerCommunicationFailed  = -9621, /* communication with print server failed */
  kPMKeyOrValueNotFound         = -9623, /* missing required key or value */
  kPMMessagingError             = -9624, /* could not connect to message port or send a message to remote client */
                                        /* Queue Manager (-9630 to -9659) */
  kPMServerNotFound             = -9630, /* print server not found */
  kPMServerAlreadyRunning       = -9631, /* print server is already running */
  kPMServerSuspended            = -9632, /* server suspended */
  kPMServerAttributeRestricted  = -9633, /* access to attribute restricted */
  kPMFileOrDirOperationFailed   = -9634, /* file/directory operation failed */
  kPMUserOrGroupNotFound        = -9635, /* specified user/group not found */
  kPMPermissionError            = -9636, /* permission related error */
  kPMUnknownMessage             = -9637, /* unknown message */
  kPMQueueNotFound              = -9638, /* queue not found */
  kPMQueueAlreadyExists         = -9639, /* queue already exists */
  kPMQueueJobFailed             = -9640, /* could not queue a new job */
  kPMJobNotFound                = -9641, /* job not found */
  kPMJobBusy                    = -9642, /* job is busy */
  kPMJobCanceled                = -9643, /* job has aborted */
  kPMDocumentNotFound           = -9644, /* document not found */
                                        /* Job Manager (-9660 to -9699) */
  kPMPMSymbolNotFound           = -9660, /* a required printer module symbol is missing */
  kPMIOMSymbolNotFound          = -9661, /* a required IO module symbol is missing */
  kPMCVMSymbolNotFound          = -9662, /* a required converter module symbol is missing */
  kPMInvalidPMContext           = -9663, /* PrinterModule context is invalid */
  kPMInvalidIOMContext          = -9664, /* IO Module context is invalid */
  kPMInvalidCVMContext          = -9665, /* Converter Module context is invalid */
  kPMInvalidJobID               = -9666, /* JobID passed from Printer Module is not valid */
  kPMNoPrinterJobID             = -9667, /* no JobID from target printer/connection */
  kPMJobStreamOpenFailed        = -9668, /* failed to open job stream */
  kPMJobStreamReadFailed        = -9669, /* failed to read from job stream */
  kPMJobStreamEndError          = -9670, /* reached end of job stream */
  kPMJobManagerAborted          = -9671, /* Job Manager is aborting */
                                        /* Converters (-9700 to -9739) */
  kPMPluginNotFound             = -9701, /* Converter plugin not found */
  kPMPluginRegisterationFailed  = -9702, /* Converter Plugin error */
  kPMFontNotFound               = -9703, /* Font not found */
  kPMFontNameTooLong            = -9704, /* font name too long */
  kPMGeneralCGError             = -9705, /* CoreGraphics returned error */
  kPMInvalidState               = -9706, /* Invalid converter state */
                                        /* Printer Modules (-9740 to -9779) */
                                        /* IO Modules (-9780 to -9799) */
  kPMInvalidPrinterAddress      = -9780, /* a file or connection could not be open */
  kPMOpenFailed                 = -9781, /* a file or connection could not be open */
  kPMReadFailed                 = -9782, /* file/connection read failed */
  kPMWriteFailed                = -9783, /* file/connection write failed */
  kPMStatusFailed               = -9784, /* connection status failed */
  kPMCloseFailed                = -9785, /* close file/connection failed */
  kPMUnsupportedConnection      = -9786, /* connection type not supported */
  kPMIOAttrNotAvailable         = -9787 /* IO attribute not available on current connection type */
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

#ifdef __cplusplus
}
#endif
