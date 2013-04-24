/*
     File:       CFDate.h
 
     Contains:   CoreFoundation date
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CFDATE__
#define __CFDATE__



#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#include <CFBase.h>

typedef double                          CFTimeInterval;
typedef CFTimeInterval                  CFAbsoluteTime;
typedef const struct __CFDate*          CFDateRef;

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __CFDATE__ */

