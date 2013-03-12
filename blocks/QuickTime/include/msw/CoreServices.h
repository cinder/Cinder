/*
     File:       CoreServices.h
 
     Contains:   Master include for CoreServices (non-UI toolbox)
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CORESERVICES__
#define __CORESERVICES__

/*
   By default using this header means you are building code for carbon.
   You can override this by setting TARGET_API_MAC_CARBON to zero before
   including this file
*/
#ifndef TARGET_API_MAC_CARBON
#define TARGET_API_MAC_CARBON 1
#endif  /* !defined(TARGET_API_MAC_CARBON) */


#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FINDER__
#include <Finder.h>
#endif

#ifndef __FIXMATH__
#include <FixMath.h>
#endif

#ifndef __SCRIPT__
#include <Script.h>
#endif

#ifndef __UTCUTILS__
#include <UTCUtils.h>
#endif

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

#ifndef __COLLECTIONS__
#include <Collections.h>
#endif

#ifndef __ENDIAN__
#include <Endian.h>
#endif

#ifndef __PATCHES__
#include <Patches.h>
#endif

#ifndef __GESTALT__
#include <Gestalt.h>
#endif

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

#ifndef __MATH64__
#include <Math64.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

#ifndef __ALIASES__
#include <Aliases.h>
#endif

#ifndef __MACLOCALES__
#include <MacLocales.h>
#endif

#ifndef __DEBUGGING__
#include <Debugging.h>
#endif

#ifndef __PLSTRINGFUNCS__
#include <PLStringFuncs.h>
#endif

#ifndef __DRIVERSYNCHRONIZATION__
#include <DriverSynchronization.h>
#endif

#ifndef __DEVICES__
#include <Devices.h>
#endif

#ifndef __DRIVERSERVICES__
#include <DriverServices.h>
#endif

#ifndef __INTLRESOURCES__
#include <IntlResources.h>
#endif

#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif

#ifndef __DATETIMEUTILS__
#include <DateTimeUtils.h>
#endif

#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

#ifndef __NAMEREGISTRY__
#include <NameRegistry.h>
#endif

#ifndef __UNICODEUTILITIES__
#include <UnicodeUtilities.h>
#endif

#ifndef __FP__
#include <fp.h>
#endif

#ifndef __TEXTENCODINGCONVERTER__
#include <TextEncodingConverter.h>
#endif

#ifndef __UNICODECONVERTER__
#include <UnicodeConverter.h>
#endif

#ifndef __THREADS__
#include <Threads.h>
#endif

#ifndef __FOLDERS__
#include <Folders.h>
#endif

#ifndef __TIMER__
#include <Timer.h>
#endif

#ifndef __MULTIPROCESSINGINFO__
#include <MultiprocessingInfo.h>
#endif

#ifndef __MULTIPROCESSING__
#include <Multiprocessing.h>
#endif

#ifndef __MACHINEEXCEPTIONS__
#include <MachineExceptions.h>
#endif

#ifndef __AVLTREE__
#include <AVLTree.h>
#endif

#ifndef __PEFBINARYFORMAT__
#include <PEFBinaryFormat.h>
#endif

#ifndef __HFSVOLUMES__
#include <HFSVolumes.h>
#endif

#ifndef __AIFF__
#include <AIFF.h>
#endif

#ifndef __TEXTENCODINGPLUGIN__
#include <TextEncodingPlugin.h>
#endif

#ifndef __APPLEDISKPARTITIONS__
#include <AppleDiskPartitions.h>
#endif

#ifndef __POWER__
#include <Power.h>
#endif

#ifndef __SCSI__
#include <SCSI.h>
#endif

#ifndef __SYSTEMSOUND__
#include <SystemSound.h>
#endif

#ifndef __ICONSTORAGE__
#include <IconStorage.h>
#endif

#ifndef __CFBASE__
#include <CFBase.h>
#endif

#ifndef __CFARRAY__
#include <CFArray.h>
#endif

#ifndef __CFBAG__
#include <CFBag.h>
#endif

#ifndef __CFBUNDLE__
#include <CFBundle.h>
#endif

#ifndef __CFCHARACTERSET__
#include <CFCharacterSet.h>
#endif

#ifndef __CFDATA__
#include <CFData.h>
#endif

#ifndef __CFDATE__
#include <CFDate.h>
#endif

#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
#endif

#ifndef __CFNUMBER__
#include <CFNumber.h>
#endif

#ifndef __CFPLUGIN__
#include <CFPlugIn.h>
#endif

#ifndef __CFPREFERENCES__
#include <CFPreferences.h>
#endif

#ifndef __CFPROPERTYLIST__
#include <CFPropertyList.h>
#endif

#ifndef __CFSET__
#include <CFSet.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFSTRINGENCODINGEXT__
#include <CFStringEncodingExt.h>
#endif

#ifndef __CFTIMEZONE__
#include <CFTimeZone.h>
#endif

#ifndef __CFTREE__
#include <CFTree.h>
#endif

#ifndef __CFURL__
#include <CFURL.h>
#endif

#ifndef __CFURLACCESS__
#include <CFURLAccess.h>
#endif

#ifndef __CFUUID__
#include <CFUUID.h>
#endif

#ifndef __CFXMLNODE__
#include <CFXMLNode.h>
#endif

#ifndef __CFXMLPARSER__
#include <CFXMLParser.h>
#endif

#ifndef __CFPLUGINCOM__
#include <CFPlugInCOM.h>
#endif

#ifndef __CFBINARYHEAP__
#include <CFBinaryHeap.h>
#endif

#ifndef __CFBITVECTOR__
#include <CFBitVector.h>
#endif

#ifndef __CFMACHPORT__
#include <CFMachPort.h>
#endif

#ifndef __CFMESSAGEPORT__
#include <CFMessagePort.h>
#endif

#ifndef __CFNOTIFICATIONCENTER__
#include <CFNotificationCenter.h>
#endif

#ifndef __CFRUNLOOP__
#include <CFRunLoop.h>
#endif

#ifndef __CFSOCKET__
#include <CFSocket.h>
#endif

#ifndef __CFSTREAM__
#include <CFStream.h>
#endif

#ifndef __CFNETWORK__
#include <CFNetwork.h>
#endif

#ifndef __WSMETHODINVOCATION__
#include <WSMethodInvocation.h>
#endif


#endif /* __CORESERVICES__ */

