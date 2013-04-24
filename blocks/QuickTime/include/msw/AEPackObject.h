/*
     File:       AEPackObject.h
 
     Contains:   AppleEvents object packing Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1991-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __AEPACKOBJECT__
#define __AEPACKOBJECT__

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
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

/* These are the object packing routines.  */
/*
 *  CreateOffsetDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ObjectSupportLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CreateOffsetDescriptor(
  long      theOffset,
  AEDesc *  theDescriptor);


/*
 *  CreateCompDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ObjectSupportLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CreateCompDescriptor(
  DescType   comparisonOperator,
  AEDesc *   operand1,
  AEDesc *   operand2,
  Boolean    disposeInputs,
  AEDesc *   theDescriptor);


/*
 *  CreateLogicalDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ObjectSupportLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CreateLogicalDescriptor(
  AEDescList *  theLogicalTerms,
  DescType      theLogicOperator,
  Boolean       disposeInputs,
  AEDesc *      theDescriptor);



/*
 *  CreateObjSpecifier()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ObjectSupportLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CreateObjSpecifier(
  DescType   desiredClass,
  AEDesc *   theContainer,
  DescType   keyForm,
  AEDesc *   keyData,
  Boolean    disposeInputs,
  AEDesc *   objSpecifier);


/*
 *  CreateRangeDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ObjectSupportLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CreateRangeDescriptor(
  AEDesc *  rangeStart,
  AEDesc *  rangeStop,
  Boolean   disposeInputs,
  AEDesc *  theDescriptor);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __AEPACKOBJECT__ */

