cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( cinder )

option( CINDER_BUILD_SAMPLES "Build Samples" OFF )

set( CINDER_ROOT      "${CMAKE_CURRENT_SOURCE_DIR}" )
set( CINDER_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proj/cmake" )

include( ${CINDER_CMAKE_DIR}/configure.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_configure.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

if( CINDER_LINUX )
	include( "${CINDER_CMAKE_DIR}/platform_linux.cmake" )
elseif( CINDER_MAC )
	include( "${CINDER_CMAKE_DIR}/platform_macosx.cmake" )
elseif( CINDER_ANDROID )
	include( "${CINDER_CMAKE_DIR}/platform_android.cmake" )
elseif( CINDER_MSW )
	include( "${CINDER_CMAKE_DIR}/platform_msw.cmake" )
else()
	message( FATAL_ERROR "no target defined for system: '${CMAKE_SYSTEM_NAME}.'" )
endif()

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )

if( CINDER_BUILD_SAMPLES )
	# TODO: move these to separate file and make easier to add new samples
	add_subdirectory( ${CINDER_ROOT}/samples/BasicApp/proj/cmake )
	add_subdirectory( ${CINDER_ROOT}/samples/_audio/NodeBasic/proj/cmake )
	add_subdirectory( ${CINDER_ROOT}/samples/_audio/BufferPlayer/proj/cmake )
	add_subdirectory( ${CINDER_ROOT}/samples/_opengl/Cube/proj/cmake )
	add_subdirectory( ${CINDER_ROOT}/samples/_opengl/NormalMapping/proj/cmake )
	add_subdirectory( ${CINDER_ROOT}/samples/_opengl/ObjLoader/proj/cmake )
endif()
