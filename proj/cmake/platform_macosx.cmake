cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CINDER_PLATFORM "Cocoa" )
set( CINDER_TARGET_SUBFOLDER "macosx" )

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

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
	${CINDER_SRC_DIR}/cinder/CaptureImplAvFoundation.mm
	${CINDER_SRC_DIR}/cinder/ImageSourceFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileQuartz.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplCocoa.mm
	${CINDER_SRC_DIR}/cinder/cocoa/CinderCocoa.mm
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
)

source_group( "cinder\\cocoa"       FILES ${SRC_SET_COCOA} )
source_group( "cinder\\app\\cocoa"  FILES ${SRC_SET_APP_COCOA} )

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )
