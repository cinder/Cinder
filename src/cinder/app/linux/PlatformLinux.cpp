/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#include "cinder/app/linux/PlatformLinux.h"
#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/ImageSourceFileStbImage.h"
#include "cinder/ImageTargetFileStbImage.h"

namespace cinder { namespace app {

PlatformLinux::PlatformLinux()
{
	ImageSourceFileRadiance::registerSelf();
	ImageSourceFileStbImage::registerSelf();
	ImageTargetFileStbImage::registerSelf();
}

PlatformLinux::~PlatformLinux()
{
}

PlatformLinux* PlatformLinux::get() 
{ 
	return reinterpret_cast<PlatformLinux*>( Platform::get() ); 
}

DataSourceRef PlatformLinux::loadResource( const fs::path &resourcePath ) 
{
	fs::path fullPath = getResourcePath( resourcePath );
	if( fullPath.empty() )
		throw ResourceLoadExc( resourcePath );
	else
		return DataSourcePath::create( fullPath );	
}

fs::path PlatformLinux::getResourceDirectory() const 
{
	
}

fs::path PlatformLinux::getResourcePath( const fs::path &rsrcRelativePath ) const 
{
	
}


fs::path PlatformLinux::getOpenFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) 
{
	
}

fs::path PlatformLinux::getFolderPath( const fs::path &initialPath ) 
{
	
}

fs::path PlatformLinux::getSaveFilePath( const fs::path &initialPath, const std::vector<std::string> &extensions ) 
{
	
}

std::map<std::string,std::string> PlatformLinux::getEnvironmentVariables() 
{
	
}

fs::path PlatformLinux::expandPath( const fs::path &path ) 
{
	
}

fs::path PlatformLinux::getHomeDirectory() const 
{
	
}

fs::path PlatformLinux::getDocumentsDirectory() const 
{
	
}

fs::path PlatformLinux::getDefaultExecutablePath() const 
{
	
}

void PlatformLinux::sleep( float milliseconds ) 
{
	
}

void PlatformLinux::launchWebBrowser( const Url &url ) 
{
	
}

std::vector<std::string> PlatformLinux::stackTrace() 
{
	
}

}} // namespace cinder::app
