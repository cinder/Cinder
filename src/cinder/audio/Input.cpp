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

#include "cinder/audio/Input.h"
#if defined( CINDER_COCOA )
	#include "cinder/audio/InputImplAudioUnit.h"
	typedef cinder::audio::InputImplAudioUnit	InputPlatformImpl;
#endif

namespace cinder { namespace audio {

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Input

Input::Input()
{
	mImpl = shared_ptr<InputImpl>( new InputPlatformImpl( InputDeviceRef() ) );
}

Input::Input( InputDeviceRef aDevice )
{
	mImpl = shared_ptr<InputImpl>( new InputPlatformImpl( aDevice ) );
}

const std::vector<InputDeviceRef>&	Input::getDevices( bool forceRefresh )
{
	return InputPlatformImpl::getDevices( forceRefresh );
}

InputDeviceRef Input::getDefaultDevice()
{
	return InputPlatformImpl::getDefaultDevice();
}

InputDeviceRef Input::findDeviceByName( const std::string &name )
{
	return InputDeviceRef();
}

InputDeviceRef Input::findDeviceByNameContains( const std::string &nameFragment )
{
	return InputDeviceRef();
}


}} //namespace