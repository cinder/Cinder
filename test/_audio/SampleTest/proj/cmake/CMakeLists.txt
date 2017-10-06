cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( audio-SampleTest )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( APP_SOURCES
	${APP_PATH}/src/SampleTestApp.cpp
	${CINDER_PATH}/samples/_audio/common/AudioDrawUtils.cpp
)

ci_make_app(
	SOURCES    	${APP_SOURCES}
	CINDER_PATH ${CINDER_PATH}
	INCLUDES	${APP_PATH}/include
	RESOURCES	${APP_PATH}/../data/tone440L220R.wav
)
