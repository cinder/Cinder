/*
 Copyright (c) 2009, The Barbarian Group
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

#include "cinder/Serial.h"
#include "cinder/Timer.h"
#include "cinder/Thread.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"

#include <string>
#include <iostream>
#include <fcntl.h>

#if defined( CINDER_MAC )
	#include <termios.h>
	#include <sys/ioctl.h>
	#include <getopt.h>
	#include <dirent.h>
#elif defined( CINDER_MSW )
	#include <windows.h>
	#include <setupapi.h>
	#pragma comment(lib, "setupapi.lib")
#endif

#include <map>
using namespace std;

namespace cinder {

bool							Serial::sDevicesInited = false;
std::vector<Serial::Device>		Serial::sDevices;

struct Serial::Impl {
	Impl( const Serial::Device &device, int baudRate );
	~Impl();

#ifdef CINDER_MSW
	::HANDLE		mDeviceHandle;
	::COMMTIMEOUTS 	mSavedTimeouts;
#else
	int				mFd;
	::termios		mSavedOptions;
#endif
};

Serial::Serial( const Serial::Device &device, int baudRate )
	: mDevice( device ), mImpl( new Impl( device, baudRate ) )
{
}

Serial::~Serial()
{
}

Serial::Impl::Impl( const Serial::Device &device, int baudRate )
{
#if defined( CINDER_MAC )
	mFd = open( ( "/dev/" + device.getName() ).c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
	if( mFd == -1 ) {
		throw SerialExcOpenFailed();
	}
	
	termios options;
	tcgetattr( mFd, &mSavedOptions );
	options = mSavedOptions;
	map<int,int> baudToConstant;
	baudToConstant[300] = B300;
	baudToConstant[1200] = B1200;
	baudToConstant[2400] = B2400;
	baudToConstant[4800] = B4800;
	baudToConstant[9600] = B9600;			
	baudToConstant[19200] = B19200;
	baudToConstant[28800] = B28800;
	baudToConstant[38400] = B38400;
	baudToConstant[57600] = B57600;
	baudToConstant[115200] = B115200;
	baudToConstant[230400] = B230400;	
	
	int rateConstant = B9600;
	if( baudToConstant.find( baudRate ) != baudToConstant.end() )
		rateConstant = baudToConstant[baudRate];
	
	::cfsetispeed( &options, rateConstant );
	::cfsetospeed( &options, rateConstant );
	
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	::tcsetattr( mFd, TCSANOW, &options );
#elif defined( CINDER_MSW )
	mDeviceHandle = ::CreateFileA( device.getPath().c_str(), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0 );
	if( mDeviceHandle == INVALID_HANDLE_VALUE ) {
		throw SerialExcOpenFailed();
	}
	
	::COMMCONFIG config;
	::DWORD configSize = sizeof( ::COMMCONFIG );
	::GetCommConfig( mDeviceHandle, &config, &configSize );
	
	string settingsStr = string("baud=") + toString( baudRate ) + " parity=N data=8 stop=1";
	if( ! ::BuildCommDCBA( settingsStr.c_str(), &config.dcb ) ) {
		throw SerialExcOpenFailed();	
	}	
	
	if( ! ::SetCommState( mDeviceHandle, &config.dcb ) ) {
		throw SerialExcOpenFailed();
	}
	
	::GetCommTimeouts( mDeviceHandle, &mSavedTimeouts );
	::COMMTIMEOUTS timeOuts( mSavedTimeouts );

	timeOuts.ReadIntervalTimeout = MAXDWORD;
	timeOuts.ReadTotalTimeoutMultiplier = 0;
	timeOuts.ReadTotalTimeoutConstant = 0;
	::SetCommTimeouts( mDeviceHandle, &timeOuts );
#endif
}

Serial::Impl::~Impl()
{
#if defined( CINDER_MAC )
	// restore the termios from before we opened the port
	::tcsetattr( mFd, TCSANOW, &mSavedOptions );
	::close( mFd );
#elif defined( CINDER_MSW )
	::SetCommTimeouts( mDeviceHandle, &mSavedTimeouts );
	::CloseHandle( mDeviceHandle );
#endif
}

Serial::Device Serial::findDeviceByName( const std::string &name, bool forceRefresh )
{
	const std::vector<Serial::Device> &devices = getDevices( forceRefresh );
	for( std::vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		if( deviceIt->getName() == name )
			return *deviceIt;
	}
	
	return Serial::Device();
}

Serial::Device Serial::findDeviceByNameContains( const std::string &searchString, bool forceRefresh )
{
	const std::vector<Serial::Device> &devices = getDevices( forceRefresh );
	for( std::vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		if( deviceIt->getName().find( searchString ) != std::string::npos )
			return *deviceIt;
	}
	
	return Serial::Device();
}
	
const std::vector<Serial::Device>& Serial::getDevices( bool forceRefresh )
{
	if( ( ! forceRefresh ) && ( sDevicesInited ) )
		return sDevices;

	sDevices.clear();

#if defined( CINDER_MAC )	
	::DIR *dir;
	::dirent *entry;
	dir = ::opendir( "/dev" );

	if( ! dir ) {
		throw SerialExcDeviceEnumerationFailed();
	}
	else {
		while( ( entry = ::readdir( dir ) ) != NULL ) {
			std::string str( (char *)entry->d_name );
			if( ( str.substr( 0, 4 ) == "tty." ) || ( str.substr( 0, 3 ) == "cu." ) ) {
				sDevices.push_back( Serial::Device( str ) );
			}
		}
	}
	
	::closedir( dir );
#elif defined( CINDER_MSW )
	::HDEVINFO devInfoSet;
	::DWORD devCount = 0;
	::SP_DEVINFO_DATA devInfo;
	::SP_DEVICE_INTERFACE_DATA devInterface;
	DWORD size = 0;

	devInfoSet = ::SetupDiGetClassDevs( &GUID_SERENUM_BUS_ENUMERATOR, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if( devInfoSet == INVALID_HANDLE_VALUE )
		throw SerialExcDeviceEnumerationFailed();
	
	devInterface.cbSize = sizeof( ::SP_DEVICE_INTERFACE_DATA );
	while( ::SetupDiEnumDeviceInterfaces( devInfoSet, 0, &GUID_SERENUM_BUS_ENUMERATOR, devCount++, &devInterface ) ) {
		// See how large a buffer we require for the device interface details
		::SetupDiGetDeviceInterfaceDetail( devInfoSet, &devInterface, 0, 0, &size, 0 );
		devInfo.cbSize = sizeof( ::SP_DEVINFO_DATA );
		shared_ptr<::SP_DEVICE_INTERFACE_DETAIL_DATA> interfaceDetail( (::SP_DEVICE_INTERFACE_DETAIL_DATA*)calloc( 1, size ), free );
		if( interfaceDetail ) {
			interfaceDetail->cbSize = sizeof( ::SP_DEVICE_INTERFACE_DETAIL_DATA );
			devInfo.cbSize = sizeof( ::SP_DEVINFO_DATA );
			if( ! ::SetupDiGetDeviceInterfaceDetail( devInfoSet, &devInterface, interfaceDetail.get(), size, 0, &devInfo ) ) {
				continue;
			}

			char friendlyName[2048];
			size = sizeof( friendlyName );
			friendlyName[0] = 0;
			::DWORD propertyDataType;
			if( ! ::SetupDiGetDeviceRegistryPropertyA( devInfoSet, &devInfo, SPDRP_FRIENDLYNAME, &propertyDataType, (LPBYTE)friendlyName, size, 0 ) ) {
				continue;
			}

			sDevices.push_back( Serial::Device( string( friendlyName ), toUtf8( (char16_t*)interfaceDetail->DevicePath ) ) );
		}
	}
	
	::SetupDiDestroyDeviceInfoList(devInfoSet);

#endif

	sDevicesInited = true;
	return sDevices;
}

const Serial::Device& Serial::getDevice() const
{
	return mDevice;
}

void Serial::writeBytes( const void *data, size_t numBytes )
{
	size_t totalBytesWritten = 0;
	
	while( totalBytesWritten < numBytes ) {
#if defined( CINDER_MAC )
		long bytesWritten = ::write( mImpl->mFd, data, numBytes - totalBytesWritten );
		if( ( bytesWritten == -1 ) && ( errno != EAGAIN ) )
			throw SerialExcWriteFailure();
#elif defined( CINDER_MSW )
		::DWORD bytesWritten;
		if( ! ::WriteFile( mImpl->mDeviceHandle, data, numBytes - totalBytesWritten, &bytesWritten, 0 ) )
			throw SerialExcWriteFailure();
#endif
		if( bytesWritten != -1 )
			totalBytesWritten += bytesWritten;
	}
}

void Serial::readBytes( void *data, size_t numBytes )
{
	size_t totalBytesRead = 0;
	while( totalBytesRead < numBytes ) {
#if defined( CINDER_MAC )
		long bytesRead = ::read( mImpl->mFd, data, numBytes - totalBytesRead );
		if( ( bytesRead == -1 ) && ( errno != EAGAIN ) )
			throw SerialExcReadFailure();
#elif defined( CINDER_MSW )
		::DWORD bytesRead = 0;
		if( ! ::ReadFile( mImpl->mDeviceHandle, data, numBytes - totalBytesRead, &bytesRead, 0 ) )
			throw SerialExcReadFailure();
#endif
		if( bytesRead != -1 )
			totalBytesRead += bytesRead;
		
		// yield thread time to the system
		this_thread::yield();
	}
}

size_t Serial::readAvailableBytes( void *data, size_t maximumBytes )
{
#if defined( CINDER_MAC )
	long bytesRead = ::read( mImpl->mFd, data, maximumBytes );
#elif defined( CINDER_MSW )
	::DWORD bytesRead = 0;
	if( ! ::ReadFile( mImpl->mDeviceHandle, data, maximumBytes, &bytesRead, 0 ) )
		throw SerialExcReadFailure();
#endif

	if( bytesRead < 0 )
		bytesRead = 0;
		
	return (size_t)bytesRead;
}

void Serial::writeByte( uint8_t data )
{
	writeBytes( &data, 1 );
}

uint8_t Serial::readByte()
{
	uint8_t result;
	readBytes( &result, 1 );
	return result;
}

std::string Serial::readStringUntil( char token, size_t maxLength, double timeoutSeconds )
{
	size_t bufferSize = 1024, bufferOffset = 0;
	shared_ptr<char> buffer( (char*)malloc( bufferSize ), free );

	bool useMaxLength = maxLength > 0;
	bool useTimer = timeoutSeconds > 0;
	Timer timer;
	if( useTimer )
		timer.start();

	bool done = false;
	while( ! done ) {
		char v = readChar();
		buffer.get()[bufferOffset++] = v;
		if( v == token ) {
			done = true;
		}
		else if( useMaxLength && ( bufferOffset == maxLength ) ) {
			done = true;
		}
		else if( useTimer && ( timer.getSeconds() > timeoutSeconds ) )
			throw SerialTimeoutExc();

		// we need to reallocate even if this happens to be the last byte, because we need room for a null-terminator
		if( bufferOffset == bufferSize ) {
			char *newBuffer = (char*)malloc( bufferSize * 2 );
			memcpy( newBuffer, buffer.get(), bufferSize );
			bufferSize *= 2;
			buffer = shared_ptr<char>( newBuffer, free );
		}
	}

	buffer.get()[bufferOffset] = 0; // need to null terminate this thing
	std::string result( buffer.get() );

	return result;
}

void Serial::writeString( const std::string &str )
{
	for( string::const_iterator strIt = str.begin(); strIt != str.end(); ++strIt )
		writeByte( *strIt );
}

size_t Serial::getNumBytesAvailable() const
{
	int result;
	
#if defined( CINDER_MAC )
	::ioctl( mImpl->mFd, FIONREAD, &result );
#elif defined( CINDER_MSW )
	::COMSTAT status;
	::DWORD error;
	if( ! ::ClearCommError( mImpl->mDeviceHandle, &error, &status ) )
		throw SerialExc( "Serial failure upon attempt to retrieve information on device handle" );
	else
		result = status.cbInQue;
#endif
	
	return result;
}
	
void Serial::flush( bool input, bool output )
{
#if defined( CINDER_MAC )
	int queue;
	if( input && output )
		queue = TCIOFLUSH;
	else if( input )
		queue = TCIFLUSH;
	else if( output )
		queue = TCOFLUSH;
	else
		return;
	
	::tcflush( mImpl->mFd, queue );
#elif defined( CINDER_MSW )
	::DWORD flags = 0;
	flags |= ( input ) ? PURGE_RXCLEAR : 0;
	flags |= ( output ) ? PURGE_TXCLEAR : 0;
	
	if( input || output )
		::PurgeComm( mImpl->mDeviceHandle, flags );
#endif
}

} // namespace cinder
