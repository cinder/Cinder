// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//--------------------------------------------------------------------------------------
// File: PlatformHelpers.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//--------------------------------------------------------------------------------------

#pragma once

#pragma warning(disable : 4324)

#include <exception>

#if defined(_DEBUG) || defined(PROFILE)
//#pragma comment(lib,"dxguid.lib")
#endif

namespace DirectX
{
    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw std::exception();
        }
    }


    // Helper sets a D3D resource name string (used by PIX and debug layer leak reporting).
    template<UINT TNameLength>
    inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ const char (&name)[TNameLength])
    {
        #if defined(_DEBUG) || defined(PROFILE)
            resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
        #else
            UNREFERENCED_PARAMETER(resource);
            UNREFERENCED_PARAMETER(name);
        #endif
    }


    // Helper smart-pointers
    struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };

    typedef public std::unique_ptr<void, handle_closer> ScopedHandle;

    inline HANDLE safe_handle( HANDLE h ) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }


    template<class T> class ScopedObject
    {
    public:
        explicit ScopedObject( T *p = 0 ) : _pointer(p) {}
        ~ScopedObject()
        {
            if ( _pointer )
            {
                _pointer->Release();
                _pointer = nullptr;
            }
        }

        bool IsNull() const { return (!_pointer); }

        T& operator*() { return *_pointer; }
        T* operator->() { return _pointer; }
        T** operator&() { return &_pointer; }

        void Reset(T *p = 0) { if ( _pointer ) { _pointer->Release(); } _pointer = p; }

        T* Get() const { return _pointer; }

    private:
        ScopedObject(const ScopedObject&);
        ScopedObject& operator=(const ScopedObject&);
        
        T* _pointer;
    };
}


#if defined(_MSC_VER) && (_MSC_VER < 1610)

// Emulate the C++0x mutex and lock_guard types when building with Visual Studio versions < 2012.
namespace std
{
    class mutex
    {
    public:
        mutex()         { InitializeCriticalSection(&mCriticalSection); }
        ~mutex()        { DeleteCriticalSection(&mCriticalSection); }

        void lock()     { EnterCriticalSection(&mCriticalSection); }
        void unlock()   { LeaveCriticalSection(&mCriticalSection); }
        bool try_lock() { return TryEnterCriticalSection(&mCriticalSection) != 0; }

    private:
        CRITICAL_SECTION mCriticalSection;

        mutex(mutex const&);
        mutex& operator= (mutex const&);
    };


    template<typename Mutex>
    class lock_guard
    {
    public:
        typedef Mutex mutex_type;

        explicit lock_guard(mutex_type& mutex)
          : mMutex(mutex)
        {
            mMutex.lock();
        }

        ~lock_guard()
        {
            mMutex.unlock();
        }

    private:
        mutex_type& mMutex;

        lock_guard(lock_guard const&);
        lock_guard& operator= (lock_guard const&);
    };
}

#else   // _MSC_VER < 1610

#include <mutex>

#endif
