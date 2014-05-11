/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/Stream.h"

#include <memory>
#include <Objidl.h>

struct tWAVEFORMATEX;
typedef struct tWAVEFORMATEX WAVEFORMATEX;

namespace cinder { namespace audio { namespace msw {

struct ComReleaser {
	template <typename T>
	void operator()( T* ptr )	{ ptr->Release(); }
};

//! Creates a unique_ptr whose deleter will properly decrement the reference count of a COM object
template<typename T>
inline std::unique_ptr<T, ComReleaser> makeComUnique( T *p )	{ return std::unique_ptr<T, ComReleaser>( p ); }

//! return pointer type is actually a WAVEFORMATEXTENSIBLE, identifiable by the wFormat tag
std::shared_ptr<::WAVEFORMATEX> interleavedFloatWaveFormat( size_t sampleRate, size_t numChannels );

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


} } } // namespace cinder::audio::msw
