/*
 Copyright (c) 2010, The Cinder Project
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
CI_API Surface8uRef convertHBitmap( HBITMAP hbitmap );

//! Converts a UTF-8 string into a wide string (wstring). Note that wstring is not a good cross-platform choice and this is here for interop with Windows APIs.
CI_API std::wstring toWideString( const std::string& utf8String );
//! Converts a wide string to a UTF-8 string. Note that wstring is not a good cross-platform choice and this is here for interop with Windows APIs.
CI_API std::string toUtf8String( const std::wstring& wideString );

//! Converts a Win32 POINTFX fixed point point to a cinder::vec2
CI_API inline vec2 toVec2( const ::POINTFX& p )
{
	return vec2( ( ( p.x.value << 16 ) | p.x.fract ) / 65535.0f, ( ( p.y.value << 16 ) | p.y.fract ) / 65535.0f );
}

//! A free function designed to interact with makeComShared, calls Release() on a com-managed object
CI_API void ComDelete( void* p );

//! Functor version that calls Release() on a com-managed object
struct CI_API ComDeleter {
	template<typename T>
	void operator()( T* p )
	{
		if( p )
			p->Release();
	}
};

template<typename T>
using ManagedComRef = std::shared_ptr<T>;

//! Creates a shared_ptr whose deleter will properly decrement the reference count of a COM object
template<typename T>
ManagedComRef<T> makeComShared( T* p )
{
	return ManagedComRef<T>( p, &ComDelete );
}

template<typename T>
using ManagedComPtr = std::unique_ptr<T, ComDeleter>;

//! Creates a unique_ptr whose deleter will properly decrement the reference count of a COM object
template<typename T>
ManagedComPtr<T> makeComUnique( T* p )
{
	return ManagedComPtr<T>( p );
}

//! A COM smart pointer matching Microsoft::WRL::ComPtr semantics for compatibility
/*! Manages COM interface lifetime with automatic AddRef/Release. Key semantics:
	- Construction/assignment from T* adopts ownership (no AddRef) - matches COM API returns
	- Copy construction/assignment does AddRef (sharing ownership)
	- Move operations transfer ownership without ref-counting */
template<typename T>
class CI_API ComPtr {
  public:
	ComPtr() noexcept
		: ptr( nullptr )
	{
	}

	//! Construct from raw pointer - ADOPTS ownership without AddRef (WRL semantics)
	ComPtr( T* p ) noexcept
		: ptr( p )
	{
		// No AddRef - assumes p already has correct refcount (e.g., from COM API)
	}

	//! Copy constructor - shares ownership with AddRef
	ComPtr( const ComPtr& other ) noexcept
		: ptr( other.ptr )
	{
		if( ptr )
			ptr->AddRef();
	}

	//! Move constructor - transfers ownership without AddRef
	ComPtr( ComPtr&& other ) noexcept
		: ptr( other.ptr )
	{
		other.ptr = nullptr;
	}

	~ComPtr()
	{
		if( ptr )
			ptr->Release();
	}

	//! Assign from raw pointer - ADOPTS ownership without AddRef (WRL semantics)
	ComPtr& operator=( T* p ) noexcept
	{
		if( ptr != p ) {
			if( ptr )
				ptr->Release();
			ptr = p; // No AddRef - adopts ownership
		}
		return *this;
	}

	//! Copy assignment - shares ownership with AddRef
	ComPtr& operator=( const ComPtr& other ) noexcept
	{
		if( ptr != other.ptr ) {
			if( ptr )
				ptr->Release();
			ptr = other.ptr;
			if( ptr )
				ptr->AddRef();
		}
		return *this;
	}

