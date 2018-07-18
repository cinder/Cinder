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

// Included macros:
// - CI_ASSERT( expr ): asserts that \a expr evaluates to true. By default it is equivalent to assert( expr )
// - CI_ASSERT_MSG( expr, msg ): same as CI_ASSERT but takes an additional, human readable \a const char* message parameter
// - CI_VERIFY( expr ): same as CI_ASSERT, but still evaluates \a expr in release mode.
// - CI_VERIFY_MSG( expr, msg ): same as CI_ASSERT_MSG, but still evaluates \a expr in release mode.
// - CI_ASSERT_NOT_REACHABLE(): utility to place at an unreachable location in code, which will cause a failed assertion.
//
// There are some user-definable variables that you can use to customize how failed assertions are handled, though
// you must rebuild cinder in order to see the changes.
//
// User-definable parameters:
// - CI_DISABLE_ASSERTS: disables all asserts, they become no-ops (VERIFY variants still evaluate \a expr).
// - CI_ENABLE_ASSERTS: forces asserts enabled, regardless of other build settings.
// - CI_ENABLE_ASSERT_HANDLER: if this is set, users must define assertionFailed() and assertionFailedMessage()
//	 to handle a failed assertion.
// - CI_ASSERT_DEBUG_BREAK: overrides default assertion behavior to break into the debugger instead of
//	 aborting. Cannot be used in conjunction with CI_ENABLE_ASSERT_HANDLER.

#pragma once

#if ( ! defined( NDEBUG ) && ! defined( CI_DISABLE_ASSERTS ) ) || defined( CI_ENABLE_ASSERTS )

	#include "cinder/Export.h"
	#include "cinder/CurrentFunction.h"
	#include <cassert>

	// defined in CinderAssert.cpp
	namespace cinder { namespace detail {
		CI_API void assertionFailedAbort( char const *expr, char const *function, char const *file, long line );
		CI_API void assertionFailedMessageAbort( char const *expr, char const *msg, char const *function, char const *file, long line );
		CI_API void assertionFailedBreak( char const *expr, char const *function, char const *file, long line );
		CI_API void assertionFailedMessageBreak( char const *expr, char const *msg, char const *function, char const *file, long line );
	} } // namespace cinder::detail

	#if defined( CI_ASSERT_DEBUG_BREAK )

		#define CI_ASSERT( expr ) ( (expr) ? ( (void)0) : ::cinder::detail::assertionFailedBreak( #expr, CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) )
		#define CI_ASSERT_MSG( expr, msg ) ( (expr) ? ( (void)0) : ::cinder::detail::assertionFailedMessageBreak( #expr, msg, CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) )

	#elif defined( CI_ENABLE_ASSERT_HANDLER )

		// User opts to define these assertion handlers
		namespace cinder {
			//! Called when CI_ASSERT() fails
			void assertionFailed( char const *expr, char const *function, char const *file, long line );
			//! Called when CI_ASSERT_MSG() fails
			void assertionFailedMessage( char const *expr, char const *msg, char const *function, char const *file, long line );
		}

		#define CI_ASSERT( expr ) ( (expr) ? ( (void)0) : ::cinder::assertionFailed( #expr, CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) )
		#define CI_ASSERT_MSG( expr, msg ) ( (expr) ? ( (void)0) : ::cinder::assertionFailedMessage( #expr, msg, CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) )

	#else // defined( CI_ENABLE_ASSERT_HANDLER )

		#define CI_ASSERT( expr )				( (expr) ? ( (void)0) : ::cinder::detail::assertionFailedAbort( #expr, CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) )
		#define CI_ASSERT_MSG( expr, msg )		( (expr) ? ( (void)0) : ::cinder::detail::assertionFailedMessageAbort( #expr, msg, CINDER_CURRENT_FUNCTION, __FILE__, __LINE__ ) )

	#endif // defined( CI_ASSERT_DEBUG_BREAK )

	#define CI_VERIFY( expr )			CI_ASSERT( expr )
	#define CI_VERIFY_MSG( expr, msg )	CI_ASSERT_MSG( expr, msg )

	#define CI_ASSERT_NOT_REACHABLE()	CI_ASSERT_MSG( 0, "not reachable" )

#else

	#define CI_ASSERT( expr )			( (void)0 )
	#define CI_ASSERT_MSG( expr, msg )	( (void)0 )
	#define CI_VERIFY( expr )			( (void)(expr) )
	#define CI_VERIFY_MSG( expr, msg )	( (void)(expr) )

	#define CI_ASSERT_NOT_REACHABLE()	( (void)0 )

#endif