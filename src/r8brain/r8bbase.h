//$ nobt

/**
 * @file r8bbase.h
 *
 * @brief The "base" inclusion file with basic classes and functions.
 *
 * This is the "base" inclusion file for the "r8brain-free-src" sample rate
 * converter. This inclusion file contains implementations of several small
 * utility classes and functions used by the library.
 *
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * See the "License.txt" file for license.
 *
 * @mainpage
 *
 * @section intro_sec Introduction
 *
 * Open source (under the MIT license) high-quality professional audio sample
 * rate converter (SRC) (resampling) library. Features routines for SRC, both
 * up- and downsampling, to/from any sample rate, including non-integer sample
 * rates: it can be also used for conversion to/from SACD sample rate and even
 * go beyond that. SRC routines were implemented in multi-platform C++ code,
 * and have a high level of optimality.
 *
 * The structure of this library's objects is such that they can be frequently
 * created and destroyed in large applications with a minimal performance
 * impact due to a high level of reusability of its most
 * "initialization-expensive" objects: the fast Fourier transform and FIR
 * filter objects.
 *
 * The SRC algorithm at first produces 2X oversampled (relative to the source
 * sample rate, or the destination sample rate if the downsampling is
 * performed) signal and then performs interpolation using a bank of short
 * (14 to 28 taps, depending on the required precision)
 * polynomial-interpolated sinc-based fractional delay filters. This puts the
 * algorithm into the league of the fastest among the most precise SRC
 * algorithms. The more precise alternative being only the whole
 * number-factored SRC, which can be slower.
 *
 * @section requirements Requirements
 *
 * C++ compiler and system with the "double" floating point type (53-bit
 * mantissa) support. No explicit code for the "float" type is present in this
 * library, because as practice has shown the "float"-based code performs
 * considerably slower on a modern processor, at least in this library.
 * However, if the "double" type really represents the "float" type (24-bit
 * mantissa) in a given compiler, on a given system, the library won't become
 * broken, only the conversion quality may become degraded. This library
 * always uses the "sizeof( double )" operator to obtain "double" floating
 * point type's size in bytes. This library does not have dependencies beside
 * the standard C library, the "windows.h" on Windows and the "pthread.h" on
 * Mac OS X and Linux.
 *
 * @section usage Usage Information
 *
 * The sample rate converter (resampler) is represented by the
 * r8b::CDSPResampler class, which is a single front-end class for the whole
 * library. You do not basically need to use nor understand any other classes
 * beside this class. Several derived classes that have varying levels of
 * precision are also available.
 *
 * The code of the library resides in the "r8b" C++ namespace, effectively
 * isolating it from all other code. The code is thread-safe. A separate
 * resampler object should be created for each audio channel or stream being
 * processed.
 *
 * Note that you will need to compile the "r8bbase.cpp" source file and
 * include the resulting object file into your application build. This source
 * file includes definitions of several global static objects used by the
 * library. You may also need to include to your project: the "Kernel32"
 * library (on Windows) and the "pthread" library on Mac OS X and Linux.
 *
 * The code of this library was commented in the Doxygen style. To generate
 * the documentation locally you may run the "doxygen ./other/r8bdoxy.txt"
 * command from the library's directory.
 *
 * Preliminary tests show that the r8b::CDSPResampler24 resampler class
 * achieves 15.6*n_cores Mflops when converting 1 channel of audio from 44100
 * to 96000 sample rate, on a typical Intel Core i7-4770K processor-based
 * system without overclocking. This approximately translates to a real-time
 * resampling of 160*n_cores audio streams, at 100% CPU load.
 *
 * @section dll Dynamic Link Library
 *
 * The functions of this SRC library are also accessible in simplified form
 * via the DLL file on Windows, requiring a processor with SSE2 support.
 * Delphi Pascal interface unit file for the DLL file is available. DLL and
 * C LIB files are distributed in a separate ZIP file on the project's home
 * page. On non-Windows systems it is preferrable to use the C++ library
 * directly.
 *
 * @section realtime Real-time Applications
 *
 * The resampler class of this library was designed as asynchronous processor:
 * it may produce any number of output samples, depending on the input sample
 * data length and the resampling parameters. The resampler must be fed with
 * the input sample data until enough output sample data was produced, with
 * any excess output samples used before feeding the resampler with more input
 * data. A "relief" factor here is that the resampler removes the initial
 * processing latency automatically, and that after initial moments of
 * processing the output becomes steady, with only minor output sample data
 * length fluctuations.
 *
 * Note that the r8b::CDSPResampler::getInLenBeforeOutStart() function can be
 * used to estimate the number of input samples that should be provided to the
 * resampler before the actual output starts.
 *
 * @section notes Notes
 *
 * When using the r8b::CDSPResampler<> class directly, you may select the
 * transition band/steepness of the low-pass (reconstruction) filter,
 * expressed as a percentage of the full spectral bandwidth of the input
 * signal (or the output signal if the downsampling is performed), and the
 * desired stop-band attenuation in decibel.
 *
 * The transition band is specified as the normalized spectral space of the
 * input signal (or the output signal if the downsampling is performed)
 * between the low-pass filter's -3 dB point and the Nyquist frequency, and
 * ranges from 0.5% to 45%. Stop-band attenuation can be specified in the
 * range 49 to 218 decibel.
 *
 * This SRC library also implements a faster "power of 2" resampling (e.g. 2X,
 * 4X, 8X, 16X, etc. upsampling and downsampling).
 *
 * This library was tested for compatibility with GNU C++, Microsoft Visual
 * C++ and Intel C++ compilers, on 32- and 64-bit Windows, Mac OS X and CentOS
 * Linux.
 *
 * All code is fully "inline", without the need to compile many source files.
 * The memory footprint is quite modest (8-byte "double" type data):
 *
 *  * 1.2 MB of static memory for the fractional delay filters
 *  * filter memory, per filter (N*8*2 bytes, where N is the block length,
 *    usually in the range 256 to 2048)
 *  * Ooura's FFT algorithm tables, per channel (N*8 bytes), plus 1 to 7
 *    smaller tables (128*8 bytes) for the "power of 2" resampling
 *  * convolver memory, per channel (N*8*5 bytes), plus Z=1 to 7 smaller
 *    convolvers for the "power of 2" resampling (Z*128*8*5 bytes)
 *  * interpolator memory (8 KB per channel)
 *  * I/O buffers, per channel (proportional to the maximal input buffer
 *    length and the source to destination sample rate ratio)
 *
 * @section users Users
 *
 * This library is used by:
 *
 *  * http://www.martinic.com/combov/ Combo Model V VSTi instrument
 *  * http://midithru.net/Home/AsioLink WDM Asio Link Driver
 *
 * @section license License
 *
 * The MIT License (MIT)
 * 
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * Please credit the creator of this library in your documentation in the
 * following way: "Sample rate converter designed by Aleksey Vaneev of
 * Voxengo"
 *
 * @version 1.5
 */

