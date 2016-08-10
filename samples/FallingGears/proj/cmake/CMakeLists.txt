cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( FallingGears )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( SAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${SAMPLE_DIR}/src/FallingGearsApp.cpp
	${SAMPLE_DIR}/src/AudioController.cpp
	${SAMPLE_DIR}/src/Box2dUtils.cpp
	${SAMPLE_DIR}/src/Config.cpp
	${SAMPLE_DIR}/src/SceneController.cpp
	${SAMPLE_DIR}/src/SceneObjects.cpp
	${SAMPLE_DIR}/src/Synths.cpp
)

ci_make_app(
	SOURCES     ${SRC_FILES}
	CINDER_PATH ${CINDER_PATH}
	BLOCKS      Box2D
)
