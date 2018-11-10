cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Kaleidoscope )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
	SOURCES		${APP_PATH}/src/InstascopeApp.cpp
			${APP_PATH}/src/InstagramStream.cpp
			${APP_PATH}/src/TextRibbon.cpp
			${APP_PATH}/src/TrianglePiece.cpp
	INCLUDES	${APP_PATH}/include
	CINDER_PATH ${CINDER_PATH}
)
