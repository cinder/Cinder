cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Earthquake )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${APP_PATH}/src/EarthquakeApp.cpp
	${APP_PATH}/src/Earth.cpp
	${APP_PATH}/src/POV.cpp
	${APP_PATH}/src/Quake.cpp
)

set( RESOURCE_FILES
	${APP_PATH}/resources/earthDiffuse.png
	${APP_PATH}/resources/earth.frag
	${APP_PATH}/resources/earthMask.png
	${APP_PATH}/resources/earthNormal.png
	${APP_PATH}/resources/earth.vert
	${APP_PATH}/resources/quake.frag
	${APP_PATH}/resources/quake.vert
	${APP_PATH}/resources/stars.jpg
	${APP_PATH}/resources/stars.png
)

ci_make_app(
	SOURCES     ${SRC_FILES}
	CINDER_PATH ${CINDER_PATH}
	INCLUDES    ${APP_PATH}/include
	RESOURCES   ${RESOURCE_FILES}
)