#ifndef R8BBASE_INCLUDED
#define R8BBASE_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "r8bconf.h"

#if defined( R8B_WIN )
	#include <windows.h>
#else // R8B_WIN
	#include <pthread.h>
#endif // R8B_WIN

#if defined( __clang__ ) || defined( __GCC__ )
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-parameter -Wparentheses"
#elif defined( _MSC_VER )
	#pragma warning( push )
	#pragma warning( disable : 4100 4706)
#endif

/**
 * @brief The "r8brain-free-src" library namespace.
 *
 * The "r8brain-free-src" sample rate converter library namespace.
 */

namespace r8b {

#if !defined( M_PI )
	/**
	 * The macro equals to "pi" constant, fits 53-bit floating point mantissa.
	 */

	#define M_PI 3.14159265358979324
#endif // M_PI

#if !defined( M_2PI )
	/**
	 * The M_2PI macro equals to "2 * pi" constant, fits 53-bit floating point
	 * mantissa.
	 */

	#define M_2PI 6.28318530717958648
#endif // M_2PI

#if !defined( M_3PI )
	/**
	 * The M_3PI macro equals to "3 * pi" constant, fits 53-bit floating point
	 * mantissa.
	 */

	#define M_3PI 9.42477796076937972
#endif // M_3PI

/**
 * A special macro that defines empty copy-constructor and copy operator with
 * the "private:" prefix. This macro should be used in classes that cannot be
 * copied in a standard C++ way.
 *
 * This macro does not need to be defined in classes derived from a class
 * where such macro was already used.
 *
 * @param ClassName The name of the class which uses this macro.
 */

#define R8BNOCTOR( ClassName ) \
	private: \
		ClassName( const ClassName& ) { } \
		ClassName& operator = ( const ClassName& ) { return( *this ); }

/**
 * @brief The default base class for objects created on heap.
 *
 * Class that implements "new" and "delete" operators that use standard
 * malloc() and free() functions.
 */

class CStdClassAllocator
{
public:
	/**
	 * @param n The size of the object, in bytes.
	 * @param p Pointer to object's pre-allocated memory block.
	 * @return Pointer to object.
	 */

