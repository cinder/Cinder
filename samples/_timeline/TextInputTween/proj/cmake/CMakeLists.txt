cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( timeline-TextInputTween )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${APP_PATH}/src/TextInputTweenApp.cpp
	${APP_PATH}/src/Character.cpp
)

ci_make_app(
	SOURCES     ${SRC_FILES}
	INCLUDES    ${APP_PATH}/include
	CINDER_PATH ${CINDER_PATH}
	RESOURCES   ${APP_PATH}/resources/Alfphabet-IV.ttf
)
