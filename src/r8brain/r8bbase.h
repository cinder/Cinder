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
 * For more information, please visit
 * https://github.com/avaneev/r8brain-free-src
 *
 * @section license License
 *
 * The MIT License (MIT)
 * 
 * r8brain-free-src Copyright (c) 2013-2022 Aleksey Vaneev
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
 * @version 5.7
 */

#ifndef R8BBASE_INCLUDED
#define R8BBASE_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "r8bconf.h"

#if defined( _WIN32 )
	#include <windows.h>
#else // defined( _WIN32 )
	#include <pthread.h>
#endif // defined( _WIN32 )

#if defined( __SSE4_2__ ) || defined( __SSE4_1__ ) || \
	defined( __SSSE3__ ) || defined( __SSE3__ ) || defined( __SSE2__ ) || \
	defined( __x86_64__ ) || defined( _M_AMD64 ) || defined( _M_X64 ) || \
	defined( __amd64 )

	#include <immintrin.h>

	#define R8B_SSE2
	#define R8B_SIMD_ISH

#elif defined( __aarch64__ ) || defined( __arm64__ )

	#include <arm_neon.h>

	#define R8B_NEON

	#if !defined( __APPLE__ )
		#define R8B_SIMD_ISH // Shuffled interpolation is inefficient on M1.
	#endif // !defined( __APPLE__ )

#endif // ARM64

/**
 * @brief The "r8brain-free-src" library namespace.
 *
 * The "r8brain-free-src" sample rate converter library namespace.
 */

namespace r8b {

/**
 * Macro defines r8brain-free-src version string.
 */

#define R8B_VERSION "5.7"

/**
 * The macro equals to "pi" constant, fits 53-bit floating point mantissa.
 */

#define R8B_PI 3.14159265358979324

/**
 * The R8B_2PI macro equals to "2 * pi" constant, fits 53-bit floating point
 * mantissa.
 */

#define R8B_2PI 6.28318530717958648

/**
 * The R8B_3PI macro equals to "3 * pi" constant, fits 53-bit floating point
 * mantissa.
 */

#define R8B_3PI 9.42477796076937972

/**
 * The R8B_PId2 macro equals to "pi divided by 2" constant, fits 53-bit
 * floating point mantissa.
 */

#define R8B_PId2 1.57079632679489662

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

	void* operator new( size_t, void* p )
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
 * This function forces the provided "ptr" pointer to be aligned to
 * "align" bytes. Works with power-of-2 alignments only.
 *
 * @param ptr Pointer to align.
 * @param align Alignment, in bytes, power-of-2.
 * @tparam T Pointer's element type.
 */

template< typename T >
inline T* alignptr( T* const ptr, const uintptr_t align )
{
	return( (T*) (( (uintptr_t) ptr + align - 1 ) & ~( align - 1 )));
}

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
 * This class applies 64-byte memory address alignment to the allocated data
 * block.
 *
 * @tparam T The type of the stored elements (e.g. "double").
 */

template< typename T >
class CFixedBuffer : public R8B_MEMALLOCCLASS
{
	R8BNOCTOR( CFixedBuffer );

public:
	CFixedBuffer()
		: Data0( NULL )
		, Data( NULL )
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

		Data0 = allocmem( Capacity * sizeof( T ) + Alignment );
		Data = (T*) alignptr( Data0, Alignment );

		R8BASSERT( Data0 != NULL || Capacity == 0 );
	}

	~CFixedBuffer()
	{
		freemem( Data0 );
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

		freemem( Data0 );
		Data0 = allocmem( Capacity * sizeof( T ) + Alignment );
		Data = (T*) alignptr( Data0, Alignment );

		R8BASSERT( Data0 != NULL || Capacity == 0 );
	}

	/**
	 * Function reallocates memory so that the specified number of elements of
	 * type T can be stored in *this buffer object. Previously allocated data
	 * is copied to the new memory buffer.
	 *
	 * @param PrevCapacity Previous capacity of *this buffer.
	 * @param NewCapacity Storage for this number of elements to allocate.
	 */

