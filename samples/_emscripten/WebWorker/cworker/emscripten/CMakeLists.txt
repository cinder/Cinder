cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( CppWebWorker )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderEmscriptenApp.cmake" )

ci_emscripten_app(
	SOURCES     ${APP_PATH}/src/worker.cpp
	CINDER_PATH ${CINDER_PATH}
	EXPORT_FROM_WORKER '_onmessage'
	BUILD_AS_WORKER TRUE
	OUTPUT_NAME worker 
	OUTPUT_DIRECTORY ../../../emscripten/build/Debug
)
