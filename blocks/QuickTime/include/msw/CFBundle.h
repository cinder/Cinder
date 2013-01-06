/*
     File:       CFBundle.h
 
     Contains:   CoreFoundation bundle
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CFBUNDLE__
#define __CFBUNDLE__



#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#include <CFBase.h>

typedef struct __CFBundle*              CFBundleRef;
typedef struct __CFBundle*              CFPlugInRef;

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __CFBUNDLE__ */

