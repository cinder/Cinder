include( CMakeParseArguments )

function( ci_make_app )
	set( oneValueArgs APP_NAME CINDER_PATH )
	set( multiValueArgs SOURCES INCLUDES LIBRARIES RESOURCES BLOCKS )

	cmake_parse_arguments( ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if( NOT ARG_APP_NAME )
		set( ARG_APP_NAME "${PROJECT_NAME}" )
	endif()

	if( ARG_UNPARSED_ARGUMENTS )
		message( WARNING "unhandled arguments: ${ARG_UNPARSED_ARGUMENTS}" )
	endif()

	include( "${ARG_CINDER_PATH}/proj/cmake/configure.cmake" )

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
		message( STATUS "LIBRARIES: ${ARG_LIBRARIES}" )
		message( STATUS "CINDER_PATH: ${ARG_CINDER_PATH}" )
		message( STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
		message( STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}" )
		message( STATUS "CINDER_TARGET: ${CINDER_TARGET}" )
		message( STATUS "CINDER_LIB_DIRECTORY: ${CINDER_LIB_DIRECTORY}" )
		message( STATUS "CINDER BLOCKS: ${ARG_BLOCKS}" )
	endif()

	# pull in cinder's exported configuration
	if( NOT TARGET cinder )
		find_package( cinder REQUIRED PATHS
			"${ARG_CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
			"$ENV{Cinder_Dir}/${CINDER_LIB_DIRECTORY}"
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
	elseif( CINDER_LINUX )
		unset( ARG_RESOURCES ) # Don't allow resources to be added to the executable on linux
	endif()

	add_executable( ${ARG_APP_NAME} MACOSX_BUNDLE WIN32 ${ARG_SOURCES} ${ICON_PATH} ${ARG_RESOURCES} )

	target_include_directories( ${ARG_APP_NAME} PUBLIC ${ARG_INCLUDES} )
	target_link_libraries( ${ARG_APP_NAME} cinder ${ARG_LIBRARIES} )

	# Blocks are first searched relative to the sample's CMakeLists.txt file, then within cinder's blocks folder
	foreach( block ${ARG_BLOCKS} )
		get_filename_component( blockModuleDir "${CMAKE_CURRENT_SOURCE_DIR}/${block}/proj/cmake" ABSOLUTE )
		set( blockTarget "" )

		if( EXISTS ${blockModuleDir} )
			list( APPEND CMAKE_MODULE_PATH ${blockModuleDir} )
			get_filename_component( blockTarget "${block}" NAME )
		elseif( EXISTS ${ARG_CINDER_PATH}/blocks/${block}/proj/cmake )
			get_filename_component( blockModuleDir "${ARG_CINDER_PATH}/blocks/${block}/proj/cmake" ABSOLUTE )
			list( APPEND CMAKE_MODULE_PATH "${blockModuleDir}" )
			set( blockTarget "${block}" )
		else()
			message( ERROR " Could not find block: ${block}, checked in cinder/blocks and at path: ${blockModuleDir}" )
		endif()

		if( blockTarget )
			if( CINDER_VERBOSE )
				message( STATUS "Found block with target: ${blockTarget}, module directory: ${blockModuleDir}" )
			endif()

			find_package( ${blockTarget} REQUIRED )
			add_dependencies( ${ARG_APP_NAME} "${blockTarget}" )
			target_link_libraries( ${ARG_APP_NAME} "${blockTarget}" )
		endif()
	endforeach()

	if( CINDER_MAC )
		# set bundle info.plist properties
		set_target_properties( ${ARG_APP_NAME} PROPERTIES
			MACOSX_BUNDLE_BUNDLE_NAME ${ARG_APP_NAME}
			MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}
		)
	endif()

endfunction()
