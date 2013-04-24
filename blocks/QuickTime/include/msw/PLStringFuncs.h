/*
     File:       PLStringFuncs.h
 
     Contains:   Pascal string manipulation routines that parallel ANSI C string.h
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __PLSTRINGFUNCS__
#define __PLSTRINGFUNCS__

#ifndef __MACTYPES__
#include <MacTypes.h>
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

/*
 *  PLstrcmp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
PLstrcmp(
  ConstStr255Param   str1,
  ConstStr255Param   str2);


/*
 *  PLstrncmp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
PLstrncmp(
  ConstStr255Param   str1,
  ConstStr255Param   str2,
  short              num);


/*
 *  PLstrcpy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr )
PLstrcpy(
  StringPtr          str1,
  ConstStr255Param   str2);


/*
 *  PLstrncpy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr )
PLstrncpy(
  StringPtr          str1,
  ConstStr255Param   str2,
  short              num);


/*
 *  PLstrcat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr )
PLstrcat(
  StringPtr          str1,
  ConstStr255Param   str2);


/*
 *  PLstrncat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr )
PLstrncat(
  StringPtr          str1,
  ConstStr255Param   str2,
  short              num);


/*
 *  PLstrchr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr )
PLstrchr(
  ConstStr255Param   str1,
  short              ch1);


/*
 *  PLstrrchr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr )
PLstrrchr(
  ConstStr255Param   str1,
  short              ch1);


/*
 *  PLstrpbrk()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr )
PLstrpbrk(
  ConstStr255Param   str1,
  ConstStr255Param   str2);


/*
 *  PLstrspn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
PLstrspn(
  ConstStr255Param   str1,
  ConstStr255Param   str2);


/*
 *  PLstrstr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr )
PLstrstr(
  ConstStr255Param   str1,
  ConstStr255Param   str2);


/*
 *  PLstrlen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
PLstrlen(ConstStr255Param str);


/*
 *  PLpos()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
PLpos(
  ConstStr255Param   str1,
  ConstStr255Param   str2);




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PLSTRINGFUNCS__ */

