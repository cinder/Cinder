/*
     File:       PMIOModule.h
 
     Contains:   Mac OS X Printing Manager IO Module Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __PMTICKET__
#include <PMTicket.h>
#endif

#ifndef __PMPLUGINHEADER__
#include <PMPluginHeader.h>
#endif

#ifndef __PMERRORS__
#include <PMErrors.h>
#endif



/* context ID for IO Modules */


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

typedef struct OpaqueIOMContext*        IOMContext;
/* kPMPrinterURI corresponds to a CFString to a connection-specific 
address specification for the printer. For instance, on an lpd connection  
this would be an address of the form lpd://<host>/<queue name>. */
#define kPMPrinterURI                   CFSTR("Printer URI")
/* System-supported connection type names used in identifying the different connection types
and for requesting lookup information from Printer Modules. PrintCenter does not use these 
strings in its UI, instead it gets the actual UI strings from the Printer Browser Module. */
#define kPMAppleTalkConnection          CFSTR("AppleTalk")
#define kPMUSBConnection                CFSTR("USB")
#define kPMLPRConnection                CFSTR("LPR")
#define kPMDirServicesConnection        CFSTR("Directory Services")
/* possible attributes that can be requested from an IO Module and allow clients to determine various 
characteristics about the IOM, its connection type, and the target printer.  The same attribute can 
be used for getting a value or setting it. */
#define kPMBiDiAttr                     CFSTR("Supports BiDirectional Communication")
/* bi-directional communication capability */
#define kPM8BitChannelAttr              CFSTR("Supports 8-bit Channel")
/* full 8-bit per byte data (vs. 7) */
#define kPMTransparentByteRange         CFSTR("Supports Transparent Byte Range")
/* transparent byte range */
#define kPMJobIDAttr                    CFSTR("Supports JobID")
/* in-printer-jobID */
#define kPMTimeoutAttr                  CFSTR("Supports Timeout")
/* get/set timeout duration */
#define kPMTStatusAttr                  CFSTR("Supports T Status Request")
/* status on the T channel */
/* Used with the IOMGetAttribute call, a supporting IO Module will hand back
 * a copy of a CFStringRef describing the last error encountered. If there was
 * no last error, or if there is no available string then NULL is passed back.
 */
#define kPMLastErrorStrAttr             CFSTR("Last Error")

/* Type ID, interface ID for the IOM CFPlugin */
#define kIOModuleTypeIDStr              "EE5A5E5C-C2DD-11D3-84B5-00050209D9C1"
#define kIOModuleIntfIDStr              "F4F7510C-C2DD-11D3-93A5-00050209D9C1"
/* IOM API version numbers (see PMPluginHeader.h for the meaning of these defintions) */
enum {
  kIOMBuildVersionMajor         = 1,
  kIOMBuildVersionMinor         = 0,
  kIOMBaseVersionMajor          = 1,
  kIOMBaseVersionMinor          = 0
};

typedef CALLBACK_API_C( OSStatus , PMIOModuleGetConnectionInfoProc )(CFStringRef *connectionType, CFStringRef *pbmPath);
typedef CALLBACK_API_C( OSStatus , PMIOModuleInitializeProc )(CFDataRef printerAddress, IOMContext *ioModuleContextPtr);
typedef CALLBACK_API_C( OSStatus , PMIOModuleOpenProc )(IOMContext ioModuleContext, PMTicketRef jobTicket, UInt32 *bufferSize);
typedef CALLBACK_API_C( OSStatus , PMIOModuleReadProc )(IOMContext ioModuleContext, Ptr buffer, UInt32 *size, Boolean *eoj);
typedef CALLBACK_API_C( OSStatus , PMIOModuleWriteProc )(IOMContext ioModuleContext, Ptr buffer, UInt32 *size, Boolean eoj);
typedef CALLBACK_API_C( OSStatus , PMIOModuleStatusProc )(IOMContext ioModuleContext, CFStringRef *status);
typedef CALLBACK_API_C( OSStatus , PMIOModuleGetAttributeProc )(IOMContext ioModuleContext, CFStringRef attribute, CFTypeRef *result);
typedef CALLBACK_API_C( OSStatus , PMIOModuleSetAttributeProc )(IOMContext ioModuleContext, CFStringRef attribute, CFTypeRef data);
typedef CALLBACK_API_C( OSStatus , PMIOModuleCloseProc )(IOMContext ioModuleContext, Boolean abort);
typedef CALLBACK_API_C( OSStatus , PMIOModuleTerminateProc )(IOMContext * ioModuleContextPtr);
/* IOMProcs contains the routines required to be exported by an IOM Module. */
struct IOMProcs {
                                              /* The plugin header is required with all plugins and must proceed object's layout.*/
  PMPlugInHeader      pluginHeader;

                                              /* The actual IO Module routine Procs*/

                                              /* GetConnectionInfo: returns the connection type and the path to the companion browser module relative */
                                              /* to /Library/Printers/.  The connection types: AppleTalk, USB, LPR, and Directory Services are reserved by Apple.*/
  PMIOModuleGetConnectionInfoProc  GetConnectionInfo;

                                              /* Initialize: creates a context to store local variables during an IO session.  printerAddress of*/
                                              /* the target printer is passed as CFDataRef as defined by the protocol and set by the Printer Browser Module*/
  PMIOModuleInitializeProc  Initialize;

                                              /* Open: open IO connection with the target device.  The parameter buffSize is set*/
                                              /* by the IOM to return its maximum write buffer size.*/
  PMIOModuleOpenProc  Open;

                                              /* Read: read data from the device over the read channel, if any.  */
  PMIOModuleReadProc  Read;

                                              /* Write: write data to the device*/
  PMIOModuleWriteProc  Write;

                                              /* Status: get status from the device*/
  PMIOModuleStatusProc  Status;

                                              /* GetAttribute: checks if a defined IO attribute is supported by the target IO connection.  If the requested */
                                              /* 'attribute' is supported and available, it's returned in 'result'.  If supported but not available, error code */
                                              /* kAttrNotAvailable is returned.  If the attribute is not supported, kPMNotImplemented is returned.*/
  PMIOModuleGetAttributeProc  GetAttribute;

                                              /* SetAttribute: sets a pre-defined 'attribute' to a given value passed in 'data'.  Only supported attributes*/
                                              /* can be set; otherwise kAttrNotAvailable or kPMNotImplemented is returned.*/
  PMIOModuleSetAttributeProc  SetAttribute;

                                              /* Close: close IO connection with the target device.  The parameter 'abort' indicates whether the current job*/
                                              /* is to be canceled before closing connection (true; i.e. user cancels) or not (false; i.e. normal job completion).*/
  PMIOModuleCloseProc  Close;

                                              /* Terminate: dispose of any data allocated in Initialize.*/
  PMIOModuleTerminateProc  Terminate;

};
typedef struct IOMProcs                 IOMProcs;
/* IOM interface is an object containing addresses to the module's entry points:*/
struct IOMInterface {
  const IOMProcs *    vtable;
};
typedef struct IOMInterface             IOMInterface;
typedef const IOMInterface *            IOMInterfaceRef;

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
