# This file is consumed by the main libcinder cmake configuration and any other targets within the library that depend on it (such as samples and tests).
# Other applications can include it as well to set the same build variables that libcinder uses

option( CINDER_VERBOSE "Print verbose build configuration messages. " OFF )
option( BUILD_SHARED_LIBS "Build Cinder as a shared library. " OFF )
option( CINDER_DISABLE_AUDIO "Build Cinder without audio support. " OFF )
option( CINDER_DISABLE_VIDEO "Build Cinder without video support. " OFF )
option( CINDER_DISABLE_ANTTWEAKBAR "Build Cinder without AntTweakBar support. " OFF )
option( CINDER_DISABLE_IMGUI "Build Cinder without imgui support. " OFF )

include( ${CMAKE_CURRENT_LIST_DIR}/utilities.cmake )

# Set default build type to Debug
if( NOT CMAKE_BUILD_TYPE )
	ci_log_v( "CMAKE_BUILD_TYPE not specified, defaulting to Debug" )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		FORCE
	)
endif()

ci_log_v( "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )

# Set default target to build for
if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
	set( CINDER_TARGET_DEFAULT "macosx" )
elseif( CMAKE_SYSTEM_NAME MATCHES "Linux" )
	set( CINDER_TARGET_DEFAULT "linux" )
elseif( CMAKE_SYSTEM_NAME MATCHES "Windows" )
	set( CINDER_TARGET_DEFAULT "msw" )
else()
	# no default target, user must specify
	set( CINDER_TARGET_DEFAULT "" )
endif()

set( CINDER_TARGET ${CINDER_TARGET_DEFAULT} CACHE STRING "Target platform to build for. Default will match the current OS. Vaid options: linux, macosx, msw, ios, android" )
if( CINDER_TARGET )
	string( TOLOWER "${CINDER_TARGET}" CINDER_TARGET_LOWER )
	if( "android" STREQUAL "${CINDER_TARGET_LOWER}" )
		set( CINDER_ANDROID TRUE )
	elseif( "ios" STREQUAL "${CINDER_TARGET_LOWER}" )
		set( CINDER_COCOA_TOUCH TRUE )
	elseif( "linux" STREQUAL "${CINDER_TARGET_LOWER}" )
		set( CINDER_LINUX TRUE )
	elseif( "macosx" STREQUAL "${CINDER_TARGET_LOWER}" )
		set( CINDER_MAC TRUE )
	elseif( "msw" STREQUAL "${CINDER_TARGET_LOWER}" )
		set( CINDER_MSW TRUE )
	else()
		message( FATAL_ERROR "unexpected CINDER_TARGET '${CINDER_TARGET}'." )
	endif()
else()
	message( FATAL_ERROR "CINDER_TARGET not defined, and no default for platform '${CMAKE_SYSTEM_NAME}.'" )
endif()

# Configure which gl target to build for, currently only used on linux.
if( CINDER_LINUX )
	set( CINDER_TARGET_GL_DEFAULT "ogl" )
else()
	set( CINDER_TARGET_GL_DEFAULT "" )
endif()

set( CINDER_TARGET_GL ${CINDER_TARGET_GL_DEFAULT} CACHE STRING "Target GL for the system. Valid options: ogl, es2, es3, es31, es32, es2-rpi" )

if( CINDER_TARGET_GL )
	ci_log_v( "CINDER_TARGET_GL: ${CINDER_TARGET_GL}" )

	string( TOLOWER "${CINDER_TARGET_GL}" CINDER_TARGET_GL_LOWER )
	if( "ogl" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		set( CINDER_GL_CORE TRUE )
	elseif( "es2" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_2 TRUE )
	elseif( "es3" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3 TRUE )
	elseif( "es31" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_1 TRUE )
	elseif( "es32" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_2 TRUE )
	elseif( "es3-rpi" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_RPI TRUE )
	else()
		message( WARNING "Unexpected value for CINDER_TARGET_GL: " ${CINDER_TARGET_GL_LOWER} ". Default to Core Profile." )
		set( CINDER_GL_CORE TRUE )
	endif()
else()
	ci_log_v( "No target GL has been set. Defaulting to Core Profile.")
	set( CINDER_GL_CORE TRUE )
endif()


# Enable headless GL rendering support.
set( CINDER_HEADLESS_GL "" CACHE STRING "Enable headless rendering. Valid options: egl" )
if( CINDER_HEADLESS_GL )
	if( CINDER_LINUX )
		string( TOLOWER "${CINDER_HEADLESS_GL}" CINDER_HEADLESS_GL_LOWER )
		if( "egl" STREQUAL "${CINDER_HEADLESS_GL_LOWER}" )
			set( CINDER_HEADLESS True )
			set( CINDER_HEADLESS_GL_EGL True )
		elseif( "osmesa" STREQUAL "${CINDER_HEADLESS_GL_LOWER}" )
			set( CINDER_HEADLESS True )
			set( CINDER_HEADLESS_GL_OSMESA True )
		else()
			message( FATAL_ERROR "Unsupported headless GL rendering option: " ${CINDER_HEADLESS_GL_LOWER} " Available options include: egl, ..." )
		endif()
	else()
		message( FATAL_ERROR "Cinder headless GL rendering support is only available on Linux." )
	endif()
endif()

# Configure platform variables needed by both libcinder and user projects.
if( CINDER_LINUX )
	# Find architecture name.
	execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )
	# CINDER_TARGET_GL is added to CINDER_TARGET_SUBFOLDER on Linux since we can have various builds depending on the target GL.
	# e.g on the TK1 we can build both core profile and es2 so this takes care of putting everything on the right place.
	set( CINDER_TARGET_SUBFOLDER "linux/${CINDER_ARCH}/${CINDER_TARGET_GL}" )
elseif( CINDER_MAC )
	set( CINDER_TARGET_SUBFOLDER "macosx" )
elseif( CINDER_ANDROID )
	#set( CINDER_ANDROID_NDK_PLATFORM 21 CACHE STRING "Android NDK Platform version number." )
	#set( CINDER_ANDROID_NDK_ARCH "armeabi-v7a" CACHE STRING "Android NDK target architecture." )
	set( CINDER_TARGET_SUBFOLDER "android/android-${CINDER_ANDROID_NDK_PLATFORM}/${CINDER_ANDROID_NDK_ARCH}" )
elseif( CINDER_MSW )
	set( CINDER_ARCH "x86" )
	if( CMAKE_CL_64 )
		set( CINDER_ARCH "x64" )
	endif()
	set( CINDER_TARGET_SUBFOLDER "msw/${CINDER_ARCH}" ) # TODO: place in msw/arch folder (x64 or x86)
endif()

# CINDER_LIB_DIRECTORY is the platform-specific, relative path that will be used to define
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY for libcinder and also specifies where user apps will locate the cinder package
set( CINDER_LIB_DIRECTORY "lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}/" )