	//! Move assignment - transfers ownership without AddRef
	ComPtr& operator=( ComPtr&& other ) noexcept
	{
		if( this != &other ) {
			if( ptr )
				ptr->Release();
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}

	// Accessors
	T*		 get() const noexcept { return ptr; }
	T*		 operator->() const noexcept { return ptr; }
	explicit operator bool() const noexcept { return ptr != nullptr; }

	//! Get address of internal pointer WITHOUT releasing - for APIs that check for existing value
	T** getAddressOf() noexcept { return &ptr; }

	//! Release current pointer and return address - for output parameters (COM "put" pattern)
	T** releaseAndGetAddressOf() noexcept
	{
		if( ptr ) {
			ptr->Release();
			ptr = nullptr;
		}
		return &ptr;
	}

	//! Adopt ownership of raw pointer without AddRef
	void attach( T* p ) noexcept
	{
		if( ptr )
			ptr->Release();
		ptr = p; // No AddRef - takes ownership
	}

	//! Release ownership without calling Release
	T* detach() noexcept
	{
		T* temp = ptr;
		ptr = nullptr;
		return temp;
	}

	//! Release current pointer to null (no parameters)
	void reset() noexcept
	{
		if( ptr ) {
			ptr->Release();
			ptr = nullptr;
		}
	}

	//! Copy interface pointer to output parameter with AddRef
	void copyTo( T** ppOut ) const noexcept
	{
		if( ppOut ) {
			*ppOut = ptr;
			if( ptr )
				ptr->AddRef();
		}
	}

	//! Query for different interface
	template<typename U>
	HRESULT as( ComPtr<U>* out ) const noexcept
	{
		if( ! out )
			return E_POINTER;
		return ptr ? ptr->QueryInterface( __uuidof( U ), reinterpret_cast<void**>( out->releaseAndGetAddressOf() ) ) : E_POINTER;
	}

	//! Query for different interface (raw pointer out)
	template<typename U>
	HRESULT as( U** out ) const noexcept
	{
		if( ! out )
			return E_POINTER;
		*out = nullptr;
		return ptr ? ptr->QueryInterface( __uuidof( U ), reinterpret_cast<void**>( out ) ) : E_POINTER;
	}

	//! Swap pointers with another ComPtr
	void swap( ComPtr& other ) noexcept
	{
		T* temp = ptr;
		ptr = other.ptr;
		other.ptr = temp;
	}

	// Comparison operators
	bool operator==( const ComPtr& other ) const noexcept { return ptr == other.ptr; }
	bool operator!=( const ComPtr& other ) const noexcept { return ptr != other.ptr; }
	bool operator==( T* p ) const noexcept { return ptr == p; }
	bool operator!=( T* p ) const noexcept { return ptr != p; }

  private:
	T* ptr;
};

//! Wraps a cinder::OStream with a COM ::IStream
class CI_API ComOStream : public ::IStream {
  public:
	ComOStream( cinder::OStreamRef aOStream )
		: mOStream( aOStream )
		, _refcount( 1 )
	{
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void** ppvObject );
	virtual ULONG STDMETHODCALLTYPE	  AddRef();
	virtual ULONG STDMETHODCALLTYPE	  Release();

	// ISequentialStream Interface
  public:
	virtual HRESULT STDMETHODCALLTYPE Read( void* /*pv*/, ULONG /*cb*/, ULONG* /*pcbRead*/ ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Write( void const* pv, ULONG cb, ULONG* pcbWritten );
	// IStream Interface
  public:
	virtual HRESULT STDMETHODCALLTYPE SetSize( ULARGE_INTEGER ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CopyTo( ::IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER* ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Commit( DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Revert() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE LockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Clone( IStream** ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Seek( LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer );
	virtual HRESULT STDMETHODCALLTYPE Stat( STATSTG* /*pStatstg*/, DWORD /*grfStatFlag*/ ) { return E_NOTIMPL; }

  private:
	cinder::OStreamRef mOStream;
	LONG			   _refcount;
};

//! Wraps a cinder::IStream with a COM ::IStream
class CI_API ComIStream : public ::IStream {
  public:
	ComIStream( cinder::IStreamRef aIStream )
		: mIStream( aIStream )
		, _refcount( 1 )
	{
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID iid, void** ppvObject );
	virtual ULONG STDMETHODCALLTYPE	  AddRef();
	virtual ULONG STDMETHODCALLTYPE	  Release();

	// ISequentialStream Interface
  public:
	virtual HRESULT STDMETHODCALLTYPE Read( void* pv, ULONG cb, ULONG* pcbRead );
	virtual HRESULT STDMETHODCALLTYPE Write( void const* /*pv*/, ULONG /*cb*/, ULONG* /*pcbWritten*/ ) { return E_NOTIMPL; }
	// IStream Interface
  public:
	virtual HRESULT STDMETHODCALLTYPE SetSize( ULARGE_INTEGER ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE CopyTo( ::IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER* ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Commit( DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Revert() { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE LockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( ULARGE_INTEGER, ULARGE_INTEGER, DWORD ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Clone( IStream** ) { return E_NOTIMPL; }
	virtual HRESULT STDMETHODCALLTYPE Seek( LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer );
	virtual HRESULT STDMETHODCALLTYPE Stat( STATSTG* pStatstg, DWORD grfStatFlag );

  private:
	cinder::IStreamRef mIStream;
	LONG			   _refcount;
};

//! Initializes COM on this thread. Uses thread local storage to prevent multiple initializations per thread
CI_API void initializeCom( DWORD params = COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );
}
} // namespace cinder::msw
