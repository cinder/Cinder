# AssetLoading example
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}../../../../" ABSOLUTE )
include( "${CINDER_DIR}/proj/cmake/modules/cinderEmscriptenApp.cmake" )

set( SRC_FILES
	src/AssetLoading.cpp
)

ci_emscripten_app(
	SOURCES     ${SRC_FILES}
	CINDER_PATH ${CINDER_DIR}
	ASSETS assets
)