	void* operator new( size_t n, void* p )
	{
		return( p );
	}

	/**
	 * @param n The size of the object, in bytes.
	 * @return Pointer to the allocated memory block for the object.
	 */

	void* operator new( size_t n )
	{
		return( :: malloc( n ));
	}

	/**
	 * @param n The size of the object, in bytes.
	 * @return Pointer to the allocated memory block for the object.
	 */

	void* operator new[]( size_t n )
	{
		return( :: malloc( n ));
	}

	/**
	 * Operator frees a previously allocated memory block for the object.
	 *
	 * @param p Pointer to the allocated memory block for the object.
	 */

	void operator delete( void* p )
	{
		:: free( p );
	}

	/**
	 * Operator frees a previously allocated memory block for the object.
	 *
	 * @param p Pointer to the allocated memory block for the object.
	 */

	void operator delete[]( void* p )
	{
		:: free( p );
	}
};

/**
 * @brief The default base class for objects that allocate blocks of memory.
 *
 * Memory buffer allocator that uses "stdlib" standard memory functions.
 */

class CStdMemAllocator : public CStdClassAllocator
{
public:
	/**
	 * Function allocates memory block.
	 *
	 * @param Size The size of the block, in bytes.
	 * @result The pointer to the allocated block.
	 */

	static void* allocmem( const size_t Size )
	{
		return( :: malloc( Size ));
	}

	/**
	 * Function reallocates a previously allocated memory block.
	 *
	 * @param p Pointer to the allocated block, can be NULL.
	 * @param Size The new size of the block, in bytes.
	 * @result The pointer to the (re)allocated block.
	 */

	static void* reallocmem( void* p, const size_t Size )
	{
		return( :: realloc( p, Size ));
	}

	/**
	 * Function frees a previously allocated memory block.
	 *
	 * @param p Pointer to the allocated block, can be NULL.
	 */

	static void freemem( void* p )
	{
		:: free( p );
	}
};

/**
 * @brief Templated memory buffer class for element buffers of fixed capacity.
 *
 * Fixed memory buffer object. Supports allocation of a fixed amount of
 * memory. Does not store buffer's capacity - the user should know the actual
 * capacity of the buffer. Does not feature "internal" storage, memory is
 * always allocated via the R8B_MEMALLOCCLASS class's functions. Thus the
 * object of this class can be moved in memory.
 *
 * This class manages memory space only - it does not perform element class
 * construction nor destruction operations.
 *
 * @param T The class of the stored elements (e.g. "double").
 */

template< class T >
class CFixedBuffer : public R8B_MEMALLOCCLASS
{
	R8BNOCTOR( CFixedBuffer );

public:
	CFixedBuffer()
		: Data( NULL )
	{
	}

	/**
	 * Constructor allocates memory so that the specified number of elements
	 * of type T can be stored in *this buffer object.
	 *
	 * @param Capacity Storage for this number of elements to allocate.
	 */

