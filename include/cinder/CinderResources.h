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

// This header defines the macro CINDER_RESOURCE, which does the right thing on all platforms for defining resources.

#pragma once

#ifdef RC_INVOKED
	#define WORKING_DIR "./"
	#define QUOTE_PATH(x) #x
	#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) \
	ID		TYPE 	QUOTE_PATH( ./##LOCALPREFIX##PATH )
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	#include <winapifamily.h>

	#if defined(WINAPI_PARTITION_DESKTOP) // MSW RESOURCE
		#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
			#define QUOTE_PATH(x) #x
			#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) \
			QUOTE_PATH(LOCALPREFIX##PATH),ID,#TYPE
		#else // WINRT RESOURCE
			#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) #PATH
		#endif
	#else // MSW RESOURCE
		#define QUOTE_PATH(x) #x
		#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) \
		QUOTE_PATH(LOCALPREFIX##PATH),ID,#TYPE
	#endif
#elif ! defined(__ANDROID__)
	// MAC or iOS RESOURCE: lives in app bundle. Linux RESOURCE: lives in folder named 'resources` next to executable
	#define CINDER_RESOURCE( LOCALPREFIX, PATH, ID, TYPE ) #PATH
#endif