	void realloc( const int PrevCapacity, const int NewCapacity )
	{
		R8BASSERT( PrevCapacity >= 0 );
		R8BASSERT( NewCapacity >= 0 );

		void* const NewData0 = allocmem( NewCapacity * sizeof( T ) +
			Alignment );

		T* const NewData = (T*) alignptr( NewData0, Alignment );
		const size_t CopySize = ( PrevCapacity > NewCapacity ?
			NewCapacity : PrevCapacity ) * sizeof( T );

		if( CopySize > 0 )
		{
			memcpy( NewData, Data, CopySize );
		}

		freemem( Data0 );
		Data0 = NewData0;
		Data = NewData;

		R8BASSERT( Data0 != NULL || NewCapacity == 0 );
	}

	/**
	 * Function deallocates a previously allocated buffer.
	 */

	void free()
	{
		freemem( Data0 );
		Data0 = NULL;
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
	static const size_t Alignment = 64; ///< Buffer address alignment, in
		///< bytes.
		///<
	void* Data0; ///< Buffer pointer, original unaligned.
		///<
	T* Data; ///< Element buffer pointer, aligned.
		///<
};

/**
 * @brief Pointer-to-object "keeper" class with automatic deletion.
 *
 * An auxiliary class that can be used for keeping a pointer to object that
 * should be deleted together with the "keeper" by calling object's "delete"
 * operator.
 *
 * @tparam T Pointer type to operate with, must include the asterisk (e.g.
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
	 * @tparam T2 Object's pointer type.
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
	 * @tparam T2 Object's pointer type.
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
		#if defined( _WIN32 )
			InitializeCriticalSectionAndSpinCount( &CritSec, 4000 );
		#else // defined( _WIN32 )
			pthread_mutexattr_t MutexAttrs;
			pthread_mutexattr_init( &MutexAttrs );
			pthread_mutexattr_settype( &MutexAttrs, PTHREAD_MUTEX_RECURSIVE );
			pthread_mutex_init( &Mutex, &MutexAttrs );
			pthread_mutexattr_destroy( &MutexAttrs );
		#endif // defined( _WIN32 )
	}

	~CSyncObject()
	{
		#if defined( _WIN32 )
			DeleteCriticalSection( &CritSec );
		#else // defined( _WIN32 )
			pthread_mutex_destroy( &Mutex );
		#endif // defined( _WIN32 )
	}

	/**
	 * Function "acquires" *this thread synchronizer object immediately or
	 * waits until another thread releases it.
	 */

	void acquire()
	{
		#if defined( _WIN32 )
			EnterCriticalSection( &CritSec );
		#else // defined( _WIN32 )
			pthread_mutex_lock( &Mutex );
		#endif // defined( _WIN32 )
	}

	/**
	 * Function "releases" *this previously acquired thread synchronizer
	 * object.
	 */

	void release()
	{
		#if defined( _WIN32 )
			LeaveCriticalSection( &CritSec );
		#else // defined( _WIN32 )
			pthread_mutex_unlock( &Mutex );
		#endif // defined( _WIN32 )
	}

private:
	#if defined( _WIN32 )
		CRITICAL_SECTION CritSec; ///< Standard Windows critical section
			///< structure.
			///<
	#else // defined( _WIN32 )
		pthread_mutex_t Mutex; ///< pthread.h mutex object.
			///<
	#endif // defined( _WIN32 )
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
 * Class implements sine signal generator without biasing.
 */

class CSineGen
{
public:
	CSineGen()
	{
	}

	/**
	 * Constructor initializes *this sine signal generator, with unity gain
	 * output.
	 *
	 * @param si Sine function increment, in radians.
	 * @param ph Starting phase, in radians. Add R8B_PId2 for cosine function.
	 */

	CSineGen( const double si, const double ph )
		: svalue1( sin( ph ))
		, svalue2( sin( ph - si ))
		, sincr( 2.0 * cos( si ))
	{
	}

