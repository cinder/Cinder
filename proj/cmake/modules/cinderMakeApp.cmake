include( CMakeParseArguments )

function( ci_make_app )
	set( oneValueArgs APP_NAME CINDER_PATH ASSETS_PATH )
	set( multiValueArgs SOURCES INCLUDES LIBRARIES RESOURCES BLOCKS )

	cmake_parse_arguments( ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if( NOT ARG_APP_NAME )
		set( ARG_APP_NAME "${PROJECT_NAME}" )
	endif()

	if( NOT ARG_ASSETS_PATH )
		# Set the default assets path to be in the standard app location (next to proj folder)
		get_filename_component( ARG_ASSETS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../assets" ABSOLUTE )
	endif()

	if( ARG_UNPARSED_ARGUMENTS )
		message( WARNING "unhandled arguments: ${ARG_UNPARSED_ARGUMENTS}" )
	endif()

	option( CINDER_COPY_ASSETS "Copy assets to a folder next to the application. Default is OFF, and a symlink is created that points to the original assets folder." OFF )
	include( "${ARG_CINDER_PATH}/proj/cmake/configure.cmake" )

	if( "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" STREQUAL "" )
		if( DEFINED ENV{CLION_IDE} )
			# By default, CLion places its binary outputs in a global cache location, where assets can't be found in the current
			# folder heirarchy. So we override that, unless the user has specified a custom binary output dir (then they're on their own).
			set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE} )
			# message( WARNING "CLion detected, set CMAKE_RUNTIME_OUTPUT_DIRECTORY to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
		else()
            if( ( "${CMAKE_GENERATOR}" MATCHES "Visual Studio.+" ) OR ( "Xcode" STREQUAL "${CMAKE_GENERATOR}" ) )
			    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
            else()
			    # Append the build type to the output dir
			    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE} )
            endif()
			message( STATUS "set CMAKE_RUNTIME_OUTPUT_DIRECTORY to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
		endif()
	endif()

	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ARG_APP_NAME} )

	ci_log_v( "APP_NAME: ${ARG_APP_NAME}" )
	ci_log_v( "SOURCES: ${ARG_SOURCES}" )
	ci_log_v( "INCLUDES: ${ARG_INCLUDES}" )
	ci_log_v( "LIBRARIES: ${ARG_LIBRARIES}" )
	ci_log_v( "CINDER_PATH: ${ARG_CINDER_PATH}" )
	ci_log_v( "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
	ci_log_v( "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}" )
	ci_log_v( "CINDER_TARGET: ${CINDER_TARGET}" )
	ci_log_v( "CINDER_LIB_DIRECTORY: ${CINDER_LIB_DIRECTORY}" )
	ci_log_v( "CINDER BLOCKS: ${ARG_BLOCKS}" )
	ci_log_v( "ASSETS_PATH: ${ARG_ASSETS_PATH}" )

	# pull in cinder's exported configuration
	if( NOT TARGET cinder )
		find_package( cinder REQUIRED PATHS
			"${ARG_CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
			"$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		)
	endif()

	# ensure the runtime output directory exists, in case we need to copy other files to it
	if( NOT EXISTS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
		file( MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
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
		if( ARG_RESOURCES )
			# copy resources to a folder next to the app names 'resources'. note the folder is flat, so duplicates will be overwritten
			get_filename_component( RESOURCES_DEST_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources" ABSOLUTE )
			message( "copying resources to: ${RESOURCES_DEST_PATH}" )
			if( EXISTS "${RESOURCES_DEST_PATH}" )
				message( "resources destination path exists, removing old first." )
			endif()

			file( COPY "${ARG_RESOURCES}" DESTINATION "${RESOURCES_DEST_PATH}" )

			unset( ARG_RESOURCES ) # Don't allow resources to be added to the executable on linux
		endif()
	elseif( CINDER_MSW )		
		if( MSVC )
			# Override the default /MD with /MT
			foreach( 
				flag_var
				CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO 
				CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO 
			)
				if( ${flag_var} MATCHES "/MD" )
					string( REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}" )
					set( "${flag_var}" "${${flag_var}}" PARENT_SCOPE )
				endif()
			endforeach()
			# Force synchronous PDB writes
			add_compile_options( /FS ) 
			# Force multiprocess compilation
			add_compile_options( /MP )
			# Add lib dirs
			cmake_policy( PUSH )
			cmake_policy( SET CMP0015 OLD )
			link_directories( "${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}" )
			cmake_policy( POP )
		endif()
	endif()

	add_executable( ${ARG_APP_NAME} MACOSX_BUNDLE WIN32 ${ARG_SOURCES} ${ICON_PATH} ${ARG_RESOURCES} )

	target_include_directories( ${ARG_APP_NAME} PUBLIC ${ARG_INCLUDES} )
	target_link_libraries( ${ARG_APP_NAME} cinder ${ARG_LIBRARIES} )

	# Ignore Specific Default Libraries
	if( MSVC )
		set_target_properties( ${ARG_APP_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCPMT" )
	endif()

	# Blocks are first searched relative to the sample's CMakeLists.txt file, then within cinder's blocks folder
	foreach( block ${ARG_BLOCKS} )
		get_filename_component( blockModuleDir "${CMAKE_CURRENT_SOURCE_DIR}/${block}/proj/cmake" ABSOLUTE )
		set( blockName "" )

		if( EXISTS ${blockModuleDir} )
			get_filename_component( blockName "${block}" NAME )
		elseif( EXISTS ${ARG_CINDER_PATH}/blocks/${block}/proj/cmake )
			get_filename_component( blockModuleDir "${ARG_CINDER_PATH}/blocks/${block}/proj/cmake" ABSOLUTE )
			set( blockName "${block}" )
		elseif( IS_DIRECTORY ${block} )
		  	get_filename_component( blockName ${block} NAME )
			set( blockModuleDir "${block}/proj/cmake" )
    		else()
			message( ERROR " Could not find block: ${block}, checked in cinder/blocks and at path: ${blockModuleDir}" )
		endif()

		if( blockName )
			find_package( ${blockName} PATHS ${blockModuleDir} NO_DEFAULT_PATH )
			# First check if a target was defined. If so then includes and extra libraries will automatically be added to the app target.
			if( TARGET "${blockName}" )
				add_dependencies( ${ARG_APP_NAME} "${blockName}" )
				target_link_libraries( ${ARG_APP_NAME} "${blockName}" )
			else()
				# Otherwise, check for either includes for a header-only block or libraries that need to be linked.
				# - sanity check to warn if someone passed in a block with unexpected cmake configuration
				if( NOT ${blockName}_INCLUDES AND NOT ${blockName}_LIBRARIES )
					message( WARNING "no target defined for '${blockName}', so expected either ${blockName}_INCLUDES} or ${blockName}_LIBRARIES to be defined but neither were found." )
				endif()

				if( ${blockName}_INCLUDES )
					target_include_directories( ${ARG_APP_NAME} PUBLIC ${${blockName}_INCLUDES} )
				endif()
				if( ${blockName}_LIBRARIES )
					target_link_libraries( ${ARG_APP_NAME} ${${blockName}_LIBRARIES} )
				endif()
			endif()

			ci_log_v( "Added block named: ${blockName}, module directory: ${blockModuleDir}" )
		endif()
	endforeach()

	if( CINDER_MAC )
		# set bundle info.plist properties
		set_target_properties( ${ARG_APP_NAME} PROPERTIES
			MACOSX_BUNDLE_BUNDLE_NAME ${ARG_APP_NAME}
			MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}
		)
	endif()

	# Handle assets directory so that it can be found relative to the application.
	get_filename_component( ASSETS_DEST_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets" ABSOLUTE )
	if( EXISTS "${ARG_ASSETS_PATH}" AND IS_DIRECTORY "${ARG_ASSETS_PATH}" )

		message( "ARG_ASSETS_PATH: ${ARG_ASSETS_PATH}, ASSETS_DEST_PATH: ${ASSETS_DEST_PATH}" )

		if( EXISTS "${ASSETS_DEST_PATH}" )
			message( STATUS "assets destination path already exists, removing first." )
			file( REMOVE_RECURSE "${ASSETS_DEST_PATH}" )
		endif()

		if( CINDER_COPY_ASSETS )
			# make a hard copy. Skipping the extra 'assets' folder as it we're copying the entire folder over
			get_filename_component( ASSETS_DEST_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" ABSOLUTE )

			file( COPY "${ARG_ASSETS_PATH}" DESTINATION "${ASSETS_DEST_PATH}" )
		else()
			# make a symlink
			execute_process(
					COMMAND "${CMAKE_COMMAND}" "-E" "create_symlink" "${ARG_ASSETS_PATH}" "${ASSETS_DEST_PATH}"
					RESULT_VARIABLE resultCode
			)

			if( NOT resultCode EQUAL 0 )
			    message( WARNING "Failed to symlink '${ARG_ASSETS_PATH}' to '${ASSETS_DEST_PATH}', result: ${resultCode}" )
			endif()
		endif()
	endif()

endfunction()
