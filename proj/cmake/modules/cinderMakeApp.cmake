include( CMakeParseArguments )

function( ci_make_app )
	set( oneValueArgs APP_NAME CINDER_PATH ICON )
	set( multiValueArgs SOURCES INCLUDES LIBRARIES RESOURCES BLOCKS )

	cmake_parse_arguments( ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if( NOT ARG_APP_NAME )
		set( ARG_APP_NAME "${PROJECT_NAME}" )
	endif()

	if( ARG_UNPARSED_ARGUMENTS )
		message( WARNING "unhandled arguments: ${ARG_UNPARSED_ARGUMENTS}" )
	endif()

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
		if( CINDER_MAC )
			message( STATUS "ICON: ${ARG_ICON}" )
		endif()
	endif()

	# pull in cinder's exported configuration
	if( NOT TARGET cinder )
		find_package( cinder REQUIRED PATHS
			"${ARG_CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
			"$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		)
	endif()

	if( CINDER_MAC )
		if( ARG_ICON )
			# Icon is first searched relative to the sample's CMakeLists.txt file, then within the app's resources folder
			get_filename_component( ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_ICON}" ABSOLUTE )
			if( EXISTS "${ICON_PATH}" )
				get_filename_component( ICON_NAME ${ICON_PATH} NAME )
			elseif( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../resources/${ARG_ICON}" )
				set( ICON_NAME ${ARG_ICON} )
				get_filename_component( ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../resources/${ICON_NAME}" ABSOLUTE )
			else()
				message( ERROR " Could not find icon: ${ARG_ICON}" )
			endif()
		else()
			set( ICON_NAME "CinderApp.icns" )
			set( ICON_PATH "${ARG_CINDER_PATH}/samples/data/${ICON_NAME}" )
		endif()

		# copy .icns to bundle's resources folder
		if( ICON_PATH )
			set_source_files_properties( ${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
		endif()
		# copy any other resources specified by user
		set_source_files_properties( ${ARG_RESOURCES} PROPERTIES HEADER_FILE_ONLY ON MACOSX_PACKAGE_LOCATION Resources )
	elseif( CINDER_LINUX )
		unset( ARG_RESOURCES ) # Don't allow resources to be added to the executable on linux
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

			if( CINDER_VERBOSE )
				message( STATUS "Added block named: ${blockName}, module directory: ${blockModuleDir}" )
			endif()
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
