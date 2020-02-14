if( NOT TARGET Cairo )
	get_filename_component( CAIRO_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( CAIRO_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../include" ABSOLUTE )

	list( APPEND CAIRO_SOURCES
		${CAIRO_SOURCE_PATH}/Cairo.cpp
	)

	add_library( Cairo ${CAIRO_SOURCES} )

	target_include_directories( Cairo PUBLIC "${CAIRO_INCLUDE_PATH}" )
	target_link_libraries( Cairo PRIVATE cinder )
	
	# sudo apt-get install libcairo2-dev
	set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}" )
    find_package( Cairo REQUIRED )
    target_include_directories( Cairo PUBLIC "${CAIRO_INCLUDE_DIRS}")
    target_link_libraries( Cairo PUBLIC "${CAIRO_LIBRARIES}")
    
    # sudo apt-get install libfreetype6-dev
    find_package( Freetype REQUIRED )
    target_include_directories( Cairo PRIVATE ${FREETYPE_INCLUDE_DIRS} )
    target_link_libraries( Cairo PRIVATE ${FREETYPE_LIBRARIES} )
endif()
