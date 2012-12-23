/*
     File:       OSServices.h
 
     Contains:   Master include for OSServices private framework
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __OSSERVICES__
#define __OSSERVICES__

#ifndef __CARBONCORE__
#include <CarbonCore.h>
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


#include <OSServices/OpenTransport.h>
#include <OSServices/OpenTransportProviders.h>
#include <OSServices/OpenTransportProtocol.h>
#include <OSServices/NSLCore.h>
#include <OSServices/SecurityCore.h>

#endif /* __OSSERVICES__ */

