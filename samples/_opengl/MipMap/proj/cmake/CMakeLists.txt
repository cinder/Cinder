cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( opengl-MipMap )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( RESOURCE_FILES
	${APP_PATH}/resources/CinderApp_ios.png
	${APP_PATH}/resources/GL_LIN_MIP_LIN.png
	${APP_PATH}/resources/GL_LIN_MIP_NEA.png
	${APP_PATH}/resources/GL_NEA_MIP_LIN.png
	${APP_PATH}/resources/GL_NEA_MIP_NEA.png
	${APP_PATH}/resources/anisotropicbutton.png
	${APP_PATH}/resources/checkerboard.png
	${APP_PATH}/resources/glgenmipmap.png
	${APP_PATH}/resources/non_pot_checkerboard.png
	${APP_PATH}/resources/usergenmipmap.png
	${APP_PATH}/resources/userresizedmipmap.png
)

ci_make_app(
	SOURCES     ${APP_PATH}/src/MipMapApp.cpp
	CINDER_PATH ${CINDER_PATH}
	INCLUDES    ${APP_PATH}/include
	RESOURCES   ${RESOURCE_FILES}
)
