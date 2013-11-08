/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include <string>
#include <windows.h>
#include <Objidl.h>
#undef min
#undef max

namespace cinder { namespace msw {

/** Converts a Win32 HBITMAP to a cinder::Surface8u
	\note Currently always copies the alpha channel **/
Surface8u convertHBitmap( HBITMAP hbitmap );


//! Converts a Win32 POINTFX fixed point point to a cinder::Vec2f
#if !defined ( CINDER_WINRT )
inline Vec2f toVec2f( const ::POINTFX &p )
{ return Vec2f( ( (p.x.value << 16) | p.x.fract ) / 65535.0f, ( (p.y.value << 16) | p.y.fract ) / 65535.0f ); }
#endif

//! A free function designed to interact with makeComShared
void ComDelete( void *p );

//! Creates a shared_ptr whose deleter will properly decrement the reference count of a COM object
template<typename T>
inline std::shared_ptr<T> makeComShared( T *p )
{ return std::shared_ptr<T>( p, &ComDelete ); }

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

//! Initializes COM on this thread. Uses Boost's thread local storage to prevent multiple initializations per thread
void initializeCom( DWORD params = COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

} } // namespace cinder::msw