	/**
	 * Constructor initializes *this sine signal generator.
	 *
	 * @param si Sine function increment, in radians.
	 * @param ph Starting phase, in radians. Add R8B_PId2 for cosine function.
	 * @param g The overall gain factor, 1.0 for unity gain (-1.0 to 1.0
	 * amplitude).
	 */

	CSineGen( const double si, const double ph, const double g )
		: svalue1( sin( ph ) * g )
		, svalue2( sin( ph - si ) * g )
		, sincr( 2.0 * cos( si ))
	{
	}

	/**
	 * Function initializes *this sine signal generator, with unity gain
	 * output.
	 *
	 * @param si Sine function increment, in radians.
	 * @param ph Starting phase, in radians. Add R8B_PId2 for cosine function.
	 */

	void init( const double si, const double ph )
	{
		svalue1 = sin( ph );
		svalue2 = sin( ph - si );
		sincr = 2.0 * cos( si );
	}

	/**
	 * Function initializes *this sine signal generator.
	 *
	 * @param si Sine function increment, in radians.
	 * @param ph Starting phase, in radians. Add R8B_PId2 for cosine function.
	 * @param g The overall gain factor, 1.0 for unity gain (-1.0 to 1.0
	 * amplitude).
	 */

	void init( const double si, const double ph, const double g )
	{
		svalue1 = sin( ph ) * g;
		svalue2 = sin( ph - si ) * g;
		sincr = 2.0 * cos( si );
	}

	/**
	 * @return Next value of the sine function, without biasing.
	 */

	double generate()
	{
		const double res = svalue1;

		svalue1 = sincr * res - svalue2;
		svalue2 = res;

		return( res );
	}

private:
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
	static const uint8_t OccupancyTable[] =
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

	const int tt = v >> 16;

