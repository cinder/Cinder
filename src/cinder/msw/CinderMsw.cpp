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

#include "cinder/msw/CinderMsw.h"
#include "cinder/ip/Fill.h"

#include <vector>

#if !defined( CINDER_WINRT )
#include <boost/thread/tss.hpp>
#endif

namespace cinder { namespace msw {

#if !defined( CINDER_WINRT )
static void surfaceDeallocatorGlobalAlloc( void *refcon )
{
	::GlobalFree( (HGLOBAL)refcon );
}

Surface8u convertHBitmap( HBITMAP hbitmap )
{
	// create a temporary DC
	HDC hdc = ::CreateCompatibleDC( 0 );
	
	// determine the dimensions first
	BITMAP bm;
	::GetObject( hbitmap, sizeof(BITMAP), &bm );
	
	BITMAPINFO bmi;
	memset( &bmi, 0, sizeof(BITMAPINFO) );
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;

	int width = bmi.bmiHeader.biWidth;
	int height = -bmi.bmiHeader.biHeight;

	// allocate enough space to hold the result
	// We use GlobalAlloc / GlobalFree to ensure 8-byte alignment
	DWORD dwBmpSize = ((width * 32 + 31) / 32) * 4 * height;
	uint8_t *data = reinterpret_cast<uint8_t*>( ::GlobalAlloc( GMEM_FIXED, dwBmpSize ) );
	Surface8u result = Surface8u( data, width, height, width * 4, SurfaceChannelOrder::BGRX );
	// have the Surface's destructor call this to call ::GlobalFree
	result.setDeallocator( surfaceDeallocatorGlobalAlloc, data );
	
	if( ::GetDIBits( hdc, hbitmap, 0, height, result.getData(), &bmi, DIB_RGB_COLORS ) == 0 )
		throw std::exception( "Invalid HBITMAP" );
	
	::ReleaseDC( NULL, hdc );

	return result;
}
#endif

void ComDelete( void *p )
{
	if( p ) {
		IUnknown *unknown = reinterpret_cast<IUnknown*>( p );
		unknown->Release();
	}
}

HRESULT STDMETHODCALLTYPE ComOStream::QueryInterface(REFIID iid, void ** ppvObject)
{ 
    if (iid == __uuidof(IUnknown)
        || iid == __uuidof(IStream)
        || iid == __uuidof(ISequentialStream))
    {
        *ppvObject = static_cast<IStream*>(this);
        AddRef();
        return S_OK;
    }
    else
        return E_NOINTERFACE; 
}

ULONG STDMETHODCALLTYPE ComOStream::AddRef() 
{ 
    return (ULONG)InterlockedIncrement(&_refcount); 
}

ULONG STDMETHODCALLTYPE ComOStream::Release() 
{
    ULONG res = (ULONG) InterlockedDecrement(&_refcount);
    if (res == 0) 
        delete this;
    return res;
}

HRESULT STDMETHODCALLTYPE ComOStream::Write( void const* pv, ULONG cb, ULONG* pcbWritten )
{
	mOStream->writeData( pv, cb );
	*pcbWritten = cb;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ComOStream::Seek( LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer )
{ 
	switch( dwOrigin ) {
		case STREAM_SEEK_SET:
			mOStream->seekAbsolute( (off_t)liDistanceToMove.QuadPart );
		break;
		case STREAM_SEEK_CUR:
			mOStream->seekRelative( (off_t)liDistanceToMove.QuadPart );
		break;
		case STREAM_SEEK_END:
			mOStream->seekAbsolute( (off_t)(-liDistanceToMove.QuadPart) );
		break;
		default:   
			return STG_E_INVALIDFUNCTION;
		break;
	}

	lpNewFilePointer->QuadPart = mOStream->tell();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////
// ComInitializer
struct ComInitializer {
	ComInitializer( DWORD params ) {
		::CoInitializeEx( NULL, params );
	}

	~ComInitializer()
	{
		::CoUninitialize();
	}
};

#if defined( CINDER_WINRT )

void initializeCom( DWORD params )
{
	::CoInitializeEx( NULL, params );
}
#else
boost::thread_specific_ptr<ComInitializer> threadComInitializer;

void initializeCom( DWORD params )
{
	if( threadComInitializer.get() == NULL )
		threadComInitializer.reset( new ComInitializer( params ) );
}
#endif

} } // namespace cinder::msw
