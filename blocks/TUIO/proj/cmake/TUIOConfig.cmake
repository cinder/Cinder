if( NOT TARGET TUIO )
	get_filename_component( TUIO_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )

	add_library( TUIO ${TUIO_SOURCE_PATH}/cinder/tuio/Tuio.cpp )

	target_include_directories( TUIO PUBLIC "${TUIO_SOURCE_PATH}" )

	if( NOT TARGET cinder )
	    include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
	    find_package( cinder REQUIRED PATHS
	        "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
	        "$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()

	# Add OSC block as a dependency
	get_filename_component( OSC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../OSC/proj/cmake" ABSOLUTE )
	find_package( OSC REQUIRED PATHS "${OSC_MODULE_PATH}" )
	add_dependencies( TUIO OSC )
	target_link_libraries( TUIO PUBLIC OSC PRIVATE cinder)
endif()