	CFixedBuffer( const int Capacity )
	{
		R8BASSERT( Capacity > 0 || Capacity == 0 );

		Data = (T*) allocmem( Capacity * sizeof( T ));

		R8BASSERT( Data != NULL || Capacity == 0 );
	}

	~CFixedBuffer()
	{
		freemem( Data );
	}

	/**
	 * Function allocates memory so that the specified number of elements of
	 * type T can be stored in *this buffer object.
	 *
	 * @param Capacity Storage for this number of elements to allocate.
	 */

	void alloc( const int Capacity )
	{
		R8BASSERT( Capacity > 0 || Capacity == 0 );

		freemem( Data );
		Data = (T*) allocmem( Capacity * sizeof( T ));

		R8BASSERT( Data != NULL || Capacity == 0 );
	}

	/**
	 * Function deallocates a previously allocated buffer.
	 */

	void free()
	{
		freemem( Data );
		Data = NULL;
	}

	/**
	 * @return Pointer to the first element of the allocated buffer, NULL if
	 * not allocated.
	 */

	T* getPtr() const
	{
		return( Data );
	}

	/**
	 * @return Pointer to the first element of the allocated buffer, NULL if
	 * not allocated.
	 */

	operator T* () const
	{
		return( Data );
	}

private:
	T* Data; ///< Element buffer pointer.
		///<
};

/**
 * @brief Pointer-to-object "keeper" class with automatic deletion.
 *
 * An auxiliary class that can be used for keeping a pointer to object that
 * should be deleted together with the "keeper" by calling object's "delete"
 * operator.
 *
 * @param T Pointer type to operate with, must include the asterisk (e.g.
 * "CDSPFIRFilter*").
 */

template< class T >
class CPtrKeeper
{
	R8BNOCTOR( CPtrKeeper );

public:
	CPtrKeeper()
		: Object( NULL )
	{
	}

	/**
	 * Constructor assigns a pointer to object to *this keeper.
	 *
	 * @param aObject Pointer to object to keep, can be NULL.
	 */

	template< class T2 >
	CPtrKeeper( T2 const aObject )
		: Object( aObject )
	{
	}

	~CPtrKeeper()
	{
		delete Object;
	}

	/**
	 * Function assigns a pointer to object to *this keeper. A previously
	 * keeped pointer will be reset and object deleted.
	 *
	 * @param aObject Pointer to object to keep, can be NULL.
	 */

	template< class T2 >
	void operator = ( T2 const aObject )
	{
		reset();
		Object = aObject;
	}

	/**
	 * @return Pointer to keeped object, NULL if no object is being kept.
	 */

	T operator -> () const
	{
		return( Object );
	}

	/**
	 * @return Pointer to keeped object, NULL if no object is being kept.
	 */

	operator T () const
	{
		return( Object );
	}

	/**
	 * Function resets the keeped pointer and deletes the keeped object.
	 */

	void reset()
	{
		T DelObj = Object;
		Object = NULL;
		delete DelObj;
	}

	/**
	 * @return Function returns the keeped pointer and resets it in *this
	 * keeper without object deletion.
	 */

	T unkeep()
	{
		T ResObject = Object;
		Object = NULL;
		return( ResObject );
	}

private:
	T Object; ///< Pointer to keeped object.
		///<
};

/**
 * @brief Multi-threaded synchronization object class.
 *
 * This class uses standard OS thread-locking (mutex) mechanism which is
 * fairly efficient in most cases.
 *
 * The acquire() function can be called recursively, in the same thread, for
 * this kind of thread-locking mechanism. This will not produce a dead-lock.
 */

class CSyncObject
{
	R8BNOCTOR( CSyncObject );

public:
	CSyncObject()
	{
		#if defined( R8B_WIN )
			#if ! WINAPI_FAMILY_PARTITION( WINAPI_PARTITION_DESKTOP )
				InitializeCriticalSectionEx( &CritSec, 4000, 0 );
			#else
				InitializeCriticalSectionAndSpinCount( &CritSec, 4000 );
			#endif
		#else // R8B_WIN
			pthread_mutexattr_t MutexAttrs;
			pthread_mutexattr_init( &MutexAttrs );
			pthread_mutexattr_settype( &MutexAttrs, PTHREAD_MUTEX_RECURSIVE );
			pthread_mutex_init( &Mutex, &MutexAttrs );
			pthread_mutexattr_destroy( &MutexAttrs );
		#endif // R8B_WIN
	}

