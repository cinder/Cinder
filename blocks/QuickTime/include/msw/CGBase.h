/*
     File:       CGBase.h
 
     Contains:   CoreGraphics base types
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGBASE_H_
#define CGBASE_H_

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#include <stddef.h>
#if __MWERKS__ > 0x2300 
#include <stdint.h>
#endif


#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

/* if stdint.h has been include, don't define same types */
#if TARGET_OS_WIN32
#ifndef _STDINT_H
#if __MWERKS__ <= 0x2300
typedef signed long                     int32_t;
#endif
/* define some unix types used by CoreGraphics */
typedef int                             boolean_t;
typedef unsigned char                   u_int8_t;
typedef unsigned short                  u_int16_t;
typedef unsigned long                   u_int32_t;
typedef unsigned char                   uint8_t;
typedef unsigned short                  uint16_t;
typedef unsigned long                   uint32_t;
#endif
#else
#include <stdint.h>
#endif

#if !defined(CG_INLINE)
#  if defined(__GNUC__)
#    define CG_INLINE static __inline__
#  elif defined(__MWERKS__)
#    define CG_INLINE static inline
#  else
#    define CG_INLINE static    
#  endif
#endif


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* CGBASE_H_ */

