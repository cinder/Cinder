cmake_minimum_required( VERSION 3.10 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( CubeMapLayoutTest )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
    APP_NAME    "CubeMapLayoutTest"
    CINDER_PATH ${CINDER_PATH}
    SOURCES     ${APP_PATH}/src/CubeMapLayoutApp.cpp
    INCLUDES    ${APP_PATH}/include
    RESOURCES   ${APP_PATH}/assets/horizontal_cross.jpg
                 ${APP_PATH}/assets/vertical_cross.png
                 ${APP_PATH}/assets/vertical_cross_layout.png
                 ${APP_PATH}/assets/horizontal.png
                 ${APP_PATH}/assets/vertical.hdr
)
