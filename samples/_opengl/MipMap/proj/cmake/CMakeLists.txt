cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( opengl-MipMap )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( SAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( RESOURCE_FILES
	${SAMPLE_DIR}/resources/CinderApp_ios.png
	${SAMPLE_DIR}/resources/GL_LIN_MIP_LIN.png
	${SAMPLE_DIR}/resources/GL_LIN_MIP_NEA.png
	${SAMPLE_DIR}/resources/GL_NEA_MIP_LIN.png
	${SAMPLE_DIR}/resources/GL_NEA_MIP_NEA.png
	${SAMPLE_DIR}/resources/anisotropicbutton.png
	${SAMPLE_DIR}/resources/checkerboard.png
	${SAMPLE_DIR}/resources/glgenmipmap.png
	${SAMPLE_DIR}/resources/non_pot_checkerboard.png
	${SAMPLE_DIR}/resources/usergenmipmap.png
	${SAMPLE_DIR}/resources/userresizedmipmap.png
)

ci_make_app(
	SOURCES     ${SAMPLE_DIR}/src/MipMapApp.cpp
	CINDER_PATH ${CINDER_PATH}
	INCLUDES    ${SAMPLE_DIR}/include
	RESOURCES   ${RESOURCE_FILES}
)
