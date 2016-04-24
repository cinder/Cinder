cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( opengl-ObjLoader )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( SAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( CINDER_BUILD_VERBOSE TRUE )

ci_make_app(
	APP_NAME    "ObjLoader"
	CINDER_PATH ${CINDER_PATH}
	SOURCES     ${SAMPLE_DIR}/src/ObjLoaderApp.cpp
	INCLUDES    ${SAMPLE_DIR}/include
	RESOURCES   ${SAMPLE_DIR}/../../data/models/8lbs.obj
)
