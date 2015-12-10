/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Surface.h"
#include "cinder/Stream.h"

#include "cinder/Log.h"

#include <string>
#include <windows.h>
#include <comdef.h> // for _com_error
#include <Objidl.h>
#undef min
#undef max

namespace cinder { namespace msw {

#ifndef LOWORD
#define LOWORD(_dw)     ((WORD)(((DWORD_PTR)(_dw)) & 0xffff))
#endif // !LOWORD

#ifndef HIWORD
#define HIWORD(_dw)     ((WORD)((((DWORD_PTR)(_dw)) >> 16) & 0xffff))
#endif // !HIWORD

#ifndef LODWORD
#define LODWORD(_qw)    ((DWORD)(_qw))
#endif // !LODWORD

#ifndef HIDWORD
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))
#endif // !HIDWORD

#ifndef HRESULT_STRING
#define HRESULT_STRING(hr)            ci::msw::toUtf8String(std::wstring((const wchar_t*)_com_error(hr).ErrorMessage()))
#endif // !HRESULT_STRING

#ifndef ERROR_STRING
#define ERROR_STRING(err)             HRESULT_STRING( HRESULT_FROM_WIN32( err ) )
#endif // !ERROR_STRING

#ifndef BREAK_ON_FAIL
#define BREAK_ON_FAIL(value)          if( FAILED( value ) ) { break; }
#endif // !BREAK_ON_FAIL

#ifndef BREAK_ON_FAIL_MSG
#define BREAK_ON_FAIL_MSG(value, msg) if( FAILED( value ) ) { CI_LOG_E(msg << " (" << HRESULT_STRING(hr) << ")"); break; }
#endif // !BREAK_ON_FAIL_MSG

#ifndef BREAK_ON_NULL
#define BREAK_ON_NULL(value, result)  if( value == NULL ) { hr = result; break; }
#endif // !BREAK_ON_NULL

#ifndef BREAK_ON_NULL_MSG
#define BREAK_ON_NULL_MSG(value, result, msg)  if( value == NULL ) { hr = result; CI_LOG_E(msg << " (" << HRESULT_STRING(hr) << ")"); break; }
#endif // !BREAK_ON_NULL_MSG

#ifndef BREAK_IF_TRUE
#define BREAK_IF_TRUE(test, result)  if( (test) ) { hr = result; break; }
#endif // !BREAK_IF_TRUE

#ifndef BREAK_IF_TRUE_MSG
#define BREAK_IF_TRUE_MSG(test, result, msg)  if( (test) ) { hr = result; CI_LOG_E(msg); break; }
#endif // !BREAK_IF_TRUE_MSG

#ifndef BREAK_IF_FALSE
#define BREAK_IF_FALSE(test, result)  if( !(test) ) { hr = result; break; }
#endif // !BREAK_IF_FALSE

