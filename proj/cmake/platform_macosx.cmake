cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

set( CINDER_PLATFORM "GLFW" )
message( STATUS "Using GLFW backend for macOS" )

# Build universal binaries for macOS (arm64 + x86_64) to match Xcode default behavior
if( NOT CMAKE_OSX_ARCHITECTURES )
	set( CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures" FORCE )
endif()

# Set minimum deployment target to macOS 10.13
if( NOT CMAKE_OSX_DEPLOYMENT_TARGET )
	set( CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "macOS minimum deployment target" FORCE )
endif()

# append mac specific source files
list( APPEND SRC_SET_COCOA
	${CINDER_SRC_DIR}/cinder/CaptureImplAvFoundation.mm
	${CINDER_SRC_DIR}/cinder/Filesystem.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplCocoa.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/PlatformCocoa.cpp
	${CINDER_SRC_DIR}/cinder/cocoa/CinderCocoa.mm
)

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_AUDIO_COCOA
		${CINDER_SRC_DIR}/cinder/audio/cocoa/CinderCoreAudio.cpp
		${CINDER_SRC_DIR}/cinder/audio/cocoa/ContextAudioUnit.cpp
		${CINDER_SRC_DIR}/cinder/audio/cocoa/DeviceManagerCoreAudio.cpp
		${CINDER_SRC_DIR}/cinder/audio/cocoa/FileCoreAudio.cpp
	)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# GLFW Backend (macOS)
# ----------------------------------------------------------------------------------------------------------------------

# Define CINDER_GLFW preprocessor macro
list( APPEND CINDER_DEFINES CINDER_GLFW )

# Add GLFW app implementation sources
list( APPEND SRC_SET_APP_GLFW
	${CINDER_SRC_DIR}/cinder/app/glfw/AppGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/glfw/AppImplGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/glfw/WindowImplGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/glfw/RendererGlGlfw.cpp
)

# Add GLFW library sources (core + Cocoa backend)
list( APPEND CINDER_SRC_FILES
	${SRC_SET_APP_GLFW}
	${SRC_SET_GLFW}
	${SRC_SET_GLFW_COCOA}
)

# Add GLFW-specific compile definitions
list( APPEND CINDER_DEFINES _GLFW_COCOA )

source_group( "cinder\\app\\glfw" FILES ${SRC_SET_APP_GLFW} )
source_group( "thirdparty\\glfw" FILES ${SRC_SET_GLFW} ${SRC_SET_GLFW_COCOA} )

# specify what files need to be compiled as Objective-C++
list( APPEND CINDER_SOURCES_OBJCPP
	${CINDER_SRC_DIR}/cinder/Capture.cpp
	${CINDER_SRC_DIR}/cinder/Font.cpp
	${CINDER_SRC_DIR}/cinder/Log.cpp
	${CINDER_SRC_DIR}/cinder/System.cpp
	${CINDER_SRC_DIR}/cinder/Utilities.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/PlatformCocoa.cpp
	${CINDER_SRC_DIR}/cinder/gl/Environment.cpp
	${CINDER_SRC_DIR}/cinder/app/glfw/AppImplGlfwMac.mm
)

