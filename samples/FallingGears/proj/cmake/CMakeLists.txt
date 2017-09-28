cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( FallingGears )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${APP_PATH}/src/FallingGearsApp.cpp
	${APP_PATH}/src/AudioController.cpp
	${APP_PATH}/src/Box2dUtils.cpp
	${APP_PATH}/src/Config.cpp
	${APP_PATH}/src/SceneController.cpp
	${APP_PATH}/src/SceneObjects.cpp
	${APP_PATH}/src/Synths.cpp
)

ci_make_app(
	SOURCES     ${SRC_FILES}
	CINDER_PATH ${CINDER_PATH}
	BLOCKS      Box2D
)
