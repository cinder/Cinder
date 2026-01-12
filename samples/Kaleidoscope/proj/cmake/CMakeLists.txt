cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Kaleidoscope )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
	SOURCES		${APP_PATH}/src/InstascopeApp.cpp
			${APP_PATH}/src/TextRibbon.cpp
			${APP_PATH}/src/TrianglePiece.cpp
	INCLUDES	${APP_PATH}/include
	CINDER_PATH ${CINDER_PATH}
	RESOURCES	${APP_PATH}/resources/Asap-Regular.ttf
			${APP_PATH}/resources/Kreon-Bold.ttf
)

if( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
    add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND plutil -replace NSHighResolutionCapable -bool false
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Info.plist"
    )
endif()
