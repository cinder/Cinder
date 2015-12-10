/*
Copyright (c) 2015, The Barbarian Group
All rights reserved.

Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include <assert.h>
#include <windows.h>

namespace cinder {
namespace msw {

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg)     static_assert(expr, #msg)

// A smart pointer class for reference counted objects.  Use this class instead
// of calling AddRef and Release manually on a reference counted object to
// avoid common memory leaks caused by forgetting to Release an object
// reference.  Sample usage:
//
//   class MyFoo : public RefCounted<MyFoo> {
//    ...
//   };
//
//   void some_function() {
//     ScopedPtr<MyFoo> foo = new MyFoo();
//     foo->Method(param);
//     // |foo| is released when this function returns
//   }
//
//   void some_other_function() {
//     ScopedPtr<MyFoo> foo = new MyFoo();
//     ...
//     foo = nullptr;  // explicitly releases |foo|
//     ...
//     if (foo)
//       foo->Method(param);
//   }
//
// The above examples show how ScopedPtr<T> acts like a pointer to T.
// Given two ScopedPtr<T> classes, it is also possible to exchange
// references between the two objects, like so:
//
//   {
//     ScopedPtr<MyFoo> a = new MyFoo();
//     ScopedPtr<MyFoo> b;
//
//     b.swap(a);
//     // now, |b| references the MyFoo object, and |a| references nullptr.
//   }
//
// To make both |a| and |b| in the above example reference the same MyFoo
// object, simply use the assignment operator:
//
//   {
//     ScopedPtr<MyFoo> a = new MyFoo();
//     ScopedPtr<MyFoo> b;
//
//     b = a;
//     // now, |a| and |b| each own a reference to the same MyFoo object.
//   }
//
template <class T>
class ScopedPtr {
public:
	typedef T element_type;

	ScopedPtr() : ptr_( nullptr )
	{
	}

	ScopedPtr( T* p ) : ptr_( p )
	{
		if( ptr_ )
			AddRef( ptr_ );
	}

	ScopedPtr( const ScopedPtr<T>& r ) : ptr_( r.ptr_ )
	{
		if( ptr_ )
			AddRef( ptr_ );
	}

	template <typename U>
	ScopedPtr( const ScopedPtr<U>& r ) : ptr_( r.get() )
	{
		if( ptr_ )
			AddRef( ptr_ );
	}

	~ScopedPtr()
	{
		if( ptr_ )
			Release( ptr_ );
	}

	T* Get() const { return ptr_; }

	// Allow ScopedPtr<C> to be used in boolean expression
	// and comparison operations.
	operator T*( ) const { return ptr_; }

	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the ptr_ member explicitly.
	T** operator&() throw( )
	{
		assert( ptr_ == nullptr );
		return &ptr_;
	}

	T& operator*() const
	{
		assert( ptr_ != nullptr );
		return *ptr_;
	}

	T* operator->() const
	{
		assert( ptr_ != nullptr );
		return ptr_;
	}

	ScopedPtr<T>& operator=( T* p )
	{
		// AddRef first so that self assignment should work
		if( p )
			AddRef( p );
		T* old_ptr = ptr_;
		ptr_ = p;
		if( old_ptr )
			Release( old_ptr );
		return *this;
	}

	ScopedPtr<T>& operator=( const ScopedPtr<T>& r )
	{
		return *this = r.ptr_;
	}

	template <typename U>
	ScopedPtr<T>& operator=( const ScopedPtr<U>& r )
	{
		return *this = r.get();
	}

	void Swap( T** pp )
	{
		T* p = ptr_;
		ptr_ = *pp;
		*pp = p;
	}

	void Swap( ScopedPtr<T>& r )
	{
		Swap( &r.ptr_ );
	}

protected:
	T* ptr_;

private:
	// Non-inline helpers to allow:
	//     class Opaque;
	//     extern template class ScopedPtr<Opaque>;
	// Otherwise the compiler will complain that Opaque is an incomplete type.
	static void AddRef( T* ptr );
	static void Release( T* ptr );
};

template <typename T>
void ScopedPtr<T>::AddRef( T* ptr )
{
	ptr->AddRef();
}

template <typename T>
void ScopedPtr<T>::Release( T* ptr )
{
	ptr->Release();
}

// Handy utility for creating a ScopedPtr<T> out of a T* explicitly without
// having to retype all the template arguments
template <typename T>
ScopedPtr<T> makeScopedPtr( T* t )
{
	return ScopedPtr<T>( t );
}

//////////////////////////////////////////////////////////////////////////////////////

// A fairly minimalistic smart class for COM interface pointers.
// Uses ScopedPtr for the basic smart pointer functionality
// and adds a few IUnknown specific services.
template <class Interface, const IID* interface_id = &__uuidof( Interface )>
class ScopedComPtr : public ScopedPtr < Interface > {
public:
	// Utility template to prevent users of ScopedComPtr from calling AddRef
	// and/or Release() without going through the ScopedComPtr class.
	class BlockIUnknownMethods : public Interface {
	private:
		STDMETHOD( QueryInterface )( REFIID iid, void** object ) = 0;
		STDMETHOD_( ULONG, AddRef )( ) = 0;
		STDMETHOD_( ULONG, Release )( ) = 0;
	};

	typedef ScopedPtr<Interface> ParentClass;

	ScopedComPtr()
	{
	}

	explicit ScopedComPtr( Interface* p ) : ParentClass( p )
	{
	}

	ScopedComPtr( const ScopedComPtr<Interface, interface_id>& p )
		: ParentClass( p )
	{
	}

	~ScopedComPtr()
	{
		// We don't want the smart pointer class to be bigger than the pointer
		// it wraps.
		COMPILE_ASSERT( sizeof( ScopedComPtr<Interface, interface_id> ) ==
						sizeof( Interface* ), ScopedComPtrSize );
	}

	// Explicit Release() of the held object.  Useful for reuse of the
	// ScopedComPtr instance.
	// Note that this function equates to IUnknown::Release and should not
	// be confused with e.g. scoped_ptr::release().
	void Release()
	{
		if( ptr_ != nullptr ) {
			ptr_->Release();
			ptr_ = nullptr;
		}
	}

	// Sets the internal pointer to nullptr and returns the held object without
	// releasing the reference.
	Interface* Detach()
	{
		Interface* p = ptr_;
		ptr_ = nullptr;
		return p;
	}

	// Accepts an interface pointer that has already been addref-ed.
	void Attach( Interface* p )
	{
		assert( !ptr_ );
		ptr_ = p;
	}

	// Retrieves the pointer address.
	// Used to receive object pointers as out arguments (and take ownership).
	// The function asserts on the current value being nullptr.
	// Usage: Foo(p.Receive());
	Interface** Receive()
	{
		assert( !ptr_ ) << "Object leak. Pointer must be nullptr.";
		return &ptr_;
	}

	// A convenience for whenever a void pointer is needed as an out argument.
	void** ReceiveVoid()
	{
		return reinterpret_cast<void**>( Receive() );
	}

	template <class Query>
	HRESULT QueryInterface( Query** p )
	{
		assert( p != nullptr );
		assert( ptr_ != nullptr );
		// IUnknown already has a template version of QueryInterface
		// so the iid parameter is implicit here. The only thing this
		// function adds are the asserts.
		return ptr_->QueryInterface( p );
	}

	// QI for times when the IID is not associated with the type.
	HRESULT QueryInterface( const IID& iid, void** obj )
	{
		assert( obj != nullptr );
		assert( ptr_ != nullptr );
		return ptr_->QueryInterface( iid, obj );
	}

	// Queries |other| for the interface this object wraps and returns the
	// error code from the other->QueryInterface operation.
	HRESULT QueryFrom( IUnknown* object )
	{
		assert( object != nullptr );
		return object->QueryInterface( Receive() );
	}

	// Convenience wrapper around CoCreateInstance
	HRESULT CreateInstance( const CLSID& clsid, IUnknown* outer = nullptr, DWORD context = CLSCTX_ALL )
	{
		assert( !ptr_ );
		HRESULT hr = ::CoCreateInstance( clsid, outer, context, *interface_id,
										 reinterpret_cast<void**>( &ptr_ ) );
		return hr;
	}

	// Checks if the identity of |other| and this object is the same.
	bool IsSameObject( IUnknown* other )
	{
		if( !other && !ptr_ )
			return true;

		if( !other || !ptr_ )
			return false;

		ScopedComPtr<IUnknown> my_identity;
		QueryInterface( my_identity.Receive() );

		ScopedComPtr<IUnknown> other_identity;
		other->QueryInterface( other_identity.Receive() );

		return static_cast<IUnknown*>( my_identity ) ==
			static_cast<IUnknown*>( other_identity );
	}

	//! Returns the current reference count. Use for debugging purposes only.
	ULONG GetRefCount()
	{
		if( !ptr_ )
			return 0;

		ptr_->AddRef();
		ULONG rc = ptr_->Release();

		return rc;
	}

	// Provides direct access to the interface.
	// Here we use a well known trick to make sure we block access to
	// IUnknown methods so that something bad like this doesn't happen:
	//    ScopedComPtr<IUnknown> p(Foo());
	//    p->Release();
	//    ... later the destructor runs, which will Release() again.
	// and to get the benefit of the asserts we add to QueryInterface.
	// There's still a way to call these methods if you absolutely must
	// by statically casting the ScopedComPtr instance to the wrapped interface
	// and then making the call... but generally that shouldn't be necessary.
	BlockIUnknownMethods* operator->() const
	{
		assert( ptr_ != nullptr );
		return reinterpret_cast<BlockIUnknownMethods*>( ptr_ );
	}

	// Pull in operator=() from the parent class.
	using ScopedPtr<Interface>::operator=;

	// Pull in operator&() from the parent class.
	using ScopedPtr<Interface>::operator&;

	static const IID& iid() { return *interface_id; }
};

}
} // namespace ci::msw