function( _ci_build_message )
	# get the path of the CMakeLists file evaluating this macro relative to the project's root source directory
	file( RELATIVE_PATH _curr_file "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_FILE}" )

	# Build up all remaining arguments into the _msg var
	set( _msg )
	foreach( i RANGE ${ARGC} )
		set( _msg "${_msg}${ARGV${i}}" )
	endforeach()

	# deposit the concatenated message into 'msg' var for the parent function to use
	set( msg "(${_curr_file}) -- ${_msg}" PARENT_SCOPE )
endfunction()

# Only prints if CINDER_VERBOSE is On
function( ci_log_v )
	if( CINDER_VERBOSE )
		_ci_build_message( "${ARGV}" )
		message( "verbose ${msg}" )
	endif()
endfunction()

# Alawys prints
function( ci_log_i )
	_ci_build_message( "${ARGV}" )
	message( "info ${msg}" )
endfunction()
