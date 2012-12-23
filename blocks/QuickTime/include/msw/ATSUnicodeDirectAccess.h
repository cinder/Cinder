/*
     File:       ATSUnicodeDirectAccess.h
 
     Contains:   Public Interfaces/Types for Low Level ATSUI
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2002 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ATSUNICODEDIRECTACCESS__
#define __ATSUNICODEDIRECTACCESS__

#ifndef __ATSUNICODE__
#include <ATSUnicode.h>
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

/* ---------------------------------------------------------------------------- */
/* Constants                                                                    */
/* ---------------------------------------------------------------------------- */

/*
 *  ATSUDirectDataSelector
 *  
 *  Summary:
 *    These are the data selectors used in the
 *    ATSUDirectGetLayoutDataArrayPtr function to get the needed layout
 *    data array pointer.
 */
typedef UInt32 ATSUDirectDataSelector;
enum {

  /*
   * Returns the parallel advance delta (delta X) array. (Array Type):
   * Fixed (Return Time): Constant, unless creation is necessary, or
   * unless requested by ATSUDirectGetLayoutDataArrayPtrFromTextLayout.
   * (Creation): This array is created only on demand. Thus, if any
   * changes are to be made iCreate should be set to true. If the array
   * had not been previously allocated it will be allocated and
   * zero-filled when iCreate is set to true.
   */
  kATSUDirectDataAdvanceDeltaFixedArray = 0L,

  /*
   * Returns the parallel baseline delta (delta Y) array. (Array Type):
   * Fixed (Return Time): Constant, unless creation is necessary, or
   * unless requested by ATSUDirectGetLayoutDataArrayPtrFromTextLayout.
   * (Creation): This array is created only on demand. Thus, if any
   * changes are to be made iCreate should be set to true. If the array
   * had not been previously allocated it will be allocated and
   * zero-filled when iCreate is set to true.
   */
  kATSUDirectDataBaselineDeltaFixedArray = 1L,

  /*
   * Returns the parallel device delta array for device- specific
   * tweaking. This is an array of values which are used to adjust
   * truncated fractional values for devices that do not accept
   * fractional positioning. It is also used to provide precise
   * positioning for connected scripts. (Array Type): SInt16 (Return
   * Time): Constant, unless creation is necessary, or unless requested
   * by ATSUDirectGetLayoutDataArrayPtrFromTextLayout. (Creation): This
   * array is created only on demand. Thus, if any changes are to be
   * made iCreate should be set to true. If the array had not been
   * previously allocated it will be allocated and zero-filled when
   * iCreate is set to true.
   */
  kATSUDirectDataDeviceDeltaSInt16Array = 2L,

  /*
   * Returns the parallel style index array. The indexes setting in the
   * array are indexes into the the StyleSetting array, which can be
   * obtained using the
   * kATSUDirectDataStyleSettingATSUStyleSettingRefArray below. (Array
   * Type): UInt16 (Return Time): Constant, unless creation is
   * necessary, or unless requested by
   * ATSUDirectGetLayoutDataArrayPtrFromTextLayout. (Creation): This
   * array is created only on demand. Thus, if any changes are to be
   * made iCreate should be set to true. If the array had not been
   * previously allocated it will be allocated and zero-filled when
   * iCreate is set to true.
   */
  kATSUDirectDataStyleIndexUInt16Array = 3L,

  /*
   * Returns the style setting ref array. (Array Type):
   * ATSUStyleSettingRef (Return Time): Linear, based on the number of
   * styles applied to the given line. (Creation): This array is always
   * present if the layout has any text assigned to it at all. Setting
   * iCreate has no effect.
   */
  kATSUDirectDataStyleSettingATSUStyleSettingRefArray = 4L,

  /*
   * Returns the ATSLayoutRecord, version 1 array. This should not be
   * used directly at all. Rather, use the
   * kATSUDirectDataLayoutRecordATSLayoutRecordCurrent selector below.
   * This will ensure that the code will always be using the most
   * current version of the ATSLayoutRecord, should there ever be a
   * change. ATSUI will only ensure the most efficient processing will
   * occur for the latest version of ATSLayoutRecord. (Array Type):
   * ATSLayoutRecord, version 1 (Return Time): Constant, unless
   * creation is necessary, or unless requested by
   * ATSUDirectGetLayoutDataArrayPtrFromTextLayout. (Creation): This
   * array is always present if the layout has any text assigned to it
   * at all. Setting iCreate has no effect
   */
  kATSUDirectDataLayoutRecordATSLayoutRecordVersion1 = 100L,

  /*
   * Returns the ATSLayoutRecord. This will return the most current
   * version of the ATSLayoutRecord, and the one that's defined in this
   * file. Always use kATSUDirectDataLayoutRecordATSLayoutRecordCurrent
   * to get the array of ATSLayoutRecords. (Array Type):
   * ATSLayoutRecord (Return Time): Constant, unless creation is
   * necessary, or unless requested by
   * ATSUDirectGetLayoutDataArrayPtrFromTextLayout. (Creation): This
   * array is always present if the layout has any text assigned to it
   * at all. Setting iCreate has no effect.
   */
  kATSUDirectDataLayoutRecordATSLayoutRecordCurrent = kATSUDirectDataLayoutRecordATSLayoutRecordVersion1
};

/* ---------------------------------------------------------------------------- */
/* Data Types                                                                   */
/* ---------------------------------------------------------------------------- */

/*
 *  ATSUStyleSettingRef
 *  
 *  Summary:
 *    A reference to a style setting object that represents an
 *    ATSUStyle plus any cached/set information about that style.
 */
typedef struct ATSStyleSetting*         ATSUStyleSettingRef;
/* ---------------------------------------------------------------------------- */
/* Direct Accessors                                                             */
/* ---------------------------------------------------------------------------- */
/*
 *  ATSUDirectGetLayoutDataArrayPtrFromLineRef()
 *  
 *  Summary:
 *    Returns the data pointer specified by iDataSelector and
 *    referenced by iLineRef.
 *  
 *  Discussion:
 *    This function simply returns the data pointer specified by
 *    iDataSelector and referenced by iLineRef. This data pointer
 *    should not be freed directly after it's been used. Rather, it
 *    should be released using ATSUDirectReleaseLayoutDataArrayPtr.
 *    Doing so serves as a signal to ATSUI that the caller is done with
 *    the data and that it can merge it in smoothly and adjust its
 *    internal processes. Furthermore, it may be the case that the
 *    pointer returned may be dynamically allocated one or contain
 *    dynamically allocated data. If it's not properly freed, a memory
 *    leak may result. This function may only be called within the
 *    context of an ATSUDirectLayoutOperationOverrideUPP callback. The
 *    pointer returned points to the exact data referenced by the
 *    ATSUTextLayout object that triggered the callback call. This is
 *    by far the most efficient way to use the direct access calls
 *    because for most requested data, no allocation and copy is done.
 *    Furthermore, because this a direct pointer to the data that ATSUI
 *    will use for it's layout, the data arrays returned by this can be
 *    tweaked and edited. Many of the requested arrays are created by
 *    ATSUI only when necessary. If these arrays are to be altered,
 *    then be sure to set iCreate to true. This will ensure that this
 *    array is created. If the arrays are not created, ATSUI
 *    automatically assumes that all entries in the array are zero. The
 *    pointer returned by this function is only valid within the
 *    context of the callback. Do not attempt to retain it for later
 *    use.
 *  
 *  Parameters:
 *    
 *    iLineRef:
 *      The ATSULineRef which was passed into a
 *      ATSUDirectLayoutOperationOverrideUPP callback function as a
 *      parameter.
 *    
 *    iDataSelector:
 *      The selector for the data that is being requested.
 *    
 *    iCreate:
 *      If the ATSULineRef passed in iLineRef does not reference the
 *      requested array, then a zero-filled one will be created and
 *      returned in oLayoutDataArray if this is set to true. For some
 *      ATSUDirectDataSelectors, these cannot be simply created. Thus,
 *      this flag will have no affect on these few
 *      ATSUDirectDataSelectors.
 *    
 *    oLayoutDataArrayPtr:
 *      Upon sucessful return, this parameter will contain a pointer to
 *      an array of the requested values if the ATSULineRef passed in
 *      iLineRef references those values. If this is not the case, then
 *      NULL will be returned, unless iCreate is set to true and the
 *      array can be created. This parameter itself may be set to NULL
 *      if only a count of the entries is needed.
 *    
 *    oLayoutDataCount:
 *      Upon sucessful return, this parameter will contain a count of
 *      the entries in the array returned in oLayoutDataArray.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUDirectGetLayoutDataArrayPtrFromLineRef(
  ATSULineRef              iLineRef,
  ATSUDirectDataSelector   iDataSelector,
  Boolean                  iCreate,
  void *                   oLayoutDataArrayPtr[],     /* can be NULL */
  ItemCount *              oLayoutDataCount);


