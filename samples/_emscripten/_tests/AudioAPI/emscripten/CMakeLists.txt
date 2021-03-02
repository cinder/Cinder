cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

# NOTE - this can only be built in Debug mode for the time being

project( AudioAPI )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderEmscriptenApp.cmake" )


ci_emscripten_app(
	SOURCES     ${APP_PATH}/src/Audioapi.cpp
	CINDER_PATH ${CINDER_PATH}
	INCLUDES ${APP_PATH}/include ${APP_PATH}/audioapi
	FLAGS "-s ASSERTIONS=0 -O0 --pre-js ${APP_PATH}/audioapi/AudioWorkletNode.js"
)
