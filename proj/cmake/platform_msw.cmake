cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CINDER_PLATFORM "MSW" )
set( CINDER_TARGET_SUBFOLDER "msw" ) # TODO: place in msw/arch folder (x64 or x86)

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

list( APPEND SRC_SET_MSW
	${CINDER_SRC_DIR}/cinder/CaptureImplDirectShow.cpp
	${CINDER_SRC_DIR}/cinder/msw/CinderMsw.cpp
	${CINDER_SRC_DIR}/cinder/msw/StackWalker.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileWic.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileWic.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplWinInet.cpp
)

list( APPEND SRC_SET_APP_MSW
	# TODO: should these two files be added to "cinder\\app" group?
	${CINDER_SRC_DIR}/cinder/app/AppScreenSaver.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererDx.cpp

	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMswBasic.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMswScreenSaver.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/PlatformMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImpl2dGdi.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImplDx.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImplGlAngle.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImplGlMsw.cpp
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_MSW}
	${SRC_SET_APP_MSW}
)

source_group( "cinder\\msw"       FILES ${SRC_SET_MSW} )
source_group( "cinder\\app\\msw"  FILES ${SRC_SET_APP_MSW} )

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )