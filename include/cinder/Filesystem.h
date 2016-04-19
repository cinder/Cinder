/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
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

#pragma once

#include "cinder/Cinder.h"


#if defined( CINDER_UWP ) || ( defined( _MSC_VER ) && ( _MSC_VER >= 1900 ) )
	#include <filesystem>
#else
	#define BOOST_FILESYSTEM_VERSION 3
	#define BOOST_FILESYSTEM_NO_DEPRECATED
	#include <boost/filesystem.hpp>
#endif

namespace cinder {
#if defined( CINDER_UWP ) || ( defined( _MSC_VER ) && ( _MSC_VER >= 1900 ) )
	namespace fs = std::tr2::sys;
} // namespace cinder
#else
	namespace fs = boost::filesystem;
} // namespace cinder

namespace boost {
	namespace filesystem {
		// C++17 filesystem library defines file_time_type, whereas boost::filesystem uses time_t
		typedef std::time_t			file_time_type;
	}
}
#endif

#if defined( CINDER_UWP ) || ( defined( _MSC_VER ) && ( _MSC_VER >= 1900 ) && ( _MSC_VER < 2000 ) )
//! kludge to work around VC120's lack of fs::canonical
namespace std { namespace tr2 { namespace sys {
template <typename PathT>
PathT canonical( const PathT &p )
{
	return p;
}
} } } // namespace std::tr2::sys
#endif
