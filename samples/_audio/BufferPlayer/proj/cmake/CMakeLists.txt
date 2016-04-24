cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( audio-BufferPlayer )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( SAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( CINDER_BUILD_VERBOSE TRUE )

set( SRC_FILES
	${SAMPLE_DIR}/src/BufferPlayerApp.cpp
	${SAMPLE_DIR}/../common/AudioDrawUtils.cpp
)

ci_make_app(
	APP_NAME    "BufferPlayer"
	CINDER_PATH ${CINDER_PATH}
	SOURCES     ${SRC_FILES}
	INCLUDES    ${SAMPLE_DIR}/include
	RESOURCES   ${SAMPLE_DIR}/../../data/sound/DrainMagic.ogg
)
