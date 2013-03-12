/*
     File:       DigitalHubRegistry.h
 
     Contains:   Digital Hub AppleEvents
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2002 by Apple Computer, Inc.  All rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DIGITALHUBREGISTRY__
#define __DIGITALHUBREGISTRY__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif




#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

/* class */
enum {
  kDigiHubEventClass            = FOUR_CHAR_CODE('dhub')
};


/* events*/
enum {
  kDigiHubMusicCD               = FOUR_CHAR_CODE('aucd'), /* 1635083108 0x61756364*/
  kDigiHubPictureCD             = FOUR_CHAR_CODE('picd'), /* 1885954916 0x70696364*/
  kDigiHubVideoDVD              = FOUR_CHAR_CODE('vdvd'), /* 1986295396 0x76647664*/
  kDigiHubBlankCD               = FOUR_CHAR_CODE('bcd '), /* 1650680864 0x62636420*/
  kDigiHubBlankDVD              = FOUR_CHAR_CODE('bdvd') /* 1650751076 0x62647664*/
};

/*
    Parameters for Digital Hub AppleEvents:
                    
        kDigiHubMusicCD
        Required parameters:
        -->     keyDirectObject         typeFSRef

        kDigiHubPictureCD
        Required parameters:
        -->     keyDirectObject         typeFSRef
                
        kDigiHubVideoDVD
        Required parameters:
        -->     keyDirectObject         typeFSRef
                
        kDigiHubBlankCD
        Required parameters:
        -->     keyDirectObject         typeUTF8Text

        kDigiHubBlankDVD
        Required parameters:
        -->     keyDirectObject         typeUTF8Text

*/

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __DIGITALHUBREGISTRY__ */

