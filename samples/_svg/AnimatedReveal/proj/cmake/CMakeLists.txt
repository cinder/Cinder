# CMakeLists.txt for svg-AnimatedReveal with Cairo support

cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( svg-AnimatedReveal )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( CAIRO_BLOCK "${CINDER_PATH}/blocks/Cairo" )
set( CAIRO_SOURCES ${CAIRO_BLOCK}/src/Cairo.cpp )
set( CAIRO_INCLUDES ${CAIRO_BLOCK}/include )

if( CINDER_LINUX )
    list( APPEND CMAKE_MODULE_PATH "${CINDER_PATH}/proj/cmake/modules" )
    find_package( Cairo REQUIRED )
    list( APPEND CAIRO_INCLUDES ${CAIRO_INCLUDE_DIRS} )
    set( CAIRO_LIBS ${CAIRO_LIBRARIES} )
elseif( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
    list( APPEND CAIRO_INCLUDES ${CAIRO_BLOCK}/include/macosx )
    set( CAIRO_LIBS
        ${CAIRO_BLOCK}/lib/macosx/libcairo.a
        ${CAIRO_BLOCK}/lib/macosx/libpixman-1.a
        ${CAIRO_BLOCK}/lib/macosx/libpng16.a
    )
elseif( CINDER_MSW )
    list( APPEND CAIRO_INCLUDES ${CAIRO_BLOCK}/include/msw )
    set( CAIRO_LIBS
        ${CAIRO_BLOCK}/lib/msw/x64/cairo-static.lib
        ${CAIRO_BLOCK}/lib/msw/x64/pixman-1.lib
        ${CINDER_PATH}/lib/msw/x64/libpng.lib
    )
endif()

ci_make_app(
    SOURCES     ${APP_PATH}/src/AnimatedRevealApp.cpp ${CAIRO_SOURCES}
    INCLUDES    ${CAIRO_INCLUDES}
    CINDER_PATH ${CINDER_PATH}
)

target_link_libraries( ${PROJECT_NAME} PRIVATE ${CAIRO_LIBS} )

if( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
    add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND plutil -replace NSHighResolutionCapable -bool false
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Info.plist"
    )
endif()
