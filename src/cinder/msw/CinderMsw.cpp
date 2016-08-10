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
#include "cinder/Exception.h"

#include <vector>

namespace cinder { namespace msw {

#if ! defined( CINDER_UWP )
Surface8uRef convertHBitmap( HBITMAP hbitmap )
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
	Surface8u *newSurface = new Surface8u( data, width, height, width * 4, SurfaceChannelOrder::BGRX );
	Surface8uRef result( newSurface, [=] ( Surface8u *s ) { ::GlobalFree( (HGLOBAL)data ); delete s; } );
	// have the Surface's destructor call this to call ::GlobalFree
	
	if( ::GetDIBits( hdc, hbitmap, 0, height, result->getData(), &bmi, DIB_RGB_COLORS ) == 0 )
		throw Exception( "Invalid HBITMAP" );
	
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

/////////////////////////////////////////////////////////////////////
// ComOStream

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

std::wstring toWideString( const std::string &utf8String )
{
	int wideSize = ::MultiByteToWideChar( CP_UTF8, 0, utf8String.c_str(), -1, NULL, 0 );
	if( wideSize == ERROR_NO_UNICODE_TRANSLATION ) {
		throw Exception( "Invalid UTF-8 sequence." );
	}
	else if( wideSize == 0 ) {
		throw Exception( "Error in UTF-8 to UTF-16 conversion." );
	}

	std::vector<wchar_t> resultString( wideSize );
	int convResult = ::MultiByteToWideChar( CP_UTF8, 0, utf8String.c_str(), -1, &resultString[0], wideSize );
	if( convResult != wideSize ) {
		throw Exception( "Error in UTF-8 to UTF-16 conversion." );
	}

	return std::wstring( &resultString[0] );
}

std::string toUtf8String( const std::wstring &wideString )
{
	int utf8Size = ::WideCharToMultiByte( CP_UTF8, 0, wideString.c_str(), -1, NULL, 0, NULL, NULL );
	if( utf8Size == 0 ) {
		throw Exception( "Error in UTF-16 to UTF-8 conversion." );
	}

	std::vector<char> resultString( utf8Size );

	int convResult = ::WideCharToMultiByte( CP_UTF8, 0, wideString.c_str(), -1, &resultString[0], utf8Size, NULL, NULL );

	if( convResult != utf8Size ) {
		throw Exception( "Error in UTF-16 to UTF-8 conversion." );
	}

	return std::string( &resultString[0] );
}

/////////////////////////////////////////////////////////////////////
// ComIStream

HRESULT STDMETHODCALLTYPE ComIStream::QueryInterface(REFIID iid, void ** ppvObject)
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

ULONG STDMETHODCALLTYPE ComIStream::AddRef() 
{ 
	return (ULONG)InterlockedIncrement(&_refcount); 
}

ULONG STDMETHODCALLTYPE ComIStream::Release() 
{
	ULONG res = (ULONG) InterlockedDecrement(&_refcount);
	if (res == 0) 
		delete this;
	return res;
}

HRESULT STDMETHODCALLTYPE ComIStream::Read( void* pv, ULONG cb, ULONG* pcbRead )
{
	ULONG dataLeft = mIStream->size() - mIStream->tell();
	if( dataLeft < cb ) {
		mIStream->readData( pv, dataLeft );
		*pcbRead = dataLeft;
		return S_FALSE;
	}
	else {
		mIStream->readData( pv, cb );
		*pcbRead = cb;
		return S_OK;
	}
}

HRESULT STDMETHODCALLTYPE ComIStream::Seek( LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer )
{ 
	switch( dwOrigin ) {
	case STREAM_SEEK_SET:
		mIStream->seekAbsolute( (off_t)liDistanceToMove.QuadPart );
		break;
	case STREAM_SEEK_CUR:
		mIStream->seekRelative( (off_t)liDistanceToMove.QuadPart );
		break;
	case STREAM_SEEK_END:
		mIStream->seekAbsolute( (off_t)( - liDistanceToMove.QuadPart ) );
		break;
	default:   
		return STG_E_INVALIDFUNCTION;
		break;
	}
	off_t pos = mIStream->tell();
	if( lpNewFilePointer )
		lpNewFilePointer->QuadPart = pos;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE ComIStream::Stat( STATSTG* pStatstg, DWORD grfStatFlag)
{
	pStatstg->pwcsName = NULL;
	pStatstg->cbSize.QuadPart = (ULONGLONG)mIStream->size();

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

#if defined( CINDER_UWP )

void initializeCom( DWORD params )
{
	::CoInitializeEx( NULL, params );
}
#else
__declspec(thread) ComInitializer *threadComInitializer = nullptr;

void initializeCom(DWORD params)
{
	if( ! threadComInitializer )
		threadComInitializer = new ComInitializer(params);
}
#endif

} } // namespace cinder::msw
