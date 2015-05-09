/*
 Copyright (c) 2014, The Cinder Project

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

// note: this file will be removed in future versions

#pragma once

#include "cinder/Cinder.h"

// these are placeholder log functions, replaced in 0.9.0 with those from cinder/Log.h
#if( CINDER_VERSION < 807 )

#include "cinder/CurrentFunction.h"
#include "cinder/app/App.h"

#if ! defined( NDEBUG )
#define CI_LOG_V( stream )		do{ ci::app::console() << " |verbose| " << CINDER_CURRENT_FUNCTION << " | " << stream << std::endl; } while( 0 )
#else
#define CI_LOG_V( stream )		do{} while( 0 )
#endif // ! defined( NDEBUG )

#define CI_LOG_I( stream )		do{ ci::app::console() << " |info   | " << CINDER_CURRENT_FUNCTION << " | " << stream << std::endl; } while( 0 )
#define CI_LOG_W( stream )		do{ ci::app::console() << " |warning| " << CINDER_CURRENT_FUNCTION << " | " << stream << std::endl; } while( 0 )
#define CI_LOG_E( stream )		do{ ci::app::console() << " |error  | " << CINDER_CURRENT_FUNCTION << " | " << stream << std::endl; } while( 0 )

#endif // CINDER_VERSION < 807
