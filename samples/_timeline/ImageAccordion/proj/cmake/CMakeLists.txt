cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( timeline-ImageAccordion )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )
get_filename_component( DATA_PATH "${APP_PATH}/../../data" ABSOLUTE )

list( APPEND RESOURCES
		${DATA_PATH}/photo_1.jpg
		${DATA_PATH}/photo_2.jpg
		${DATA_PATH}/photo_3.jpg
		${DATA_PATH}/photo_4.jpg
		${DATA_PATH}/photo_5.jpg
		${DATA_PATH}/photo_6.jpg
		${DATA_PATH}/photo_7.jpg
		${DATA_PATH}/photo_8.jpg
	)

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
	SOURCES     ${APP_PATH}/src/ImageAccordionApp.cpp ${APP_PATH}/src/AccordionItem.cpp
	CINDER_PATH ${CINDER_PATH}
	INCLUDES    ${APP_PATH}/include
	RESOURCES   ${RESOURCES}
)
