cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( FallingGears )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( SAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

# ???: Box2D Find* cmake file should live in modules folder?
set( CMAKE_MODULE_PATH "${CINDER_PATH}/blocks/Box2D/proj/cmake" )

find_package( Cinder-Box2D REQUIRED )

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
)

add_dependencies( FallingGears Cinder-Box2D )
target_link_libraries( FallingGears Cinder-Box2D )
