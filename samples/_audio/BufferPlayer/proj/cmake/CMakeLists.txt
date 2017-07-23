cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( audio-BufferPlayer )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${APP_PATH}/src/BufferPlayerApp.cpp
	${APP_PATH}/../common/AudioDrawUtils.cpp
)

ci_make_app(
	APP_NAME    "BufferPlayer"
	CINDER_PATH ${CINDER_PATH}
	SOURCES     ${SRC_FILES}
	INCLUDES    ${APP_PATH}/include
	RESOURCES   ${APP_PATH}/../../data/sound/DrainMagic.ogg
)
