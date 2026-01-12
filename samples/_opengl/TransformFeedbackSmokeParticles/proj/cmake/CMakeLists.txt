cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( opengl-TransformFeedbackSmokeParticles )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
	SOURCES     ${APP_PATH}/src/TransformFeedbackSmokeParticlesApp.cpp
	CINDER_PATH ${CINDER_PATH}
)

if( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
    add_custom_command( TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND plutil -replace NSHighResolutionCapable -bool false
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Info.plist"
    )
endif()
