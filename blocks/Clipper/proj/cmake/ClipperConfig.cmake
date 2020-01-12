if( NOT TARGET Clipper )
	get_filename_component( CLIPPER_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )

	add_library( CLIPPER 
	    ${CLIPPER_SOURCE_PATH}/clipper.cpp 
	    ${CLIPPER_SOURCE_PATH}/CinderClipper.cpp 
	)

	target_include_directories( CLIPPER PUBLIC "${CLIPPER_SOURCE_PATH}"/../include )
endif()
