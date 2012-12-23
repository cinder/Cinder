/*
     File:       AEMach.h
 
     Contains:   AppleEvent over mach_msg interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __AEMACH__
#define __AEMACH__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
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

/*-
 * AE Mach API --
 *
 * AppleEvents on OS X are implemented in terms of mach messages.
 * To facilitate writing server processes that can send and receive
 * AppleEvents, the following APIs are provided.
 *
 * AppleEvents are directed to a well known port uniquely tied to a
 * process.  The AE framework will discover this port based on the
 * keyAddressAttr of the event (as specifed in AECreateAppleEvent by
 * the target parameter.)  If a port cannot be found,
 * procNotFound (-600) will be returned on AESend.
 *
 * Of note is a new attribute for an AppleEvent, keyReplyPortAttr.
 * This specifies the mach_port_t to which an AppleEvent reply
 * should be directed.  By default, replies are sent to the
 * processes registered port where they are culled from the normal  
 * event stream if there is an outstanding AESend + kAEWaitReply.
 * But it may be desirable for a client to specify their own port to
 * receive quued replies.
 * (In the case of AESendMessage with kAEWaitReply specified, an 
 * anonymous port will be used to block until the reply is received.)
 *
 * Not supplied is a convenience routine to block a server and
 * process AppleEvents.  This implementation will be detailed in a
 * tech note.
 **/
enum {
  keyReplyPortAttr              = FOUR_CHAR_CODE('repp')
};

/* typeReplyPortAttr was misnamed and is deprecated; use keyReplyPortAttr instead. */
enum {
  typeReplyPortAttr             = keyReplyPortAttr
};

/*-
 * Return the mach_port_t that was registered with the bootstrap
 * server for this process.  This port is considered public, and
 * will be used by other applications to target your process.  You
 * are free to use this mach_port_t to add to a port set, if and
 * only if, you are not also using routines from HIToolbox.  In that
 * case, HIToolbox retains control of this port and AppleEvents are
 * dispatched through the main event loop.  
 **/
/*
 *  AEGetRegisteredMachPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( mach_port_t )
AEGetRegisteredMachPort(void);


/*-
 * Decode a mach_msg into an AppleEvent and its related reply.  (The
 * reply is set up from fields of the event.)  You can call this
 * routine if you wish to dispatch or handle the event yourself.  To
 * return a reply to the sender, you should call:
 *
 *  AESendMessage(reply, NULL, kAENoReply, kAENormalPriority, kAEDefaultTimeout);
 *
 * The contents of the header are invalid after this call.  
 **/
/*
 *  AEDecodeMessage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEDecodeMessage(
  mach_msg_header_t *  header,
  AppleEvent *         event,
  AppleEvent *         reply);       /* can be NULL */


/*-
 * Decodes and dispatches an event to an event handler.  Handles
 * packaging and returning the reply to the sender.
 *
 * The contents of the header are invalid after this call.
 **/
/*
 *  AEProcessMessage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEProcessMessage(mach_msg_header_t * header);


/*-
 * Send an AppleEvent to a target process.  If the target is the
 * current process (as specified by using typeProcessSerialNumber of
 * { 0, kCurrentProcess } it is dispatched directly to the
 * appropriate event handler in your process and not serialized.
 **/
/*
 *  AESendMessage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AESendMessage(
  const AppleEvent *  event,
  AppleEvent *        reply,                /* can be NULL */
  AESendMode          sendMode,
  long                timeOutInTicks);




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AEMACH__ */

