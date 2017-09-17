
# Only prints if CINDER_VERBOSE is On
function( ci_log_v )
	if( CINDER_VERBOSE )
		file( RELATIVE_PATH curr_file "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_FILE}" )
		set( msg )
		foreach( arg IN LISTS ARGN )
			set( msg "${msg}${arg}" )
		endforeach()

		message( "verbose (${curr_file}) ${msg}" )
	endif()
endfunction()

# Alawys prints
function( ci_log_i )
	file( RELATIVE_PATH curr_file "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_FILE}" )
	set( msg )
	foreach( arg IN LISTS ARGN )
		set( msg "${msg}${arg}" )
	endforeach()

	message( "info (${curr_file}) ${msg}" )
endfunction()
