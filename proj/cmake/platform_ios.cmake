cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

set( CINDER_PLATFORM "CocoaTouch" )

# Force OpenGL ES
set( CINDER_GL_ES TRUE )
set( CINDER_GL_ES_3 TRUE )

# iOS deployment target
if( NOT CMAKE_OSX_DEPLOYMENT_TARGET )
	set( CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "iOS minimum deployment target" FORCE )
endif()

# iOS source files
list( APPEND SRC_SET_COCOA_TOUCH
	${CINDER_SRC_DIR}/cinder/CaptureImplAvFoundation.mm
	${CINDER_SRC_DIR}/cinder/Filesystem.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplCocoa.mm
	${CINDER_SRC_DIR}/cinder/cocoa/CinderCocoa.mm
	${CINDER_SRC_DIR}/cinder/cocoa/CinderCocoaTouch.mm
)

list( APPEND SRC_SET_APP_COCOA_TOUCH
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppCocoaTouch.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppImplCocoaTouch.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/CinderViewCocoaTouch.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/PlatformCocoa.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/RendererImplGlCocoaTouch.mm
)

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_AUDIO_COCOA_TOUCH
		${CINDER_SRC_DIR}/cinder/audio/cocoa/CinderCoreAudio.cpp
		${CINDER_SRC_DIR}/cinder/audio/cocoa/ContextAudioUnit.cpp
		${CINDER_SRC_DIR}/cinder/audio/cocoa/DeviceManagerAudioSession.mm
		${CINDER_SRC_DIR}/cinder/audio/cocoa/FileCoreAudio.cpp
	)
endif()

if( NOT CINDER_DISABLE_VIDEO )
	list( APPEND SRC_SET_VIDEO_COCOA_TOUCH
		${CINDER_SRC_DIR}/cinder/qtime/AvfUtils.mm
		${CINDER_SRC_DIR}/cinder/qtime/AvfWriter.mm
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplAvf.cpp
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeImplAvf.mm
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeUtils.cpp
	)
endif()

# Files that need Objective-C++ compilation
list( APPEND CINDER_SOURCES_OBJCPP
	${CINDER_SRC_DIR}/cinder/Capture.cpp
	${CINDER_SRC_DIR}/cinder/Clipboard.cpp
	${CINDER_SRC_DIR}/cinder/Display.cpp
	${CINDER_SRC_DIR}/cinder/Font.cpp
	${CINDER_SRC_DIR}/cinder/Log.cpp
	${CINDER_SRC_DIR}/cinder/System.cpp
	${CINDER_SRC_DIR}/cinder/Utilities.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/PlatformCocoa.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppCocoaTouch.cpp
	${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
	${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
	${CINDER_SRC_DIR}/cinder/app/Window.cpp
	${CINDER_SRC_DIR}/cinder/gl/Environment.cpp
)

if( NOT CINDER_DISABLE_VIDEO )
	list( APPEND CINDER_SOURCES_OBJCPP
		${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplAvf.cpp
	)
endif()

set_source_files_properties( ${CINDER_SOURCES_OBJCPP}
	PROPERTIES COMPILE_FLAGS "-x objective-c++"
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_COCOA_TOUCH}
	${SRC_SET_APP_COCOA_TOUCH}
	${SRC_SET_AUDIO_COCOA_TOUCH}
	${SRC_SET_VIDEO_COCOA_TOUCH}
)

source_group( "cinder\\cocoa"           FILES ${SRC_SET_COCOA_TOUCH} )
source_group( "cinder\\app\\cocoa"      FILES ${SRC_SET_APP_COCOA_TOUCH} )
source_group( "cinder\\audio\\cocoa"    FILES ${SRC_SET_AUDIO_COCOA_TOUCH} )
source_group( "cinder\\video\\cocoa"    FILES ${SRC_SET_VIDEO_COCOA_TOUCH} )

# iOS frameworks
find_library( UIKIT_FRAMEWORK UIKit REQUIRED )
find_library( OPENGLES_FRAMEWORK OpenGLES REQUIRED )
find_library( AVFOUNDATION_FRAMEWORK AVFoundation REQUIRED )
find_library( COREVIDEO_FRAMEWORK CoreVideo REQUIRED )
find_library( ACCELERATE_FRAMEWORK Accelerate REQUIRED )
find_library( COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED )
find_library( QUARTZCORE_FRAMEWORK QuartzCore REQUIRED )
find_library( FOUNDATION_FRAMEWORK Foundation REQUIRED )
find_library( IMAGEIO_FRAMEWORK ImageIO REQUIRED )
find_library( MOBILECORESERVICES_FRAMEWORK MobileCoreServices REQUIRED )

if( NOT CINDER_DISABLE_AUDIO )
	find_library( AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED )
	find_library( COREAUDIO_FRAMEWORK CoreAudio REQUIRED )
	find_library( AVFAUDIO_FRAMEWORK AVFAudio REQUIRED )
endif()

if( NOT CINDER_DISABLE_VIDEO )
	find_library( COREMEDIA_FRAMEWORK CoreMedia REQUIRED )
endif()

list( APPEND CINDER_LIBS_DEPENDS
	${UIKIT_FRAMEWORK}
	${OPENGLES_FRAMEWORK}
	${AVFOUNDATION_FRAMEWORK}
	${COREVIDEO_FRAMEWORK}
	${ACCELERATE_FRAMEWORK}
	${COREGRAPHICS_FRAMEWORK}
	${QUARTZCORE_FRAMEWORK}
	${FOUNDATION_FRAMEWORK}
	${IMAGEIO_FRAMEWORK}
	${MOBILECORESERVICES_FRAMEWORK}
	${AUDIOTOOLBOX_FRAMEWORK}
	${COREAUDIO_FRAMEWORK}
	${AVFAUDIO_FRAMEWORK}
	${COREMEDIA_FRAMEWORK}
)

# Include paths for CocoaTouch headers
list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
	${CINDER_INC_DIR}/cinder/app/cocoa
)

# GL ES defines
list( APPEND CINDER_DEFINES "-DCINDER_GL_ES" "-DCINDER_GL_ES_3" )
