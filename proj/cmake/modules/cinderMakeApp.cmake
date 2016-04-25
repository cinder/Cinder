include( CMakeParseArguments )

function( ci_make_app )
	set( oneValueArgs APP_NAME CINDER_PATH )
	set( multiValueArgs SOURCES INCLUDES RESOURCES )

	cmake_parse_arguments( ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if( NOT ARG_APP_NAME )
		set( ARG_APP_NAME "${PROJECT_NAME}" )
	endif()

	if( ARG_UNPARSED_ARGUMENTS )
		message( WARNING "unhandled arguments: ${ARG_UNPARSED_ARGUMENTS}" )
	endif()

	# Unless already set by the user, make sure runtime output directory is relative to the project folder
	# so that cinder's assets system works.
	if( "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" STREQUAL "" )
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE} )
		# this shouldn't be a warning, but this is currently the only way to see the message in CLion
		# message( WARNING "set CMAKE_RUNTIME_OUTPUT_DIRECTORY to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
	endif()

	if( CINDER_VERBOSE )
		message( STATUS "APP_NAME: ${ARG_APP_NAME}" )
		message( STATUS "SOURCES: ${ARG_SOURCES}" )
		message( STATUS "INCLUDES: ${ARG_INCLUDES}" )
		message( STATUS "CINDER_PATH: ${ARG_CINDER_PATH}" )
		message( STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
		message( STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}" )
		message( STATUS "CINDER_TARGET: ${CINDER_TARGET}" )
	endif()

	# pull in cinder's exported configuration
	if( NOT TARGET cinder )
		message( STATUS "CINDER_ARCHIVE_OUTPUT_DIRECTORY: ${CINDER_ARCHIVE_OUTPUT_DIRECTORY}" )
		find_package( cinder REQUIRED PATHS
			"${ARG_CINDER_PATH}/${CINDER_ARCHIVE_OUTPUT_DIRECTORY}"
			"$ENV{Cinder_Dir}/${CINDER_ARCHIVE_OUTPUT_DIRECTORY}"
		)
	endif()

	if( CINDER_MAC )
		# set icon. TODO: make this overridable
		set( ICON_NAME "CinderApp.icns" )
		set( ICON_PATH "${ARG_CINDER_PATH}/samples/data/${ICON_NAME}" )

		# copy .icns to bundle's resources folder
		set_source_files_properties( ${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
		# copy any other resources specified by user
		set_source_files_properties( ${ARG_RESOURCES} PROPERTIES HEADER_FILE_ONLY ON MACOSX_PACKAGE_LOCATION Resources )
	endif()

	add_executable( ${ARG_APP_NAME} MACOSX_BUNDLE WIN32 ${ARG_SOURCES} ${ICON_PATH} ${ARG_RESOURCES} )
	target_include_directories( ${ARG_APP_NAME} PUBLIC ${ARG_INCLUDES} )
	target_link_libraries( ${ARG_APP_NAME} cinder )

	if( CINDER_MAC )
		# set bundle info.plist properties
		set_target_properties( ${ARG_APP_NAME} PROPERTIES
			MACOSX_BUNDLE_BUNDLE_NAME ${ARG_APP_NAME}
			MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}
		)
	endif()

endfunction()