	if( tt != 0 )
	{
		const int t = v >> 24;

		return( t != 0 ? 24 + OccupancyTable[ t & 0xFF ] :
			16 + OccupancyTable[ tt ]);
	}
	else
	{
		const int t = v >> 8;

		return( t != 0 ? 8 + OccupancyTable[ t ] : OccupancyTable[ v ]);
	}
}

/**
 * Function calculates frequency response of the specified FIR filter at the
 * specified circular frequency. Phase can be calculated as atan2( im, re ).
 *
 * @param flt FIR filter's coefficients.
 * @param fltlen Number of coefficients (taps) in the filter.
 * @param th Circular frequency [0; pi].
 * @param[out] re0 Resulting real part of the complex frequency response.
 * @param[out] im0 Resulting imaginary part of the complex frequency response.
 * @param fltlat Filter's latency, in samples.
 */

inline void calcFIRFilterResponse( const double* flt, int fltlen,
	const double th, double& re0, double& im0, const int fltlat = 0 )
{
	const double sincr = 2.0 * cos( th );
	double cvalue1;
	double svalue1;

	if( fltlat == 0 )
	{
		cvalue1 = 1.0;
		svalue1 = 0.0;
	}
	else
	{
		cvalue1 = cos( -fltlat * th );
		svalue1 = sin( -fltlat * th );
	}

	double cvalue2 = cos( -( fltlat + 1 ) * th );
	double svalue2 = sin( -( fltlat + 1 ) * th );

	double re = 0.0;
	double im = 0.0;

	while( fltlen > 0 )
	{
		re += cvalue1 * flt[ 0 ];
		im += svalue1 * flt[ 0 ];
		flt++;
		fltlen--;

		double tmp = cvalue1;
		cvalue1 = sincr * cvalue1 - cvalue2;
		cvalue2 = tmp;

		tmp = svalue1;
		svalue1 = sincr * svalue1 - svalue2;
		svalue2 = tmp;
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
 * @param th Circular frequency [0; pi].
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

	if( ths[ 1 ] > R8B_PI )
	{
		ths[ 1 ] = R8B_PI;
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

	if( fabs( ph1[ 1 ] - ph1[ 0 ]) > R8B_PI )
	{
		if( ph1[ 1 ] > ph1[ 0 ])
		{
			ph1[ 1 ] -= R8B_2PI;
		}
		else
		{
			ph1[ 1 ] += R8B_2PI;
		}
	}

	const double thd = ths[ 1 ] - ths[ 0 ];
	gd = ( ph1[ 1 ] - ph1[ 0 ]) / thd;
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

inline void calcSpline3p8Coeffs( double* const c, const double xm3,
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
 * based on the calcSpline3p8Coeffs() function, but without the 3rd order
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

inline void calcSpline2p8Coeffs( double* const c, const double xm3,
	const double xm2, const double xm1, const double x0, const double x1,
	const double x2, const double x3, const double x4 )
{
	c[ 0 ] = x0;
	c[ 1 ] = ( 61.0 * ( x1 - xm1 ) + 16.0 * ( xm2 - x2 ) +
		3.0 * ( x3 - xm3 )) / 76.0;

	c[ 2 ] = ( 106.0 * ( xm1 + x1 ) + 10.0 * x3 + 6.0 * xm3 - 3.0 * x4 -
		29.0 * ( xm2 + x2 ) - 167.0 * x0 ) / 76.0;
}

/**
 * Function calculates coefficients used to calculate 3rd order segment
 * interpolation polynomial on the equidistant lattice, using 4 points.
 *
 * @param[out] c Output coefficients buffer, length = 4.
 * @param[in] y Equidistant point values. Value at offset 1 corresponds to
 * x=0 point.
 */

inline void calcSpline3p4Coeffs( double* const c, const double* const y )
{
	c[ 0 ] = y[ 1 ];
	c[ 1 ] = 0.5 * ( y[ 2 ] - y[ 0 ]);
	c[ 2 ] = y[ 0 ] - 2.5 * y[ 1 ] + y[ 2 ] + y[ 2 ] - 0.5 * y[ 3 ];
	c[ 3 ] = 0.5 * ( y[ 3 ] - y[ 0 ] ) + 1.5 * ( y[ 1 ] - y[ 2 ]);
}

/**
 * Function calculates coefficients used to calculate 3rd order segment
 * interpolation polynomial on the equidistant lattice, using 6 points.
 *
 * @param[out] c Output coefficients buffer, length = 4.
 * @param[in] y Equidistant point values. Value at offset 2 corresponds to
 * x=0 point.
 */

inline void calcSpline3p6Coeffs( double* const c, const double* const y )
{
	c[ 0 ] = y[ 2 ];
	c[ 1 ] = ( 11.0 * ( y[ 3 ] - y[ 1 ]) + 2.0 * ( y[ 0 ] - y[ 4 ])) / 14.0;
	c[ 2 ] = ( 20.0 * ( y[ 1 ] + y[ 3 ]) + 2.0 * y[ 5 ] - 4.0 * y[ 0 ] -
		7.0 * y[ 4 ] - 31.0 * y[ 2 ]) / 14.0;

	c[ 3 ] = ( 17.0 * ( y[ 2 ] - y[ 3 ]) + 9.0 * ( y[ 4 ] - y[ 1 ]) +
		2.0 * ( y[ 0 ] - y[ 5 ])) / 14.0;
}

#if !defined( min )

/**
 * @param v1 Value 1.
 * @param v2 Value 2.
 * @tparam T Values' type.
 * @return The minimum of 2 values.
 */

template< typename T >
inline T min( const T& v1, const T& v2 )
{
	return( v1 < v2 ? v1 : v2 );
}

#endif // min

#if !defined( max )

/**
 * @param v1 Value 1.
 * @param v2 Value 2.
 * @tparam T Values' type.
 * @return The maximum of 2 values.
 */

template< typename T >
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
 * of the input value. Approximate value. Coefficients by Abramowitz and
 * Stegun.
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

#endif // R8BBASE_INCLUDED