# GLFW backend needs these as Obj-C++ for macOS system integration
list( APPEND CINDER_SOURCES_OBJCPP
	${CINDER_SRC_DIR}/cinder/Clipboard.cpp
	${CINDER_SRC_DIR}/cinder/Display.cpp
	${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
	${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
	${CINDER_SRC_DIR}/cinder/app/Window.cpp
	${CINDER_SRC_DIR}/cinder/app/glfw/RendererImplGlfwGl.cpp
)

if( NOT CINDER_DISABLE_VIDEO )
	list( APPEND SRC_SET_QTIME
		${CINDER_SRC_DIR}/cinder/qtime/AvfUtils.mm
		${CINDER_SRC_DIR}/cinder/qtime/AvfWriter.mm
		${CINDER_SRC_DIR}/cinder/qtime/MovieWriter.cpp
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplAvf.cpp
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeImplAvf.mm
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeUtils.cpp
	)

	list( APPEND CINDER_SOURCES_OBJCPP ${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplAvf.cpp )
endif()

set_source_files_properties( ${CINDER_SOURCES_OBJCPP}
	PROPERTIES COMPILE_FLAGS "-x objective-c++"
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_COCOA}
	${SRC_SET_AUDIO_COCOA}
)

# link in system frameworks
find_library( COCOA_FRAMEWORK Cocoa REQUIRED )
find_library( OPENGL_FRAMEWORK OpenGL REQUIRED )
find_library( AVFOUNDATION_FRAMEWORK AVFoundation REQUIRED )
if( NOT CINDER_DISABLE_AUDIO )
	find_library( AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED )
	find_library( AUDIOUNIT_FRAMEWORK AudioUnit REQUIRED )
	find_library( COREAUDIO_FRAMEWORK CoreAudio REQUIRED )
endif()
if( NOT CINDER_DISABLE_VIDEO )
	find_library( COREMEDIA_FRAMEWORK CoreMedia REQUIRED )
endif()
# some Core Video functions are used inside CinderCocoa.mm so we have to link for
# now even if CINDER_DISABLE_AUDIO is defined.
find_library( COREVIDEO_FRAMEWORK CoreVideo REQUIRED )  
find_library( ACCELERATE_FRAMEWORK Accelerate REQUIRED )
find_library( IOSURFACE_FRAMEWORK IOSurface REQUIRED )
find_library( IOKIT_FRAMEWORK IOKit REQUIRED )

# Option for using GStreamer under OS X.
if( CINDER_MAC AND NOT CINDER_DISABLE_VIDEO )
	option( CINDER_MAC_USE_GSTREAMER "Use GStreamer for video playback." OFF )
endif()

if( NOT CINDER_DISABLE_VIDEO )
	if( CINDER_MAC_USE_GSTREAMER )
		find_library( GSTREAMER_FRAMEWORK GStreamer REQUIRED )
		list( APPEND CINDER_LIBS_DEPENDS ${GSTREAMER_FRAMEWORK} ${GSTREAMER_FRAMEWORK}/Versions/Current/lib/libgstgl-1.0.dylib )
		list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${GSTREAMER_FRAMEWORK}/Headers ${CINDER_INC_DIR}/cinder/linux )
		list( APPEND CINDER_SRC_FILES ${CINDER_SRC_DIR}/cinder/linux/GstPlayer.cpp ${CINDER_SRC_DIR}/cinder/linux/Movie.cpp )
		list( APPEND CINDER_DEFINES CINDER_MAC_USE_GSTREAMER )
	else()
		list( APPEND CINDER_SRC_FILES ${SRC_SET_QTIME} )
	endif()
endif()

list( APPEND CINDER_LIBS_DEPENDS
    ${COCOA_FRAMEWORK}
    ${OPENGL_FRAMEWORK}
    ${AUDIOTOOLBOX_FRAMEWORK}
    ${AVFOUNDATION_FRAMEWORK}
    ${AUDIOUNIT_FRAMEWORK}
    ${COREAUDIO_FRAMEWORK}
    ${COREMEDIA_FRAMEWORK}
    ${COREVIDEO_FRAMEWORK}
    ${ACCELERATE_FRAMEWORK}
    ${IOSURFACE_FRAMEWORK}
    ${IOKIT_FRAMEWORK}
)

source_group( "cinder\\cocoa"           FILES ${SRC_SET_COCOA} )
source_group( "cinder\\audio\\cocoa"    FILES ${SRC_SET_AUDIO_COCOA} )

set( MACOS_SUBFOLDER            "${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}" )

if( NOT ( "Xcode" STREQUAL "${CMAKE_GENERATOR}" ) )
	if(NOT CMAKE_LIBTOOL)
		find_program(CMAKE_LIBTOOL NAMES libtool)
	endif()

	if(CMAKE_LIBTOOL)
		set(CMAKE_LIBTOOL ${CMAKE_LIBTOOL} CACHE PATH "libtool executable")
		message(STATUS "Found libtool - ${CMAKE_LIBTOOL}")
		get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
		foreach(lang ${languages})
			set(CMAKE_${lang}_CREATE_STATIC_LIBRARY "${CMAKE_LIBTOOL} -static -o <TARGET> <LINK_FLAGS> <OBJECTS> ")
		endforeach()
	endif()
endif()

# These are samples that cannot be built on Mac OS X, indicating they should be skipped with CINDER_BUILD_ALL_SAMPLES is on.
list( APPEND CINDER_SKIP_SAMPLES
	_opengl/ParticleSphereCS
	_opengl/NVidiaComputeParticles
)
