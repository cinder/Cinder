/*
     File:       CFRunLoop.h
 
     Contains:   CoreFoundation binary heap
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CFRUNLOOP__
#define __CFRUNLOOP__



#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#include <CFBase.h>

typedef struct __CFRunLoop*             CFRunLoopRef;
typedef struct __CFRunLoopSource*       CFRunLoopSourceRef;

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __CFRUNLOOP__ */

