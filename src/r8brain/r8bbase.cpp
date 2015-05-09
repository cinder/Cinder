/**
 * @file r8bbase.cpp
 *
 * @brief C++ file that should be compiled and included into your application.
 *
 * This is a single library file that should be compiled and included into the
 * project that uses the "r8brain-free-src" sample rate converter. This file
 * defines several global static objects used by the library.
 *
 * You may also need to include to your project: the "Kernel32" library
 * (on Windows) and the "pthread" library on Mac OS X and Linux.
 *
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * See the "License.txt" file for license.
 */

#include "CDSPFIRFilter.h"
#include "CDSPFracInterpolator.h"

namespace r8b {

CSyncObject CDSPRealFFTKeeper :: StateSync;
CDSPRealFFT :: CObjKeeper CDSPRealFFTKeeper :: FFTObjects[ 31 ];
CSyncObject CDSPFIRFilterCache :: StateSync;
CPtrKeeper< CDSPFIRFilter* > CDSPFIRFilterCache :: Filters;
int CDSPFIRFilterCache :: FilterCount = 0;

} // namespace r8b