	~CSyncObject()
	{
		#if defined( R8B_WIN )
			DeleteCriticalSection( &CritSec );
		#else // R8B_WIN
			pthread_mutex_destroy( &Mutex );
		#endif // R8B_WIN
	}

	/**
	 * Function "acquires" *this thread synchronizer object immediately or
	 * waits until another thread releases it.
	 */

	void acquire()
	{
		#if defined( R8B_WIN )
			EnterCriticalSection( &CritSec );
		#else // R8B_WIN
			pthread_mutex_lock( &Mutex );
		#endif // R8B_WIN
	}

	/**
	 * Function "releases" *this previously acquired thread synchronizer
	 * object.
	 */

	void release()
	{
		#if defined( R8B_WIN )
			LeaveCriticalSection( &CritSec );
		#else // R8B_WIN
			pthread_mutex_unlock( &Mutex );
		#endif // R8B_WIN
	}

private:
	#if defined( R8B_WIN )
		CRITICAL_SECTION CritSec; ///< Standard Windows critical section
			///< structure.
			///<
	#else // R8B_WIN
		pthread_mutex_t Mutex; ///< pthread.h mutex object.
			///<
	#endif // R8B_WIN
};

/**
 * @brief A "keeper" class for CSyncObject-based synchronization.
 *
 * Sync keeper class. The object of this class can be used as auto-init and
 * auto-deinit object for calling the acquire() and release() functions of an
 * object of the CSyncObject class. This "keeper" object is best used in
 * functions as an "automatic" object allocated on the stack, possibly via the
 * R8BSYNC() macro.
 */

class CSyncKeeper
{
	R8BNOCTOR( CSyncKeeper );

public:
	CSyncKeeper()
		: SyncObj( NULL )
	{
	}

	/**
	 * @param aSyncObj Pointer to the sync object which should be used for
	 * sync'ing, can be NULL.
	 */

	CSyncKeeper( CSyncObject* const aSyncObj )
		: SyncObj( aSyncObj )
	{
		if( SyncObj != NULL )
		{
			SyncObj -> acquire();
		}
	}

	/**
	 * @param aSyncObj Reference to the sync object which should be used for
	 * sync'ing.
	 */

	CSyncKeeper( CSyncObject& aSyncObj )
		: SyncObj( &aSyncObj )
	{
		SyncObj -> acquire();
	}

	~CSyncKeeper()
	{
		if( SyncObj != NULL )
		{
			SyncObj -> release();
		}
	}

protected:
	CSyncObject* SyncObj; ///< Sync object in use (can be NULL).
		///<
};

/**
 * The synchronization macro. The R8BSYNC( obj ) macro, which creates and
 * object of the r8b::CSyncKeeper class on stack, should be put before
 * sections of the code that may potentially change data asynchronously with
 * other threads at the same time. The R8BSYNC( obj ) macro "acquires" the
 * synchronization object thus blocking execution of other threads that also
 * use the same R8BSYNC( obj ) macro. The blocked section begins with the
 * R8BSYNC( obj ) macro and finishes at the end of the current C++ code block.
 * Multiple R8BSYNC() macros may be defined from within the same code block.
 *
 * @param SyncObject An object of the CSyncObject type that is used for
 * synchronization.
 */

#define R8BSYNC( SyncObject ) R8BSYNC_( SyncObject, __LINE__ )
#define R8BSYNC_( SyncObject, id ) R8BSYNC__( SyncObject, id )
#define R8BSYNC__( SyncObject, id ) CSyncKeeper SyncKeeper##id( SyncObject )

/**
 * @brief Sine signal generator class.
 *
 * Class implements sine signal generator with optional biasing.
 */

class CSineGen
{
public:
	/**
	 * Function initializes *this sine signal generator.
	 *
	 * @param si Sine function increment, in radians.
	 * @param g Overall gain. If biasing is not planned to be used, this value
	 * should be twice as high.
	 * @param ph Starting phase, in radians. Add 0.5 * M_PI for cosine
	 * function.
	 */

