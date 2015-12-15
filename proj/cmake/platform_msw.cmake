cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CINDER_PLATFORM "Windows" )

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
	${CINDER_SRC_DIR}/cinder/app/AppScreenSaver.cpp
	${CINDER_SRC_DIR}/cinder/app/AppImplMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/AppImplMswBasic.cpp
	${CINDER_SRC_DIR}/cinder/app/AppImplMswRendererGl.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererDx.cpp
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_MSW}
	${SRC_SET_APP_MSW}
)

source_group( "cinder\\msw"       FILES ${SRC_SET_MSW} )
source_group( "cinder\\app\\msw"  FILES ${SRC_SET_APP_MSW} )

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )