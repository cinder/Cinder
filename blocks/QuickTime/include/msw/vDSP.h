/*
     File:       vDSP.h
 
     Contains:   AltiVec DSP Interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __VDSP__
#define __VDSP__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

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

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

struct DSPComplex {
  float               real;
  float               imag;
};
typedef struct DSPComplex               DSPComplex;
struct DSPSplitComplex {
  float *             realp;
  float *             imagp;
};
typedef struct DSPSplitComplex          DSPSplitComplex;
struct DSPDoubleComplex {
  double              real;
  double              imag;
};
typedef struct DSPDoubleComplex         DSPDoubleComplex;
struct DSPDoubleSplitComplex {
  double *            realp;
  double *            imagp;
};
typedef struct DSPDoubleSplitComplex    DSPDoubleSplitComplex;
typedef struct OpaqueFFTSetup*          FFTSetup;
typedef struct OpaqueFFTSetupD*         FFTSetupD;
typedef SInt32 FFTDirection;
enum {
  kFFTDirection_Forward         = 1,
  kFFTDirection_Inverse         = -1
};

typedef SInt32 FFTRadix;
enum {
  kFFTRadix2                    = 0,
  kFFTRadix3                    = 1,
  kFFTRadix5                    = 2
};

/*
-------------------------------------------------------------------------------
    The criteria to invoke the PowerPC vector implementation is subject to     
    change and become less restrictive in the future.                          
-------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------
    Functions create_fftsetup and destroy_fftsetup.
              create_fftsetupD and destroy_fftsetupD.
              
    create_fftsetup will allocate memory and setup a weight array used by      
    the FFT. The call destroy_fftsetup will free the array.                    
-------------------------------------------------------------------------------
*/
/*
 *  create_fftsetup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( FFTSetup )
create_fftsetup(
  UInt32     log2n,
  FFTRadix   radix);


/*
 *  destroy_fftsetup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
destroy_fftsetup(FFTSetup setup);


/*
 *  create_fftsetupD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( FFTSetupD )
create_fftsetupD(
  UInt32     log2n,
  FFTRadix   radix);


/*
 *  destroy_fftsetupD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
destroy_fftsetupD(FFTSetupD setup);



/*
-------------------------------------------------------------------------------
    Functions ctoz and ztoc.
              ctozD and ztocD.
    
    ctoz converts a complex array to a complex-split array
    ztoc converts a complex-split array to a complex array
    
    Criteria to invoke PowerPC vector code:    
        1. size > 3
        2. strideC = 2
        3. strideZ = 1
        4. C is 16-byte aligned and Z.realp and Z.imagp are 16-byte aligned.
-------------------------------------------------------------------------------
*/
/*
 *  ctoz()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
ctoz(
  const DSPComplex   C[],
  SInt32             strideC,
  DSPSplitComplex *  Z,
  SInt32             strideZ,
  UInt32             size);


/*
 *  ztoc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
ztoc(
  const DSPSplitComplex *  Z,
  SInt32                   strideZ,
  DSPComplex               C[],
  SInt32                   strideC,
  UInt32                   size);


/*
 *  ctozD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
ctozD(
  const DSPDoubleComplex   C[],
  SInt32                   strideC,
  DSPDoubleSplitComplex *  Z,
  SInt32                   strideZ,
  UInt32                   size);


/*
 *  ztocD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
ztocD(
  const DSPDoubleSplitComplex *  Z,
  SInt32                         strideZ,
  DSPDoubleComplex               C[],
  SInt32                         strideC,
  UInt32                         size);



/*
-------------------------------------------------------------------------------
    Functions fft_zip and fft_zipt 
              fft_zipD and fft_ziptD
              
    In-place Split Complex Fourier Transform with or without temporary memory.
            
      Criteria to invoke PowerPC vector code:    
        
        1. ioData.realp and ioData.imagp must be 16-byte aligned.
        2. stride = 1
        3. 2 <= log2n <= 20
        4. bufferTemp.realp and bufferTemp.imagp must be 16-byte aligned.
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  Direction can be either
      kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft_zip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zip(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             stride,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zipt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zipt(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             stride,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zipD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zipD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   stride,
  UInt32                   log2n,
  FFTDirection             direction);


/*
 *  fft_ziptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_ziptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   stride,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2n,
  FFTDirection             direction);



/*
-------------------------------------------------------------------------------
     Functions fft_zop and fft_zopt
               fft_zopD and fft_zoptD
     
     Out-of-place Split Complex Fourier Transform with or without temporary
     memory
            
      Criteria to invoke PowerPC vector code:  
        
        1. signal.realp and signal.imagp must be 16-byte aligned.
        2. signalStride = 1
        3. result.realp and result.imagp must be 16-byte aligned.
        4. strideResult = 1
        5. 2 <= log2n <= 20
        6. bufferTemp.realp and bufferTemp.imagp must be 16-byte aligned.
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  Direction can be either
      kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft_zop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zopt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zopt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zopD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zopD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   log2n,
  FFTDirection             direction);


/*
 *  fft_zoptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zoptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2n,
  FFTDirection             direction);



/*
-------------------------------------------------------------------------------
    Functions fft_zrip and fft_zript
              fft_zripD and fft_zriptD
              
    In-Place Real Fourier Transform with or without temporary memory,
    split Complex Format
            
      Criteria to invoke PowerPC vector code:    
        1. ioData.realp and ioData.imagp must be 16-byte aligned.
        2. stride = 1
        3. 3 <= log2n <= 13
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  Direction can be either
      kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft_zrip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zrip(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             stride,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zript()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zript(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             stride,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zripD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zripD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   stride,
  UInt32                   log2n,
  FFTDirection             flag);


/*
 *  fft_zriptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zriptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   stride,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2n,
  SInt32                   flag);



/*
-------------------------------------------------------------------------------
    Functions fft_zrop and fft_zropt
              fft_zropD and fft_zroptD
              
    Out-of-Place Real Fourier Transform with or without temporary memory,
    split Complex Format
            
      Criteria to invoke PowerPC vector code:  
        1. signal.realp and signal.imagp must be 16-byte aligned.
        2. signalStride = 1
        3. result.realp and result.imagp must be be 16-byte aligned.
        4. strideResult = 1
        5. 3 <= log2n <= 13
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  Direction can be either
      kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft_zrop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zrop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zropt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft_zropt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2n,
  FFTDirection       direction);


/*
 *  fft_zropD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zropD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   log2n,
  SInt32                   flag);


/*
 *  fft_zroptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft_zroptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2n,
  SInt32                   flag);



/*
-------------------------------------------------------------------------------
    Functions fft2d_zip and fft2d_zipt
              fft2d_zipD and fft2d_ziptD
              
    In-place two dimensional Split Complex Fourier Transform with or without
    temporary memory
            
      Criteria to invoke PowerPC vector code:  
        1. ioData.realp and ioData.imagp must be 16-byte aligned.
        2. strideInRow = 1;
        3. strideInCol must be a multiple of 4
        4. 2 <= log2nInRow <= 12
        5. 2 <= log2nInCol <= 12
        6. bufferTemp.realp and bufferTemp.imagp must be 16-byte aligned.
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  ( log2n = log2nInRow + log2nInCol ) 
      Direction can be either kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft2d_zip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zip(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             strideInRow,
  SInt32             strideInCol,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  FFTDirection       direction);


/*
 *  fft2d_zipt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zipt(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             strideInRow,
  SInt32             strideInCol,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  FFTDirection       direction);


/*
 *  fft2d_zipD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zipD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   strideInRow,
  SInt32                   strideInCol,
  UInt32                   log2nInCol,
  UInt32                   log2nInRow,
  FFTDirection             direction);


/*
 *  fft2d_ziptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_ziptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   strideInRow,
  SInt32                   strideInCol,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2nInCol,
  UInt32                   log2nInRow,
  FFTDirection             direction);



/*
-------------------------------------------------------------------------------
    Functions fft2d_zop and fft2d_zopt
              fft2d_zopD and fft2d_zoptD
              
    Out-of-Place two dimemsional Split Complex Fourier Transform with or
    without temporary memory
            
      Criteria to invoke PowerPC vector code:  
        
        1. signal.realp and signal.imagp must be 16-byte aligned.
        2. signalStrideInRow = 1;
        3. signalStrideInCol must be a multiple of 4
        4. result.realp and result.imagp must be 16-byte aligned.
        5. strideResultInRow = 1;
        6. strideResultInCol must be a multiple of 4
        7. 2 <= log2nInRow <= 12
        8. 2 <= log2nInCol <= 12
        9. bufferTemp.realp and bufferTemp.imagp must be 16-byte aligned.

      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  ( log2n = log2nInRow + log2nInCol ) 
      Direction can be either kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft2d_zop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStrideInRow,
  SInt32             signalStrideInCol,
  DSPSplitComplex *  result,
  SInt32             strideResultInRow,
  SInt32             strideResultInCol,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  SInt32             flag);


/*
 *  fft2d_zopt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zopt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStrideInRow,
  SInt32             signalStrideInCol,
  DSPSplitComplex *  result,
  SInt32             strideResultInRow,
  SInt32             strideResultInCol,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  SInt32             flag);


/*
 *  fft2d_zopD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zopD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStrideInRow,
  SInt32                   signalStrideInCol,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResultInRow,
  SInt32                   strideResultInCol,
  UInt32                   log2nInCol,
  UInt32                   log2nInRow,
  SInt32                   flag);


/*
 *  fft2d_zoptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zoptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStrideInRow,
  SInt32                   signalStrideInCol,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResultInRow,
  SInt32                   strideResultInCol,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2nInCol,
  UInt32                   log2nInRow,
  SInt32                   flag);



/*
-------------------------------------------------------------------------------
    Functions fft2d_zrip and fft2d_zript
              fft2d_zripD and fft2d_zriptD
              
    In-place two dimensional Real Fourier Transform with or without temporary
    memory, Split Complex Format
            
      Criteria to invoke PowerPC vector code:  
        1. ioData.realp and ioData.imagp must be 16-byte aligned.
        2. strideInRow = 1;
        3. strideInCol must be a multiple of 4
        4. 3 <= log2nInRow <= 12
        5. 3 <= log2nInCol <= 13
        6. bufferTemp.realp and bufferTemp.imagp must be 16-byte aligned.

      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  ( log2n = log2nInRow + log2nInCol ) 
      Direction can be either kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft2d_zrip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zrip(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             strideInRow,
  SInt32             strideInCol,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  FFTDirection       direction);


/*
 *  fft2d_zript()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zript(
  FFTSetup           setup,
  DSPSplitComplex *  ioData,
  SInt32             strideInRow,
  SInt32             strideInCol,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  FFTDirection       direction);


/*
 *  fft2d_zripD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zripD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   strideInRow,
  SInt32                   strideInCol,
  UInt32                   log2nInCol,
  UInt32                   log2nInRow,
  SInt32                   flag);


/*
 *  fft2d_zriptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zriptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   strideInRow,
  SInt32                   strideInCol,
  DSPDoubleSplitComplex *  bufferTemp,
  UInt32                   log2nInCol,
  UInt32                   log2nInRow,
  SInt32                   flag);



/*
-------------------------------------------------------------------------------
    Functions fft2d_zrop and fft2d_zropt
              fft2d_zropD and fft2d_zroptD
              
    Out-of-Place Two-Dimemsional Real Fourier Transform with or without
    temporary memory, Split Complex Format
            
      Criteria to invoke PowerPC vector code:  
        1. signal.realp and signal.imagp must be 16-byte aligned.
        2. signalStrideInRow = 1;
        3. signalStrideInCol must be a multiple of 4
        4. result.realp and result.imagp must be 16-byte aligned.
        5. strideResultInRow = 1;
        6. strideResultInCol must be a multiple of 4
        7. 3 <= log2nInRow <= 12
        8. 3 <= log2nInCol <= 13
        9. bufferTemp.realp and bufferTemp.imagp must be 16-byte aligned.

      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The size of temporary memory for each part
      is the lower value of 4*n and 16k.  ( log2n = log2nInRow + log2nInCol ) 
      Direction can be either kFFTDirection_Forward or kFFTDirection_Inverse.
-------------------------------------------------------------------------------
*/
/*
 *  fft2d_zrop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zrop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStrideInRow,
  SInt32             signalStrideInCol,
  DSPSplitComplex *  result,
  SInt32             strideResultInRow,
  SInt32             strideResultInCol,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  SInt32             flag);


/*
 *  fft2d_zropt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
fft2d_zropt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStrideInRow,
  SInt32             signalStrideInCol,
  DSPSplitComplex *  result,
  SInt32             strideResultInRow,
  SInt32             strideResultInCol,
  DSPSplitComplex *  bufferTemp,
  UInt32             log2nInCol,
  UInt32             log2nInRow,
  SInt32             flag);


/*
 *  fft2d_zropD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zropD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   Kr,
  SInt32                   Kc,
  DSPDoubleSplitComplex *  ioData2,
  SInt32                   Ir,
  SInt32                   Ic,
  UInt32                   log2nc,
  UInt32                   log2nr,
  SInt32                   flag);


/*
 *  fft2d_zroptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft2d_zroptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   Kr,
  SInt32                   Kc,
  DSPDoubleSplitComplex *  ioData2,
  SInt32                   Ir,
  SInt32                   Ic,
  DSPDoubleSplitComplex *  temp,
  UInt32                   log2nc,
  UInt32                   log2nr,
  SInt32                   flag);



/*
________________________________________________________________________________
      Functions fftm_zip and fftm_zipt
                fftm_zipD and fftm_ziptD
                
      In-Place multiple One_Dimensional Complex Fourier Transform with or 
      without temporary memory, Split Complex Format
      
         Criteria to invoke PowerPC vector code:
            1. signal.realp and signal.imagp must be 16-byte aligned.
            2. signalStride = 1;
            3. fftStride must be a multiple of 4
            4. 2 <= log2n <= 12
            5. temp.realp and temp.imagp must be 16-byte aligned.
         
         If any of the above criteria are not satisfied, the PowerPC scalar code
         implementation will be used.
________________________________________________________________________________
*/
/*
 *  fftm_zip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zip(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zipt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zipt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  DSPSplitComplex *  temp,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zipD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zipD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);


/*
 *  fftm_ziptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_ziptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  DSPDoubleSplitComplex *  temp,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);



/*
________________________________________________________________________________
      Functions fftm_zop and fftm_zopt
                fftm_zopD and fftm_zoptD
                
      Out-Of-Place multiple One_Dimensional Complex Fourier Transform with or 
      without temporary memory, Split Complex Format
      
         Criteria to invoke PowerPC vector code:
            1. signal.realp and signal.imagp must be 16-byte aligned.
            2. signalStride = 1;
            3. fftStride must be a multiple of 4
            4. result.realp and result.imagp must be 16-byte aligned.
            5. resultStride = 1;
            6. rfftStride must be a multiple of 4
            7. 2 <= log2n <= 12
            8. temp.realp and temp.imagp must be 16-byte aligned.
        
        If any of the above criteria are not satisfied, the PowerPC scalar code
        implementation will be used.
________________________________________________________________________________
*/
/*
 *  fftm_zop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  DSPSplitComplex *  result,
  SInt32             resultStride,
  SInt32             rfftStride,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zopt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zopt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  DSPSplitComplex *  result,
  SInt32             resultStride,
  SInt32             rfftStride,
  DSPSplitComplex *  temp,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zopD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zopD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   resultStride,
  SInt32                   rfftStride,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);


/*
 *  fftm_zoptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zoptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   resultStride,
  SInt32                   rfftStride,
  DSPDoubleSplitComplex *  temp,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);



/*
________________________________________________________________________________
      Functions fftm_zrip and fftm_zript
                fftm_zripD and fftm_zriptD
                
      In-Place multiple One_Dimensional Real Fourier Transform with or 
      without temporary memory, Split Complex Format
      
         Criteria to invoke PowerPC vector code:
            1. signal.realp and signal.imagp must be 16-byte aligned.
            2. signalStride = 1;
            3. fftStride must be a multiple of 4
            4. 3 <= log2n <= 13
            5. temp.realp and temp.imagp must be 16-byte aligned.
        If any of the above criteria are not satisfied, the PowerPC scalar code
        implementation will be used.
________________________________________________________________________________
*/
/*
 *  fftm_zrip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zrip(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zript()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zript(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  DSPSplitComplex *  temp,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zripD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zripD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);


/*
 *  fftm_zriptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zriptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  DSPDoubleSplitComplex *  temp,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);



/*
________________________________________________________________________________
      Functions fftm_zrop and fftm_zropt
                fftm_zropD and fftm_zroptD
                
      Out-Of-Place multiple One_Dimensional Real Fourier Transform with or 
      without temporary memory, Split Complex Format
      
         Criteria to invoke PowerPC vector code:
            1. signal.realp and signal.imagp must be 16-byte aligned.
            2. signalStride = 1;
            3. fftStride must be a multiple of 4
            4. result.realp and result.imagp must be 16-byte aligned.
            5. resultStride = 1;
            6. rfftStride must be a multiple of 4
            7. 3 <= log2n <= 13
            8. temp.realp and temp.imagp must be 16-byte aligned.
         
         If any of the above criteria are not satisfied, the PowerPC scalar code
         implementation will be used.
________________________________________________________________________________
*/
/*
 *  fftm_zrop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zrop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  DSPSplitComplex *  result,
  SInt32             resultStride,
  SInt32             rfftStride,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zropt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zropt(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  SInt32             fftStride,
  DSPSplitComplex *  result,
  SInt32             resultStride,
  SInt32             rfftStride,
  DSPSplitComplex *  temp,
  UInt32             log2n,
  UInt32             numFFT,
  SInt32             flag);


/*
 *  fftm_zropD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zropD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   resultStride,
  SInt32                   rfftStride,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);


/*
 *  fftm_zroptD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fftm_zroptD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  SInt32                   fftStride,
  DSPDoubleSplitComplex *  result,
  SInt32                   resultStride,
  SInt32                   rfftStride,
  DSPDoubleSplitComplex *  temp,
  UInt32                   log2n,
  UInt32                   numFFT,
  SInt32                   flag);



/*
________________________________________________________________________________
      Functions fft3_zop and fft5_zop
                fft3_zopD and fft5_zopD
                
      Out-Of-Place One_Dimensional Complex Fourier Transform in base-3 and 
      base-5 with or without temporary memory, Split Complex Format 
      
         Criteria to invoke PowerPC vector code:
            1. signal.realp and signal.imagp must be 16-byte aligned.
            2. signalStride = 1;
            3. result.realp and result.imagp must be 16-byte aligned.
            4. resultStride = 1;
            5. 3 <= log2n 
         
         If any of the above criteria are not satisfied, the PowerPC scalar code
         implementation will be used.
________________________________________________________________________________
*/
/*
 *  fft3_zop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft3_zop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  result,
  SInt32             resultStride,
  UInt32             log2n,
  SInt32             flag);


/*
 *  fft5_zop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft5_zop(
  FFTSetup           setup,
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  result,
  SInt32             resultStride,
  UInt32             log2n,
  SInt32             flag);


/*
 *  fft3_zopD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft3_zopD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   K,
  DSPDoubleSplitComplex *  ioData2,
  SInt32                   L,
  UInt32                   log2n,
  SInt32                   flag);


/*
 *  fft5_zopD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
fft5_zopD(
  FFTSetupD                setup,
  DSPDoubleSplitComplex *  ioData,
  SInt32                   K,
  DSPDoubleSplitComplex *  ioData2,
  SInt32                   L,
  UInt32                   log2n,
  SInt32                   flag);



/*
-------------------------------------------------------------------------------
    Function conv
             convD
             
    Floating Point Convolution and Correlation in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. signal and result must have relative alignement.
        2. 4 <= lenFilter <= 256
        3. lenResult > 36
        4. signalStride = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  strideFilter can be positive for
      correlation or negative for convolution.
-------------------------------------------------------------------------------
*/
/*
 *  conv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
conv(
  const float   signal[],
  SInt32        signalStride,
  const float   filter[],
  SInt32        strideFilter,
  float         result[],
  SInt32        strideResult,
  SInt32        lenResult,
  SInt32        lenFilter);


/*
 *  convD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
convD(
  const double   signal[],
  SInt32         signalStride,
  const double   filter[],
  SInt32         strideFilter,
  double         result[],
  SInt32         strideResult,
  SInt32         lenResult,
  SInt32         lenFilter);


/*
_______________________________________________________________________________
     Functions f3x3, f5x5, and imgfir
               f3x3D, f5x5D and imgfirD
               
     Filter, 3x3, 5x5, MxN Single and Double Precision Convolution
     
       Criteria to invoke PowerPC vector code:
         1. signal, filter, and result must have relative alignment and
            be 16-byte aligned.
         2. for f3x3, NC >= 18
         3. for f5x5, NC >= 20
         4. for imgfir, NC >= 20
       
       If any of the above criteria are not satisfied, the PowerPC scalar code
       implementation will be used.
_______________________________________________________________________________
*/
/*
 *  f3x3()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
f3x3(
  float *  signal,
  SInt32   rowStride,
  SInt32   colStride,
  float *  filter,
  float *  result);


/*
 *  f3x3D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
f3x3D(
  double *  signal,
  SInt32    rowStride,
  SInt32    colStride,
  double *  filter,
  double *  result);


/*
 *  f5x5()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
f5x5(
  float *  signal,
  SInt32   rowStride,
  SInt32   colStride,
  float *  filter,
  float *  result);


/*
 *  f5x5D()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
f5x5D(
  double *  signal,
  SInt32    rowStride,
  SInt32    colStride,
  double *  filter,
  double *  result);


/*
 *  imgfir()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
imgfir(
  float *  signal,
  SInt32   numRow,
  SInt32   numCol,
  float *  filter,
  float *  result,
  SInt32   fnumRow,
  SInt32   fnumCol);


/*
 *  imgfirD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
imgfirD(
  double *  signal,
  SInt32    numRow,
  SInt32    numCol,
  double *  filter,
  double *  result,
  SInt32    fnumRow,
  SInt32    fnumCol);



/*
_______________________________________________________________________________
     Function mtrans
              mtransD
              
     Single and Double Precision Matrix Transpose
     
       Criteria to invoke PowerPC vector code:
         1. a = c
         2. a and c must be 16-byte aligned.
         3. M must be a multiple of 8.
       
       If any of the above criteria are not satisfied, the PowerPC scalar code
       implementation will be used.
_______________________________________________________________________________
*/
/*
 *  mtrans()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
mtrans(
  float *  a,
  SInt32   aStride,
  float *  c,
  SInt32   cStride,
  SInt32   M,
  SInt32   N);


/*
 *  mtransD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
mtransD(
  double *  a,
  SInt32    aStride,
  double *  c,
  SInt32    cStride,
  SInt32    M,
  SInt32    N);


/*
_______________________________________________________________________________
      Function mmul
               mmulD
               
      Single and Double Precision Matrix Multiply
      
        Criteria to invoke PowerPC vector code:
          1. a, b, c must be 16-byte aligned.
          2. M >= 8.
          3. N >= 32.
          4. P is a multiple of 8.
        
        If any of the above criteria are not satisfied, the PowerPC scalar code
        implementation will be used.
_______________________________________________________________________________
*/
/*
 *  mmul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
mmul(
  float *  a,
  SInt32   aStride,
  float *  b,
  SInt32   bStride,
  float *  c,
  SInt32   cStride,
  SInt32   M,
  SInt32   N,
  SInt32   P);


/*
 *  mmulD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
mmulD(
  double *  a,
  SInt32    aStride,
  double *  b,
  SInt32    bStride,
  double *  c,
  SInt32    cStride,
  SInt32    M,
  SInt32    N,
  SInt32    P);


/*
_______________________________________________________________________________
      Function zmma, zmms, zmsm, and zmmul
               zmmaD, zmmsD, zmsmD, and zmmulD
               
      Single and Double Precision Complex Split Matrix mul/add, mul/sub, sub/mul, 
                                                       and mul
      
        Criteria to invoke PowerPC vector code:
          1. a, b, c, and d must be 16-byte aligned.
          2. N is a multiple of 4.
          3. P is a multiple of 4.
          4. I, J, K, L = 1;
          
       If any of the above criteria are not satisfied, the PowerPC scalar code
       implementation will be used.
_______________________________________________________________________________
*/
/*
 *  zmma()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmma(
  DSPSplitComplex *  a,
  SInt32             i,
  DSPSplitComplex *  b,
  SInt32             j,
  DSPSplitComplex *  c,
  SInt32             k,
  DSPSplitComplex *  d,
  SInt32             l,
  SInt32             M,
  SInt32             N,
  SInt32             P);


/*
 *  zmmaD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmmaD(
  DSPDoubleSplitComplex *  a,
  SInt32                   i,
  DSPDoubleSplitComplex *  b,
  SInt32                   j,
  DSPDoubleSplitComplex *  c,
  SInt32                   k,
  DSPDoubleSplitComplex *  d,
  SInt32                   l,
  SInt32                   M,
  SInt32                   N,
  SInt32                   P);


/*
 *  zmms()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmms(
  DSPSplitComplex *  a,
  SInt32             i,
  DSPSplitComplex *  b,
  SInt32             j,
  DSPSplitComplex *  c,
  SInt32             k,
  DSPSplitComplex *  d,
  SInt32             l,
  SInt32             M,
  SInt32             N,
  SInt32             P);


/*
 *  zmmsD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmmsD(
  DSPDoubleSplitComplex *  a,
  SInt32                   i,
  DSPDoubleSplitComplex *  b,
  SInt32                   j,
  DSPDoubleSplitComplex *  c,
  SInt32                   k,
  DSPDoubleSplitComplex *  d,
  SInt32                   l,
  SInt32                   M,
  SInt32                   N,
  SInt32                   P);


/*
 *  zmsm()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmsm(
  DSPSplitComplex *  a,
  SInt32             i,
  DSPSplitComplex *  b,
  SInt32             j,
  DSPSplitComplex *  c,
  SInt32             k,
  DSPSplitComplex *  d,
  SInt32             l,
  SInt32             M,
  SInt32             N,
  SInt32             P);


/*
 *  zmsmD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmsmD(
  DSPDoubleSplitComplex *  a,
  SInt32                   i,
  DSPDoubleSplitComplex *  b,
  SInt32                   j,
  DSPDoubleSplitComplex *  c,
  SInt32                   k,
  DSPDoubleSplitComplex *  d,
  SInt32                   l,
  SInt32                   M,
  SInt32                   N,
  SInt32                   P);


/*
 *  zmmul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmmul(
  DSPSplitComplex *  a,
  SInt32             i,
  DSPSplitComplex *  b,
  SInt32             j,
  DSPSplitComplex *  c,
  SInt32             k,
  SInt32             M,
  SInt32             N,
  SInt32             P);


/*
 *  zmmulD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zmmulD(
  DSPDoubleSplitComplex *  a,
  SInt32                   i,
  DSPDoubleSplitComplex *  b,
  SInt32                   j,
  DSPDoubleSplitComplex *  c,
  SInt32                   k,
  SInt32                   M,
  SInt32                   N,
  SInt32                   P);


/*
-------------------------------------------------------------------------------
    Function vadd
             vaddD
    
    Floating Point Add in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1 and input2 and result are all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vadd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vadd(
  const float   input1[],
  SInt32        stride1,
  const float   input2[],
  SInt32        stride2,
  float         result[],
  SInt32        strideResult,
  UInt32        size);


/*
 *  vaddD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vaddD(
  const double   input1[],
  SInt32         stride1,
  const double   input2[],
  SInt32         stride2,
  double         result[],
  SInt32         strideResult,
  UInt32         size);


/*
-------------------------------------------------------------------------------
    Function vsub
             vsubD
             
     Floating Point Substract in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input1 and input2 and result are all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vsub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vsub(
  const float   input1[],
  SInt32        stride1,
  const float   input2[],
  SInt32        stride2,
  float         result[],
  SInt32        strideResult,
  UInt32        size);


/*
 *  vsubD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vsubD(
  const double   input1[],
  SInt32         stride1,
  const double   input2[],
  SInt32         stride2,
  double         result[],
  SInt32         strideResult,
  UInt32         size);


/*
-------------------------------------------------------------------------------
    Function vmul
             vmulD
             
    Floating Point Multiply in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1 and input2 and result must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vmul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vmul(
  const float   input1[],
  SInt32        stride1,
  const float   input2[],
  SInt32        stride2,
  float         result[],
  SInt32        strideResult,
  UInt32        size);


/*
 *  vmulD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vmulD(
  const double   input1[],
  SInt32         stride1,
  const double   input2[],
  SInt32         stride2,
  double         result[],
  SInt32         strideResult,
  UInt32         size);


/*
-------------------------------------------------------------------------------
    Function vsmul
             vsmulD
    
    Floating Point - Scalar Multiply in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1 and result are all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vsmul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vsmul(
  const float    input1[],
  SInt32         stride1,
  const float *  input2,
  float          result[],
  SInt32         strideResult,
  UInt32         size);


/*
 *  vsmulD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vsmulD(
  const double    input1[],
  SInt32          stride1,
  const double *  input2,
  double          result[],
  SInt32          strideResult,
  UInt32          size);



/*
-------------------------------------------------------------------------------
    Function vsq
             vsqD
    
    Floating Point Square in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input and result are relatively aligned.
        2. size >= 8
        3. strideInput = 1
        4. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vsq()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vsq(
  const float   input[],
  SInt32        strideInput,
  float         result[],
  SInt32        strideResult,
  UInt32        size);


/*
 *  vsqD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vsqD(
  const double   input[],
  SInt32         strideInput,
  double         result[],
  SInt32         strideResult,
  UInt32         size);



/*
-------------------------------------------------------------------------------
    Function vssq
             vssqD
             
    Floating Point Signed Square in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input and result must be all relatively aligned.
        2. size >= 8
        3. strideInput = 1
        4. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vssq()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vssq(
  const float   input[],
  SInt32        strideInput,
  float         result[],
  SInt32        strideResult,
  UInt32        size);


/*
 *  vssqD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vssqD(
  const double   input[],
  SInt32         strideInput,
  double         result[],
  SInt32         strideResult,
  UInt32         size);


/*
-------------------------------------------------------------------------------
    Function dotpr
             dotprD
    
    Floating Point Dot product in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1 and input2 are relatively aligned.
        2. size >= 20
        3. stride1 = 1
        4. stride2 = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  dotpr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
dotpr(
  const float   input1[],
  SInt32        stride1,
  const float   input2[],
  SInt32        stride2,
  float *       result,
  UInt32        size);


/*
 *  dotprD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
dotprD(
  const double   input1[],
  SInt32         stride1,
  const double   input2[],
  SInt32         stride2,
  double *       result,
  UInt32         size);


/*
-------------------------------------------------------------------------------
    Function vam
             vamD
             
    Floating Point vadd and Multiply in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1, input2, input_3 and result are all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. stride_3 = 1
        6. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  vam()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vam(
  const float   input1[],
  SInt32        stride1,
  const float   input2[],
  SInt32        stride2,
  const float   input3[],
  SInt32        stride3,
  float         result[],
  SInt32        strideResult,
  UInt32        size);


/*
 *  vamD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
vamD(
  const double   input1[],
  SInt32         stride1,
  const double   input2[],
  SInt32         stride2,
  const double   input3[],
  SInt32         stride3,
  double         result[],
  SInt32         strideResult,
  UInt32         size);


/*
-------------------------------------------------------------------------------
    Function zconv
             zconvD
                
    Split Complex Convolution and Correlation in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. signal->realp, signal->imagp, result->realp, result->imagp
           must be relatively aligned.
        2. 4 <= lenFilter <= 128
        3. lenResult > 20
        4. signalStride = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  strideFilter can be positive for correlation
      or negative for convolution
-------------------------------------------------------------------------------
*/
/*
 *  zconv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zconv(
  DSPSplitComplex *  signal,
  SInt32             signalStride,
  DSPSplitComplex *  filter,
  SInt32             strideFilter,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  SInt32             lenResult,
  SInt32             lenFilter);


/*
 *  zconvD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zconvD(
  DSPDoubleSplitComplex *  signal,
  SInt32                   signalStride,
  DSPDoubleSplitComplex *  filter,
  SInt32                   strideFilter,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  SInt32                   lenResult,
  SInt32                   lenFilter);


/*
-------------------------------------------------------------------------------
    Function zvadd
             zvaddD
    
    Split Complex vadd in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2.realp, input2.imagp,
           result.realp, result.imagp must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zvadd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zvadd(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  DSPSplitComplex *  input2,
  SInt32             stride2,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size);


/*
 *  zvaddD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zvaddD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  DSPDoubleSplitComplex *  input2,
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zvsub
             zvsubD
             
    Split Complex Substract in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2.realp, input2.imagp,
           result.realp, result.imagp must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zvsub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zvsub(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  DSPSplitComplex *  input2,
  SInt32             stride2,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size);


/*
 *  zvsubD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zvsubD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  DSPDoubleSplitComplex *  input2,
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zvmul
             zvmulD
             
    Split Complex Multiply in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2.realp, input2.imagp,
           result.realp, result.imagp must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1

      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.  The conjugate value can be 1 or -1.
-------------------------------------------------------------------------------
*/
/*
 *  zvmul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zvmul(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  DSPSplitComplex *  input2,
  SInt32             stride2,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size,
  SInt32             conjugate);


/*
 *  zvmulD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zvmulD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  DSPDoubleSplitComplex *  input2,
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size,
  SInt32                   conjugate);


/*
-------------------------------------------------------------------------------
    Function zdotpr
             zdotprD
             
    Split Complex Dot product in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2.realp, input2.imagp are all
           relatively aligned.
        2. size >= 20
        3. stride1 = 1
        4. stride2 = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zdotpr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zdotpr(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  DSPSplitComplex *  input2,
  SInt32             stride2,
  DSPSplitComplex *  result,
  UInt32             size);


/*
 *  zdotprD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zdotprD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  DSPDoubleSplitComplex *  input2,
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zidotpr
             zidotprD
             
    Split Complex Inner Dot product in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2.realp, input2.imagp must be
           all relatively aligned.
        2. size >= 20
        3. stride1 = 1
        4. stride2 = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zidotpr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zidotpr(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  DSPSplitComplex *  input2,
  SInt32             stride2,
  DSPSplitComplex *  result,
  UInt32             size);


/*
 *  zidotprD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zidotprD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  DSPDoubleSplitComplex *  input2,
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  UInt32                   size);



/*
-------------------------------------------------------------------------------
    Function zrdotpr
             zrdotprD
                
    Split Complex - Real Dot product in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2 are must be relatively aligned.
        2. size >= 16
        3. stride1 = 1
        4. stride2 = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zrdotpr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zrdotpr(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  const float        input2[],
  SInt32             stride2,
  DSPSplitComplex *  result,
  UInt32             size);


/*
 *  zrdotprD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zrdotprD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  const double             input2[],
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zvcma
             zvcmaD
             
    Split Complex Conjugate Multiply And vadd in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2.realp, input2.imagp,
          input_3.realp, input_3.imagp, result.realp, result.imagp
          must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. stride_3 = 1
        6. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zvcma()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zvcma(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  DSPSplitComplex *  input2,
  SInt32             stride2,
  DSPSplitComplex *  input3,
  SInt32             stride3,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size);


/*
 *  zvcmaD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zvcmaD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  DSPDoubleSplitComplex *  input2,
  SInt32                   stride2,
  DSPDoubleSplitComplex *  input3,
  SInt32                   stride3,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zrvadd
             zrvaddD
             
    Split Complex - Real Add in Single and Double Precision
      
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2, result.realp, result.imagp
           are all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zrvadd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zrvadd(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  const float        input2[],
  SInt32             stride2,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size);


/*
 *  zrvaddD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zrvaddD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  const double             input2[],
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zrvsub
             zrvsubD
                
    Split Complex - Real Substract in Single and Double Precision
    
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2, result.realp, result.imagp
           must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zrvsub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zrvsub(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  const float        input2[],
  SInt32             stride2,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size);


/*
 *  zrvsubD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zrvsubD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  const double             input2[],
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size);


/*
-------------------------------------------------------------------------------
    Function zrvmul
             zrvmulD
             
    Split Complex - Real Multiply
    
      Criteria to invoke PowerPC vector code:  
        1. input1.realp, input1.imagp, input2, result.realp, result.imagp
           must be all relatively aligned.
        2. size >= 8
        3. stride1 = 1
        4. stride2 = 1
        5. strideResult = 1
      
      If any of the above criteria are not satisfied, the PowerPC scalar code
      implementation will be used.
-------------------------------------------------------------------------------
*/
/*
 *  zrvmul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
zrvmul(
  DSPSplitComplex *  input1,
  SInt32             stride1,
  const float        input2[],
  SInt32             stride2,
  DSPSplitComplex *  result,
  SInt32             strideResult,
  UInt32             size);


/*
 *  zrvmulD()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
zrvmulD(
  DSPDoubleSplitComplex *  input1,
  SInt32                   stride1,
  const double             input2[],
  SInt32                   stride2,
  DSPDoubleSplitComplex *  result,
  SInt32                   strideResult,
  UInt32                   size);



#ifndef USE_NON_APPLE_STANDARD_DATATYPES
#define USE_NON_APPLE_STANDARD_DATATYPES 1
#endif  /* !defined(USE_NON_APPLE_STANDARD_DATATYPES) */

#if USE_NON_APPLE_STANDARD_DATATYPES
enum {
  FFT_FORWARD                   = kFFTDirection_Forward,
  FFT_INVERSE                   = kFFTDirection_Inverse
};

enum {
  FFT_RADIX2                    = kFFTRadix2,
  FFT_RADIX3                    = kFFTRadix3,
  FFT_RADIX5                    = kFFTRadix5
};


typedef DSPComplex                      COMPLEX;
typedef DSPSplitComplex                 COMPLEX_SPLIT;
typedef DSPDoubleComplex                DOUBLE_COMPLEX;
typedef DSPDoubleSplitComplex           DOUBLE_COMPLEX_SPLIT;
#endif  /* USE_NON_APPLE_STANDARD_DATATYPES */


#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VDSP__ */

