if( NOT TARGET OSC )
	get_filename_component( OSC_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	add_library( OSC ${OSC_SOURCE_PATH}/cinder/osc/Osc.cpp )

	target_include_directories( OSC PUBLIC "${OSC_SOURCE_PATH}" )
	target_include_directories( OSC SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		    find_package( cinder REQUIRED PATHS
		        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
		        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
	target_link_libraries( OSC PRIVATE cinder )
	
endif()



