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

#include "cinder/Filesystem.h"
#include <android/asset_manager.h>

namespace cinder { namespace app { namespace android {

typedef AAsset AssetFileSystem_FILE;

bool 					AssetFileSystem_exists( ci::fs::path path );

AssetFileSystem_FILE *	AssetFileSystem_fopen( const char * filename, const char * mode );
int 					AssetFileSystem_fclose( AssetFileSystem_FILE * stream );
int 					AssetFileSystem_fseek( AssetFileSystem_FILE * stream, long int offset, int origin );
long int 				AssetFileSystem_ftell( AssetFileSystem_FILE * stream );
size_t 					AssetFileSystem_fread( void * ptr, size_t size, size_t count, AssetFileSystem_FILE * stream );
int 					AssetFileSystem_feof( AssetFileSystem_FILE * stream );
long int 				AssetFileSystem_flength( AssetFileSystem_FILE * stream );

} } } // namespace cinder::app::android
