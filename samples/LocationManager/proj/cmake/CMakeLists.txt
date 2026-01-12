cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( LocationManager )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_require_platform( MAC IOS )

set( LOCATIONMANAGER_BLOCK "${CINDER_PATH}/blocks/LocationManager" )

ci_make_app(
	SOURCES     ${APP_PATH}/src/LocationManagerApp.cpp
	            ${LOCATIONMANAGER_BLOCK}/src/cinder/LocationManager.cpp
	CINDER_PATH ${CINDER_PATH}
	INCLUDES    ${APP_PATH}/include ${LOCATIONMANAGER_BLOCK}/src
	RESOURCES   ${APP_PATH}/resources/earth.jpg
)

target_link_libraries( ${PROJECT_NAME} PRIVATE "-framework CoreLocation" )

if( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
    add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND plutil -replace NSHighResolutionCapable -bool false
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Info.plist"
    )
endif()
