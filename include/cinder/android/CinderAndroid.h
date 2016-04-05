/*
 Copyright (c) 2015, The Cinder Project

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

#include "cinder/Filesystem.h"
#include "cinder/Surface.h"

int stoi( const std::string& str );

namespace std {

std::string to_string( int value );
std::string to_string( long value );
std::string to_string( long long value );
std::string to_string( unsigned value );
std::string to_string( unsigned long value );
std::string to_string( unsigned long long value );
std::string to_string( float value );
std::string to_string( double value );
std::string to_string( long double value );

} // namespace std

namespace cinder { namespace android {

enum DisplayRotation {
	 ROTATION_0 	= 0x00000000,
	 ROTATION_90 	= 0x00000001,
	 ROTATION_180 	= 0x00000002,
	 ROTATION_270 	= 0x00000003,
};

namespace fs {

//! Returns true if path exists on asset fs or regular fs
bool exists( const ci::fs::path& path );
//! Returns the normalized path
cinder::fs::path normalizePath( const cinder::fs::path &path );

} // namespace fs

cinder::fs::path getCacheDirectory();
cinder::fs::path getPicturesDirectory();
void setWallpaper( const ci::Surface8u& surf );
void launchTwitter( const std::string& text = "", const ci::Surface8u* surf = nullptr );

void setActivityGainedFocusCallback( std::function<void()> fn );
void clearActivityGainedFocusCallback();
void setActivityLostFocusCallback( std::function<void()> fn );
void clearActivityLostFocusCallback();

}} // namespace cinder::android
