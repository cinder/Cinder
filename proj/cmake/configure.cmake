# This file is consumed by the main libcinder cmake configuration and any other targets within the library that depend on it (such as samples and tests).
# Other applications can include it as well to set the same build variables that libcinder uses

set( CINDER_TARGET "" CACHE STRING "Target platform to build for." )
option( CINDER_VERBOSE "Print verbose build configuration messages. " OFF )

# Set default build type to Debug
if( NOT CMAKE_BUILD_TYPE )
	message( "CMAKE_BUILD_TYPE not specified, defaulting to Debug" )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		FORCE
	)
endif()

# If there's a target specified, try to build for that. Otherwise, build based on the current host OS.
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
	if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
		set( CINDER_TARGET "macosx" )
		set( CINDER_MAC TRUE )
	elseif( CMAKE_SYSTEM_NAME MATCHES "Linux" )
		set( CINDER_TARGET "linux" )
		set( CINDER_LINUX TRUE )
	elseif( CMAKE_SYSTEM_NAME MATCHES "Windows" )
		message( WARNING "CMake support on Windows is experimental and may be incomplete." )
		set( CINDER_TARGET "msw" )
		set( CINDER_MSW TRUE )
	else()
		message( FATAL_ERROR "CINDER_TARGET not defined, and no default for platform '${CMAKE_SYSTEM_NAME}.'" )
	endif()
endif()

# Configure which gl target to build for, currently only used on linux.
if( CINDER_LINUX )
	set( CINDER_TARGET_GL_DEFAULT "ogl" )
else()
	set( CINDER_TARGET_GL_DEFAULT "" )
endif()

set( CINDER_TARGET_GL ${CINDER_TARGET_GL_DEFAULT} CACHE STRING "Target GL for the system. Valid options : ogl, es2, es3, es31, es32, es2-rpi" )

if( CINDER_TARGET_GL )
	string( TOLOWER "${CINDER_TARGET_GL}" CINDER_TARGET_GL_LOWER )
	if( "ogl" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with Core Profile support." )
		set( CINDER_GL_CORE TRUE )
	elseif( "es2" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es2 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_2 TRUE )
	elseif( "es3" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es3 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3 TRUE )
	elseif( "es31" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es3 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_1 TRUE )
	elseif( "es32" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es3 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_2 TRUE )
	elseif( "es2-rpi" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es2-rpi support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_2_RPI TRUE )
	else()
		message( STATUS "Unknown target GL : " ${CINDER_TARGET_GL_LOWER} " . Default to Core Profile.")
		set( CINDER_GL_CORE TRUE )
	endif()
else()
	message( STATUS "No target GL has been set. Defaulting to Core Profile.")
	set( CINDER_GL_CORE TRUE )
endif()

# Configure platform variables needed by both libcinder and user projects.
if( CINDER_LINUX )
	# Find architecture name.
	execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )
	set( CINDER_TARGET_SUBFOLDER "linux/${CINDER_ARCH}" )
elseif( CINDER_MAC )
	set( CINDER_TARGET_SUBFOLDER "macosx" )
elseif( CINDER_ANDROID )
	set( CINDER_ANDROID_NDK_PLATFORM 21 CACHE STRING "Android NDK Platform version number." )
	set( CINDER_ANDROID_NDK_ARCH "armeabi-v7a" CACHE STRING "Android NDK target architecture." )

	set( CINDER_TARGET_SUBFOLDER "android-${CINDER_ANDROID_NDK_PLATFORM/CINDER_ANDROID_NDK_ARCH" )
elseif( CINDER_MSW )
	set( CINDER_TARGET_SUBFOLDER "msw" ) # TODO: place in msw/arch folder (x64 or x86)
endif()

# note: CINDER_TARGET_SUBFOLDER is defined by each platform config, to be a folder that lives in cinder/lib/*
# note: CINDER_TARGET_GL_SUBFOLDER is necessary for Linux since we can have various builds depending on the target GL.
# e.g on the TK1 we can build both core profile and es2 so this takes care of putting everything on the right place.
# For other platforms than Linux if this var is not needed will just stay empty.
set( CINDER_ARCHIVE_OUTPUT_DIRECTORY lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}/${CINDER_TARGET_GL} )
