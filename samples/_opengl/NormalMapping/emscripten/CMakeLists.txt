cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( NormalMapping )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderEmscriptenApp.cmake" )


ci_emscripten_app(
	SOURCES     ${APP_PATH}/src/NormalMappingApp.cpp ${APP_PATH}/src/DebugMesh.cpp
	CINDER_PATH ${CINDER_PATH}
	INCLUDES ${APP_PATH}/include
	ASSETS ${APP_PATH}/assets
	RESOURCES ${APP_PATH}/resources
)