/* ---------------------------------------------------------------------------- */
/*
 *  ATSUDirectGetLayoutDataArrayPtrFromTextLayout()
 *  
 *  Summary:
 *    Returns the data pointer specified by iDataSelector and
 *    referenced by iTextLayout for the line starting at iLineOffset.
 *  
 *  Discussion:
 *    This function simply returns the data pointer specified by
 *    iDataSelector and referenced by iTextLayout for the line starting
 *    at iLineOffset. This data pointer should not be freed directly
 *    after it's been used. Rather, it should be released using
 *    ATSUDirectReleaseLayoutDataArrayPtr. Doing so serves as a signal
 *    to ATSUI that the caller is done with the data. Furthermore, it
 *    may be the case that the pointer returned may be dynamically
 *    allocated one or contain dynamically allocated data. If it's not
 *    properly freed, a memory leak may result. This function may not
 *    be called inside the context of an
 *    ATSUDirectLayoutOperationOverrideUPP callback for the
 *    ATSUTextLayout data that triggered the callback.  All data
 *    returned will be a copy of the data in the object requested. This
 *    means two things: first of all, this means that it's a very
 *    inefficient way of using the data. All of the selectors that
 *    would have returned in constant time now would be forced to
 *    return in order-n time. Second of all, this means that the
 *    developer cannot change any of the data. Any changes the
 *    developer makes to the arrays returned by this API will have no
 *    effect on the layout. Using the 
 *    kATSULayoutOperationPostLayoutAdjustment operation selector
 *    override and the ATSUDirectGetLayoutDataArrayPtrFromLineRef is a
 *    great alternative to using this API. Many of the requested arrays
 *    are created by ATSUI only when necessary. This means that it's
 *    possible that this API will return NULL pointer and a count of 0.
 *    In this case, if there's no error returned, the array simply
 *    doesn't exist and the caller should treat all of the entries in
 *    the array that they would have recieved as being 0.
 *  
 *  Parameters:
 *    
 *    iTextLayout:
 *      The ATSUTextLayout object from which the requested data will
 *      come from.
 *    
 *    iLineOffset:
 *      The edge offset that corresponds to the beginning of the range
 *      of text of the line of the requested data. If the text has
 *      multiple lines, then ATSUDirectGetLayoutDataArrayPtrFromLineRef
 *      will need to be called for each of the lines in which the
 *      requested data is needed.
 *    
 *    iDataSelector:
 *      The selector for the data that is being requested.
 *    
 *    oLayoutDataArrayPtr:
 *      Upon sucessful return, this parameter will contain a pointer to
 *      an array of the requested values if the ATSUTextLayout passed
 *      in iTextLayout references those values for the line offset
 *      iLineOffset. If this is not the case, then NULL will be
 *      returned. This parameter itself may be set to NULL if only a
 *      count of the entries is needed.
 *    
 *    oLayoutDataCount:
 *      Upon sucessful return, this parameter will contain a count of
 *      the entries in the array returned in oLayoutDataArray.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUDirectGetLayoutDataArrayPtrFromTextLayout(
  ATSUTextLayout           iTextLayout,
  UniCharArrayOffset       iLineOffset,
  ATSUDirectDataSelector   iDataSelector,
  void *                   oLayoutDataArrayPtr[],     /* can be NULL */
  ItemCount *              oLayoutDataCount);


