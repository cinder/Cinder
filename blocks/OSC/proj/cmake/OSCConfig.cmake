if( NOT TARGET OSC )
	get_filename_component( OSC_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	add_library( OSC ${OSC_SOURCE_PATH}/cinder/osc/Osc.cpp )

	target_compile_options( OSC PUBLIC "-std=c++11" )

	target_include_directories( OSC PUBLIC "${OSC_SOURCE_PATH}" )
	target_include_directories( OSC SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )
endif()