	void init( const double si, const double g,
		const double ph = -0.5 * M_PI )
	{
		bias = 0.5 * g;
		svalue1 = sin( ph ) * bias;
		svalue2 = sin( ph - si ) * bias;
		sincr = 2.0 * cos( si );
	}

	/**
	 * @return Next value of the sine function, with biasing.
	 */

	double genBias()
	{
		const double r = svalue1 + bias;

		const double tmp = svalue1;
		svalue1 = sincr * svalue1 - svalue2;
		svalue2 = tmp;

		return( r );
	}

	/**
	 * @return Next value of the sine function, without biasing.
	 */

	double gen()
	{
		const double r = svalue1;

		const double tmp = svalue1;
		svalue1 = sincr * svalue1 - svalue2;
		svalue2 = tmp;

		return( r );
	}

private:
	double bias; ///< Value bias.
		///<
	double svalue1; ///< Current sine value.
		///<
	double svalue2; ///< Previous sine value.
		///<
	double sincr; ///< Sine value increment.
		///<
};

/**
 * @param v Input value.
 * @return Calculated bit occupancy of the specified input value. Bit
 * occupancy means how many significant lower bits are necessary to store a
 * specified value. Function treats the input value as unsigned.
 */

inline int getBitOccupancy( const int v )
{
	static const char OccupancyTable[] =
	{
		1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};

	int t, tt;
	tt = v >> 16;

	if( tt != 0 )
	{
		return(( t = v >> 24 ) ? 24 + OccupancyTable[ t ] :
			16 + OccupancyTable[ tt & 0xFF ]);
	}
	else
	{
		return(( t = v >> 8 ) ? 8 + OccupancyTable[ t ] :
			OccupancyTable[ v ]);
	}
}

/**
 * Function calculates frequency response of the specified FIR filter at the
 * specified circular frequency.
 *
 * @param flt FIR filter's coefficients.
 * @param fltlen Number of coefficients (taps) in the filter.
 * @param th Circular frequency (0..pi).
 * @param[out] re0 Resulting real part of the complex frequency response.
 * @param[out] im0 Resulting imaginary part of the complex frequency response.
 */

inline void calcFIRFilterResponse( const double* flt, int fltlen,
	const double th, double& re0, double& im0 )
{
	double svalue1 = 0.0;
	double svalue2 = sin( -th );
	const double sincr = 2.0 * cos( th );
	double cvalue1 = 1.0;
	double cvalue2 = sin( M_PI * 0.5 - th );
	double re = 0.0;
	double im = 0.0;

	while( fltlen > 0 )
	{
		re += svalue1 * flt[ 0 ];
		im += cvalue1 * flt[ 0 ];
		flt++;
		fltlen--;

		double tmp = svalue1;
		svalue1 = sincr * svalue1 - svalue2;
		svalue2 = tmp;

		tmp = cvalue1;
		cvalue1 = sincr * cvalue1 - cvalue2;
		cvalue2 = tmp;
	}

	re0 = re;
	im0 = im;
}

/**
 * Function calculates frequency response and group delay of the specified FIR
 * filter at the specified circular frequency. The group delay is calculated
 * by evaluating the filter's response at close side-band frequencies of "th".
 *
 * @param flt FIR filter's coefficients.
 * @param fltlen Number of coefficients (taps) in the filter.
 * @param th Circular frequency (0..pi).
 * @param[out] re Resulting real part of the complex frequency response.
 * @param[out] im Resulting imaginary part of the complex frequency response.
 * @param[out] gd Resulting group delay at the specified frequency, in
 * samples.
 */

inline void calcFIRFilterResponseAndGroupDelay( const double* const flt,
	const int fltlen, const double th, double& re, double& im, double& gd )
{
	// Calculate response at "th".

	calcFIRFilterResponse( flt, fltlen, th, re, im );

	// Calculate response at close sideband frequencies.

	const int Count = 2;
	const double thd2 = 1e-9;
	double ths[ Count ] = { th - thd2, th + thd2 };

	if( ths[ 0 ] < 0.0 )
	{
		ths[ 0 ] = 0.0;
	}

	if( ths[ 1 ] > M_PI )
	{
		ths[ 1 ] = M_PI;
	}

	double ph1[ Count ];
	int i;

	for( i = 0; i < Count; i++ )
	{
		double re1;
		double im1;

		calcFIRFilterResponse( flt, fltlen, ths[ i ], re1, im1 );
		ph1[ i ] = atan2( im1, re1 );
	}

	if( fabs( ph1[ 1 ] - ph1[ 0 ]) > M_PI )
	{
		if( ph1[ 1 ] > ph1[ 0 ])
		{
			ph1[ 1 ] -= M_2PI;
		}
		else
		{
			ph1[ 1 ] += M_2PI;
		}
	}

	const double thd = ths[ 1 ] - ths[ 0 ];
	gd = ( ph1[ 1 ] - ph1[ 0 ]) / -thd;
}

/**
 * Function normalizes FIR filter so that its frequency response at DC is
 * equal to DCGain.
 *
 * @param[in,out] p Filter coefficients.
 * @param l Filter length.
 * @param DCGain Filter's gain at DC (linear, non-decibel value).
 * @param pstep "p" array step.
 */

inline void normalizeFIRFilter( double* const p, const int l,
	const double DCGain, const int pstep = 1 )
{
	R8BASSERT( l > 0 );
	R8BASSERT( pstep != 0 );

	double s = 0.0;
	double* pp = p;
	int i = l;

	while( i > 0 )
	{
		s += *pp;
		pp += pstep;
		i--;
	}

	s = DCGain / s;
	pp = p;
	i = l;

	while( i > 0 )
	{
		*pp *= s;
		pp += pstep;
		i--;
	}
}

/**
 * Function calculates coefficients used to calculate 3rd order spline
 * (polynomial) on the equidistant lattice, using 8 points.
 *
 * @param[out] c Output coefficients buffer, length = 4.
 * @param xm3 Point at x-3 position.
 * @param xm2 Point at x-2 position.
 * @param xm1 Point at x-1 position.
 * @param x0 Point at x position.
 * @param x1 Point at x+1 position.
 * @param x2 Point at x+2 position.
 * @param x3 Point at x+3 position.
 * @param x4 Point at x+4 position.
 */

inline void calcSpline3p8Coeffs( double* c, const double xm3,
	const double xm2, const double xm1, const double x0, const double x1,
	const double x2, const double x3, const double x4 )
{
	c[ 0 ] = x0;
	c[ 1 ] = ( 61.0 * ( x1 - xm1 ) + 16.0 * ( xm2 - x2 ) +
		3.0 * ( x3 - xm3 )) / 76.0;

	c[ 2 ] = ( 106.0 * ( xm1 + x1 ) + 10.0 * x3 + 6.0 * xm3 - 3.0 * x4 -
		29.0 * ( xm2 + x2 ) - 167.0 * x0 ) / 76.0;

	c[ 3 ] = ( 91.0 * ( x0 - x1 ) + 45.0 * ( x2 - xm1 ) +
		13.0 * ( xm2 - x3 ) + 3.0 * ( x4 - xm3 )) / 76.0;
}

/**
 * Function calculates coefficients used to calculate 2rd order spline
 * (polynomial) on the equidistant lattice, using 8 points. This function is
 * based on the calcSpline3Coeffs8() function, but without the 3rd order
 * coefficient.
 *
 * @param[out] c Output coefficients buffer, length = 3.
 * @param xm3 Point at x-3 position.
 * @param xm2 Point at x-2 position.
 * @param xm1 Point at x-1 position.
 * @param x0 Point at x position.
 * @param x1 Point at x+1 position.
 * @param x2 Point at x+2 position.
 * @param x3 Point at x+3 position.
 * @param x4 Point at x+4 position.
 */

inline void calcSpline2p8Coeffs( double* c, const double xm3,
	const double xm2, const double xm1, const double x0, const double x1,
	const double x2, const double x3, const double x4 )
{
	c[ 0 ] = x0;
	c[ 1 ] = ( 61.0 * ( x1 - xm1 ) + 16.0 * ( xm2 - x2 ) +
		3.0 * ( x3 - xm3 )) / 76.0;

	c[ 2 ] = ( 106.0 * ( xm1 + x1 ) + 10.0 * x3 + 6.0 * xm3 - 3.0 * x4 -
		29.0 * ( xm2 + x2 ) - 167.0 * x0 ) / 76.0;
}

#if !defined( min )

/**
 * @param v1 Value 1.
 * @param v2 Value 2.
 * @return The minimum of 2 values.
 */

template< class T >
inline T min( const T& v1, const T& v2 )
{
	return( v1 < v2 ? v1 : v2 );
}

#endif // min

#if !defined( max )

/**
 * @param v1 Value 1.
 * @param v2 Value 2.
 * @return The maximum of 2 values.
 */

template< class T >
inline T max( const T& v1, const T& v2 )
{
	return( v1 > v2 ? v1 : v2 );
}

#endif // max

/**
 * Function "clamps" (clips) the specified value so that it is not lesser than
 * "minv", and not greater than "maxv".
 *
 * @param Value Value to clamp.
 * @param minv Minimal allowed value.
 * @param maxv Maximal allowed value.
 * @return "Clamped" value.
 */

inline double clampr( const double Value, const double minv,
	const double maxv )
{
	if( Value < minv )
	{
		return( minv );
	}
	else
	if( Value > maxv )
	{
		return( maxv );
	}
	else
	{
		return( Value );
	}
}

/**
 * @param x Value to square.
 * @return Squared value of the argument.
 */

inline double sqr( const double x )
{
	return( x * x );
}

/**
 * @param v Input value.
 * @param p Power factor.
 * @return Returns pow() function's value with input value's sign check.
 */

inline double pows( const double v, const double p )
{
	return( v < 0.0 ? -pow( -v, p ) : pow( v, p ));
}

/**
 * @param v Input value.
 * @return Calculated single-argument Gaussian function of the input value.
 */

inline double gauss( const double v )
{
	return( exp( -( v * v )));
}

/**
 * @param v Input value.
 * @return Calculated inverse hyperbolic sine of the input value.
 */

inline double asinh( const double v )
{
	return( log( v + sqrt( v * v + 1.0 )));
}

/**
 * @param x Input value.
 * @return Calculated zero-th order modified Bessel function of the first kind
 * of the input value. Approximate value.
 */

inline double besselI0( const double x )
{
	const double ax = fabs( x );
	double y;

	if( ax < 3.75 )
	{
		y = x / 3.75;
		y *= y;

		return( 1.0 + y * ( 3.5156229 + y * ( 3.0899424 + y * ( 1.2067492 +
			y * ( 0.2659732 + y * ( 0.360768e-1 + y * 0.45813e-2 ))))));
	}

	y = 3.75 / ax;

	return( exp( ax ) / sqrt( ax ) * ( 0.39894228 + y * ( 0.1328592e-1 +
		y * ( 0.225319e-2 + y * ( -0.157565e-2 + y * ( 0.916281e-2 +
		y * ( -0.2057706e-1 + y * ( 0.2635537e-1 + y * ( -0.1647633e-1 +
		y * 0.392377e-2 )))))))));
}

} // namespace r8b

#if defined( __clang__ ) || defined( __GCC__ )
	#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif // R8BBASE_INCLUDED