/* ---------------------------------------------------------------------------- */
/*
 *  ATSUDirectReleaseLayoutDataArrayPtr()
 *  
 *  Summary:
 *    Properly releases of an array pointer returned by
 *    ATSUDirectGetLayoutDataArrayPtrFromLineRef() or
 *    ATSUDirectGetLayoutDataArrayPtrFromTextLayout.
 *  
 *  Discussion:
 *    This function is needed to let ATSUI know that the caller is
 *    finished with the pointer that was previously requested by
 *    ATSUDirectGetLayoutDataArrayPtrFromLineRef() or
 *    ATSUDirectGetLayoutDataArrayPtrFromTextLayout(). This is needed
 *    in case ATSUI needs to make any internal adjustments to it's
 *    internal structures.
 *  
 *  Parameters:
 *    
 *    iLineRef:
 *      The lineRef from which the layout data array pointer came from.
 *      If the layout data array pointer did not come from a lineRef,
 *      then set this to NULL.
 *    
 *    iDataSelector:
 *      The selector for which iLayoutDataArrayPtr was obtained.
 *    
 *    iLayoutDataArrayPtr:
 *      A pointer to the layout data array which is to be disposed of.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUDirectReleaseLayoutDataArrayPtr(
  ATSULineRef              iLineRef,                  /* can be NULL */
  ATSUDirectDataSelector   iDataSelector,
  void *                   iLayoutDataArrayPtr[]);


/* ---------------------------------------------------------------------------- */
/*
 *  ATSUDirectAddStyleSettingRef()
 *  
 *  Summary:
 *    This function will fetch a style index for the
 *    ATSUStyleSettingRef passed in.
 *  
 *  Discussion:
 *    This function allows for glyph replacement or substitution from
 *    one layout or line to another layout or line. Not only will it
 *    look up the style index for iStyleSettingRef, but if the
 *    ATSUStyleSettingRef passed in iStyleSettingRef is not yet part of
 *    the line referenced by iLineRef, it will add it. If there is an
 *    outstanding ATSUStyleSettingRef array obtained by using the
 *    kATSUDirectDataStyleSettingATSUStyleSettingRefArray selector, the
 *    pointer obtained for this may no longer be valid after this
 *    function has been called. These pointers should be freed before
 *    calling this function and re-obtained afterwards.
 *  
 *  Parameters:
 *    
 *    iLineRef:
 *      An ATSULineRef which was passed into a
 *      ATSUDirectLayoutOperationOverrideUPP callback function as a
 *      parameter.
 *    
 *    iStyleSettingRef:
 *      The ATSUStyleSettingRef to be looked up or added to the
 *      ATSUTextLayout referenced by iTextLayout for the line starting
 *      at the offset iLineOffset.
 *    
 *    oStyleIndex:
 *      Upon sucessful return, this will parameter will be set to the
 *      index of the ATSUStyleSettingRef passed in iStyleSettingRef for
 *      the line referenced by iLineRef. If the ATSUStyleSettingRef
 *      does not exist, in that context, then it will be added and the
 *      new index will be returned here.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSUDirectAddStyleSettingRef(
  ATSULineRef           iLineRef,
  ATSUStyleSettingRef   iStyleSettingRef,
  UInt16 *              oStyleIndex);




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ATSUNICODEDIRECTACCESS__ */

