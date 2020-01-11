cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Basic )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

# wildcard on resources
file (GLOB APP_RESOURCES
    ${APP_PATH}/resources/*
)

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
	APP_NAME    ${PROJECT_NAME}App
	CINDER_PATH ${CINDER_PATH}
	SOURCES     ${APP_PATH}/src/_TBOX_PREFIX_App.cpp
	INCLUDES    ${APP_PATH}/include
	RESOURCES   ${APP_RESOURCES}
	ASSETS_PATH ${APP_PATH}/assets
)
