cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CINDER_PLATFORM "Cocoa" )

# append mac specific source files
list( APPEND SRC_SET_COCOA
	${CINDER_SRC_DIR}/cinder/CaptureImplAvFoundation.mm
	${CINDER_SRC_DIR}/cinder/ImageSourceFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplCocoa.mm
	${CINDER_SRC_DIR}/cinder/cocoa/CinderCocoa.mm
)

list( APPEND SRC_SET_APP_COCOA
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppCocoaView.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppImplMac.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppMac.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/CinderViewMac.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/PlatformCocoa.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/RendererImpl2dMacQuartz.mm
	${CINDER_SRC_DIR}/cinder/app/cocoa/RendererImplGlMac.mm
)

list( APPEND SRC_SET_AUDIO_COCOA
	${CINDER_SRC_DIR}/cinder/audio/cocoa/CinderCoreAudio.cpp
	${CINDER_SRC_DIR}/cinder/audio/cocoa/ContextAudioUnit.cpp
	${CINDER_SRC_DIR}/cinder/audio/cocoa/DeviceManagerCoreAudio.cpp
	${CINDER_SRC_DIR}/cinder/audio/cocoa/FileCoreAudio.cpp
)

list( APPEND SRC_SET_QTIME
	${CINDER_SRC_DIR}/cinder/qtime/AvfUtils.mm
	${CINDER_SRC_DIR}/cinder/qtime/AvfWriter.mm
	${CINDER_SRC_DIR}/cinder/qtime/MovieWriter.cpp
	${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplAvf.cpp
	${CINDER_SRC_DIR}/cinder/qtime/QuickTimeImplAvf.mm
	${CINDER_SRC_DIR}/cinder/qtime/QuickTimeUtils.cpp
)

# specify what files need to be compiled as Objective-C++
list( APPEND CINDER_SOURCES_OBJCPP
	${CINDER_SRC_DIR}/cinder/Capture.cpp
	${CINDER_SRC_DIR}/cinder/Clipboard.cpp
	${CINDER_SRC_DIR}/cinder/Display.cpp
	${CINDER_SRC_DIR}/cinder/Font.cpp
	${CINDER_SRC_DIR}/cinder/Log.cpp
	${CINDER_SRC_DIR}/cinder/System.cpp
	${CINDER_SRC_DIR}/cinder/Utilities.cpp
	${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
	${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
	${CINDER_SRC_DIR}/cinder/app/Window.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/AppMac.cpp
	${CINDER_SRC_DIR}/cinder/app/cocoa/PlatformCocoa.cpp
	${CINDER_SRC_DIR}/cinder/gl/Environment.cpp
	${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplAvf.cpp

	${CINDER_SRC_DIR}/AntTweakBar/TwColors.cpp
	${CINDER_SRC_DIR}/AntTweakBar/TwFonts.cpp
	${CINDER_SRC_DIR}/AntTweakBar/LoadOGL.cpp
	${CINDER_SRC_DIR}/AntTweakBar/LoadOGLCore.cpp
	${CINDER_SRC_DIR}/AntTweakBar/TwBar.cpp
	${CINDER_SRC_DIR}/AntTweakBar/TwMgr.cpp
	${CINDER_SRC_DIR}/AntTweakBar/TwOpenGl.cpp
	${CINDER_SRC_DIR}/AntTweakBar/TwOpenGLCore.cpp
	${CINDER_SRC_DIR}/AntTweakBar/TwPrecomp.cpp
)

set_source_files_properties( ${CINDER_SOURCES_OBJCPP}
	PROPERTIES COMPILE_FLAGS "-x objective-c++"
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_COCOA}
	${SRC_SET_APP_COCOA}
	${SRC_SET_AUDIO_COCOA}
	${SRC_SET_QTIME}
)

list( APPEND CINDER_LIBS_DEPENDS
    ${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}/libboost_system.a
    ${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}/libboost_filesystem.a
    ${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}/libz.a
)

# link in system frameworks
find_library( COCOA_FRAMEWORK Cocoa REQUIRED )
find_library( OPENGL_FRAMEWORK OpenGL REQUIRED )
find_library( AVFOUNDATION_FRAMEWORK AVFoundation REQUIRED )
find_library( AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED )
find_library( AUDIOUNIT_FRAMEWORK AudioUnit REQUIRED )
find_library( COREAUDIO_FRAMEWORK CoreAudio REQUIRED )
find_library( COREMEDIA_FRAMEWORK CoreMedia REQUIRED )
find_library( COREVIDEO_FRAMEWORK CoreVideo REQUIRED )
find_library( ACCELERATE_FRAMEWORK Accelerate REQUIRED )
find_library( IOSURFACE_FRAMEWORK IOSurface REQUIRED )
find_library( IOKIT_FRAMEWORK IOKit REQUIRED )

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
source_group( "cinder\\app\\cocoa"      FILES ${SRC_SET_APP_COCOA} )
source_group( "cinder\\audio\\cocoa"    FILES ${SRC_SET_AUDIO_COCOA} )

# These are samples that cannot be built on Mac OS X, indicating they should be skipped with CINDER_BUILD_SAMPLES is on.
list( APPEND CINDER_SKIP_SAMPLES
	_opengl/ParticleSphereCS
	_opengl/NVidiaComputeParticles
)

