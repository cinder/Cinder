cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( svg-EuroMap )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

# Linux-specific Cairo handling
if( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    # Add our custom FindCairo module to the module path
    list(APPEND CMAKE_MODULE_PATH "${CINDER_PATH}/proj/cmake/modules")

    # Find system Cairo
    find_package( Cairo REQUIRED )

    # Include Cairo headers and Cinder Cairo wrapper
    set( CAIRO_SOURCES
        ${CINDER_PATH}/blocks/Cairo/src/Cairo.cpp
    )

    set( CAIRO_INCLUDES
        ${CINDER_PATH}/blocks/Cairo/include
        ${CAIRO_INCLUDE_DIRS}
    )

    ci_make_app(
        SOURCES     ${APP_PATH}/src/EuroMapApp.cpp ${CAIRO_SOURCES}
        INCLUDES    ${CAIRO_INCLUDES}
        LIBRARIES   ${CAIRO_LIBRARIES}
        CINDER_PATH ${CINDER_PATH}
    )

    # Link Cairo libraries and set include directories
    target_link_libraries( ${PROJECT_NAME} PRIVATE ${CAIRO_LIBRARIES} )
    target_include_directories( ${PROJECT_NAME} PRIVATE ${CAIRO_INCLUDE_DIRS} )

else()
    # Use standard CinderBlock for other platforms
    ci_make_app(
        SOURCES     ${APP_PATH}/src/EuroMapApp.cpp
        BLOCKS      Cairo
        CINDER_PATH ${CINDER_PATH}
    )
endif()
