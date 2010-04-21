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


#include "cinder/Cinder.h"
#include "cinder/Exception.h"

#include <string>
#include <vector>

#if defined( CINDER_MAC )
	#include <termios.h>
#elif defined( CINDER_MSW )
	#include <windows.h>
#endif

namespace cinder {

class Serial {
  public:
	class Device {
	  public:
		Device() {}
		Device( const std::string &nameAndPath ) : mName( nameAndPath ), mPath( nameAndPath ) {}
		Device( const std::string &name, const std::string &path ) : mName( name ), mPath( path ) {}
		
		const std::string& getName() const { return mName; }
		const std::string& getPath() const { return mPath; }

	  private:
		std::string		mName;
		std::string		mPath;
	};
	
	//! Returns a vector of all serial devices available on the machine. Uses a cached list unless \a forceRefresh
	static const std::vector<Serial::Device>& getDevices( bool forceRefresh = false );
	//! Returns the first Serial::Device whose name is \a name. Returns a null Serial::Device if none are found. Uses a cached list of the serial devices unless \a forceRefresh.
	static Serial::Device findDeviceByName( const std::string &name, bool forceRefresh = false );
	//! Returns the first Serial::Device whose name contains the string \a searchString. Returns a null Serial::Device if none are found. Uses a cached list of the serial devices unless \a forceRefresh.
	static Serial::Device findDeviceByNameContains( const std::string &searchString, bool forceRefresh = false );
	
	
	Serial() {}
	Serial( const Serial::Device &device, int baudRate );

	//! Returns the Device associated with this Serial port
	const Device&	getDevice() const;
	
	//! Reads \a numBytes bytes of data from the serial port to \a data.
	void	readBytes( void *data, size_t numBytes );
	//! Reads up to \a maximumBytes bytes of data from the serial port to \a data. Returns the number of bytes read.
	size_t	readAvailableBytes( void *data, size_t maximumBytes );
	//! Writes \a numBytes bytes of data to the serial port from \a data.
	void	writeBytes( const void *data, size_t numBytes );
	//! Writes a single byte \a data to the serial port.
	void	writeByte( uint8_t data );
	//! Returns a single byte read from the serial port
	uint8_t	readByte();
	//! Returns a single character read from the serial port
	char	readChar() { return static_cast<char>( readByte() ); }

	//! Returns a string composed of bytes read until a character \a token is found, or up to \a maxLength bytes have been read and \a maxLength > 0. Throws a SerialTimeoutExc() if timeoutSeconds > 0 and \a timeoutSeconds seconds pass before \a token is found.
	std::string	readStringUntil( char token, size_t maxLength = 0, double timeoutSeconds = -1.0 );
	//! Writes a string \a str to the serial port, excluding the null terminator
	void		writeString( const std::string &str );

	//! Forces the device to flush any buffered \a input and/or \a output bytes
	void	flush( bool input = true, bool output = true );
	//! Returns the number of bytes available for reading from the device
	size_t	getNumBytesAvailable() const;
	
  protected:
	struct Obj {
		Obj();
		Obj( const Serial::Device &device, int baudRate );
		~Obj();
	
		Device			mDevice;
		
#ifdef CINDER_MSW
		::HANDLE		mDeviceHandle;
		::COMMTIMEOUTS 	mSavedTimeouts;
#else
		int				mFd;
		::termios		mSavedOptions;
#endif	
	};
	
	shared_ptr<Obj>		mObj;
	
  private:

	static bool							sDevicesInited;
	static std::vector<Serial::Device>	sDevices;	
};
	
class SerialExc : public Exception {
};

class SerialExcOpenFailed : public SerialExc {
};

class SerialExcDeviceEnumerationFailed : public SerialExc {
};

class SerialExcReadFailure : public SerialExc {
};

class SerialExcWriteFailure : public SerialExc {
};

class SerialTimeoutExc : public SerialExc {	
};
		
} // namespace cinder