cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

set( CINDER_PLATFORM "Posix" )
set( CINDER_TARGET_SUBFOLDER "linux" )

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

#list( APPEND SRC_SET_CINDER_LINUX
#)

list( APPEND SRC_SET_CINDER_APP_LINUX
	${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxGlfw.cpp
)

list( APPEND CINDER_SRC_FILES
#	${SRC_SET_CINDER_LINUX}
	${SRC_SET_CINDER_APP_LINUX}
)

#source_group( "cinder\\linux"           FILES ${SRC_SET_CINDER_LINUX} )
source_group( "cinder\\app\\linux"      FILES ${SRC_SET_CINDER_APP_LINUX} )

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )
