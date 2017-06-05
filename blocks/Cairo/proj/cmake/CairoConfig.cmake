if( NOT TARGET OSC )
	get_filename_component( CAIRO_PATH "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	set( CAIRO_INCLUDES
		${CAIRO_PATH}/include
	)
	set( CAIRO_SOURCES
		${CAIRO_PATH}/src/Cairo.cpp
	)

	if ( APPLE )
		set( CAIRO_INCLUDES
			${CAIRO_PATH}/include/macosx/
			${CAIRO_INCLUDES}
		)
		set( CAIRO_LIBRARIES
			${CAIRO_PATH}/lib/macosx/libcairo.a
			${CAIRO_PATH}/lib/macosx/libpixman-1.a
			${CAIRO_PATH}/lib/macosx/libpng16.a
		)
	endif()

	add_library( Cairo ${CAIRO_SOURCES} )
	target_link_libraries( Cairo PUBLIC ${CAIRO_LIBRARIES} )

	target_include_directories( Cairo PUBLIC "${CAIRO_INCLUDES}" )
	target_include_directories( Cairo SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )

	if( NOT TARGET cinder )
		include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
		find_package( cinder REQUIRED PATHS
			"${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
			"$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
	endif()
	target_link_libraries( Cairo PRIVATE cinder )
endif()
