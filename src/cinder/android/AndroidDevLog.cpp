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

#include "cinder/android/AndroidDevLog.h"

#include <android/log.h>
#include <sstream>

#define DBG_APP_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "cinder_app", __VA_ARGS__))
#define DBG_APP_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "cinder_app", __VA_ARGS__))
#define DBG_APP_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"cinder_app", __VA_ARGS__))

namespace cinder { namespace android {

void dbg_app_log( const std::string& s )
{
	std::stringstream ss;
	ss << s;
	DBG_APP_LOGI( ss.str().c_str() );
}	

void dbg_app_warn( const std::string& s )
{
	std::stringstream ss;
	ss << s;
	DBG_APP_LOGW( ss.str().c_str() );
}

void dbg_app_error( const std::string& s )
{
	std::stringstream ss;
	ss << s;
	DBG_APP_LOGE( ss.str().c_str() );
}

void dbg_app_fn_enter( const std::string& s )
{
	std::stringstream ss;
	ss << "[FN]: " << s << " entered";
	DBG_APP_LOGI( ss.str().c_str() );
}

void dbg_app_fn_exit( const std::string& s )
{
	std::stringstream ss;
	ss << "[FN]: " << s << " exited";
	DBG_APP_LOGI( ss.str().c_str() );
}

void dbg_obtained_fn( const std::string& s )
{
	std::stringstream ss;
	ss << "[JNI]: " << s << " method obtained";
	DBG_APP_LOGI( ss.str().c_str() );
}

}} // namespace cinder::android