#ifndef BREAK_IF_FALSE_MSG
#define BREAK_IF_FALSE_MSG(test, result, msg)  if( !(test) ) { hr = result; CI_LOG_E(msg); break; }
#endif // !BREAK_IF_FALSE_MSG

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg)     static_assert(expr, #msg)

/** Converts a Win32 HBITMAP to a cinder::Surface8u
	\note Currently always copies the alpha channel **/
Surface8uRef convertHBitmap( HBITMAP hbitmap );

//! Converts a UTF-8 string into a wide string (wstring). Note that wstring is not a good cross-platform choice and this is here for interop with Windows APIs.
std::wstring toWideString( const std::string &utf8String );
//! Converts a wide string to a UTF-8 string. Note that wstring is not a good cross-platform choice and this is here for interop with Windows APIs.
std::string toUtf8String( const std::wstring &wideString );

//! Converts a Win32 POINTFX fixed point point to a cinder::vec2
#if !defined ( CINDER_WINRT )
inline vec2 toVec2( const ::POINTFX &p )
{ return vec2( ( (p.x.value << 16) | p.x.fract ) / 65535.0f, ( (p.y.value << 16) | p.y.fract ) / 65535.0f ); }
#endif

//! Closes a handle if not NULL, and sets the handle to NULL.
inline void SafeCloseHandle( HANDLE& h )
{
	if( h != NULL ) {
		CloseHandle( h );
		h = NULL;
	}
}

//! Deletes a pointer allocated with new.
template <class T> inline void SafeDelete( T*& pT )
{
	if( pT != NULL ) {
		delete pT;
		pT = NULL;
	}
}

//! Deletes a pointer to an array allocated with new[].
template <class T> inline void SafeDeleteArray( T*& pT )
{
	if( pT != NULL ) {
		delete[] pT;
		pT = NULL;
	}
}

//! Releases a COM pointer if the pointer is not NULL, and sets the pointer to NULL.
template <class T> inline ULONG SafeRelease( T*& pT )
{
	ULONG rc = 0;

	if( pT != NULL ) {
		rc = pT->Release();
		pT = NULL;
	}

	return rc;
}

//! Converts a value in ticks to a value in milliseconds. One tick is 100 nanoseconds.
template <class T> inline double TicksToMilliseconds( const T& t )
{
	return t / 10000.0;
}

//! Converts a value in milliseconds to a value in ticks. One tick is 100 nanoseconds.
template <class T> inline T MillisecondsToTicks( const T& t )
{
	return t * 10000;
}

//! A free function designed to interact with makeComShared, calls Release() on a com-managed object
void ComDelete( void *p );

//! Functor version that calls Release() on a com-managed object
struct ComDeleter {
	template <typename T>
	void operator()( T *p )	{ if( p ) p->Release(); }
};

template<typename T>
using ManagedComRef = std::shared_ptr<T>;

//! Creates a shared_ptr whose deleter will properly decrement the reference count of a COM object
template<typename T>
ManagedComRef<T> makeComShared( T *p )		{ return ManagedComRef<T>( p, &ComDelete ); }

template<typename T>
using ManagedComPtr = std::unique_ptr<T, ComDeleter>;

//! Creates a unique_ptr whose deleter will properly decrement the reference count of a COM object
template<typename T>
ManagedComPtr<T> makeComUnique( T *p )		{ return ManagedComPtr<T>( p ); }

//! Wraps a cinder::OStream with a COM ::IStream
class ComOStream : public ::IStream
{
  public:
	ComOStream( cinder::OStreamRef aOStream ) : mOStream( aOStream ), _refcount( 1 ) {}

    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void ** ppvObject );
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release(); 

  // ISequentialStream Interface
  public:
	virtual HRESULT STDMETHODCALLTYPE Read( void* pv, ULONG cb, ULONG* pcbRead ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Write( void const* pv, ULONG cb, ULONG* pcbWritten );
  // IStream Interface
  public:
	virtual HRESULT STDMETHODCALLTYPE SetSize( ULARGE_INTEGER ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CopyTo( ::IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER* ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Commit( DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Revert() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE LockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Seek( LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer );
	virtual HRESULT STDMETHODCALLTYPE Stat( STATSTG* pStatstg, DWORD grfStatFlag) { return E_NOTIMPL; }

  private:
	cinder::OStreamRef	mOStream;
	LONG			_refcount;
};

//! Wraps a cinder::IStream with a COM ::IStream
class ComIStream : public ::IStream
{
public:
	ComIStream( cinder::IStreamRef aIStream ) : mIStream( aIStream ), _refcount( 1 ) {}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void ** ppvObject );
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release(); 

	// ISequentialStream Interface
public:
	virtual HRESULT STDMETHODCALLTYPE Read( void* pv, ULONG cb, ULONG* pcbRead );
	virtual HRESULT STDMETHODCALLTYPE Write( void const* pv, ULONG cb, ULONG* pcbWritten ) { return E_NOTIMPL; }
	// IStream Interface
public:
	virtual HRESULT STDMETHODCALLTYPE SetSize( ULARGE_INTEGER ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CopyTo( ::IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER* ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Commit( DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Revert() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE LockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Seek( LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer );
	virtual HRESULT STDMETHODCALLTYPE Stat( STATSTG* pStatstg, DWORD grfStatFlag);

private:
	cinder::IStreamRef	mIStream;
	LONG			_refcount;
};

//! Initializes COM on this thread. Uses thread local storage to prevent multiple initializations per thread
void initializeCom( DWORD params = COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

//! Copies a COM pointer and takes care of proper reference counting.
template <class T>
void CopyComPtr( T*& dest, T* src )
{
	if( dest ) {
		dest->Release();
	}
	dest = src;
	if( dest ) {
		dest->AddRef();
	}
}

//! Compares two COM pointers and returns \c true if they are equal.
template <class T1, class T2>
bool AreComObjectsEqual( T1 *p1, T2 *p2 )
{
	if( p1 == NULL && p2 == NULL ) {
		// Both are NULL
		return true;
	}
	else if( p1 == NULL || p2 == NULL ) {
		// One is NULL and one is not
		return false;
	}
	else {
		// Both are not NULL. Compare IUnknowns.
		ScopedComPtr<IUnknown> pUnk1, pUnk2;
		if( SUCCEEDED( p1->QueryInterface( IID_IUnknown, (void**)&pUnk1 ) ) ) {
			if( SUCCEEDED( p2->QueryInterface( IID_IUnknown, (void**)&pUnk2 ) ) ) {
				return ( pUnk1 == pUnk2 );
			}
		}
	}

	return false;
}

//! Warps a critical section.
class CriticalSection {
private:
	CRITICAL_SECTION mCriticalSection;
public:
	CriticalSection()
	{
		::InitializeCriticalSection( &mCriticalSection );
	}

	~CriticalSection()
	{
		assert( mCriticalSection.LockCount == -1 );
		::DeleteCriticalSection( &mCriticalSection );
	}

	void Lock()
	{
		::EnterCriticalSection( &mCriticalSection );
	}

	void Unlock()
	{
		assert( mCriticalSection.LockCount < -1 );
		::LeaveCriticalSection( &mCriticalSection );
	}
};

//! Provides automatic locking and unlocking of a critical section.
class ScopedCriticalSection {
private:
	CriticalSection *mCriticalSectionPtr;
public:
	ScopedCriticalSection( CriticalSection& section )
	{
		mCriticalSectionPtr = &section;
		mCriticalSectionPtr->Lock();
	}
	~ScopedCriticalSection()
	{
		assert( mCriticalSectionPtr != nullptr );
		mCriticalSectionPtr->Unlock();
	}
};

} } // namespace cinder::msw
