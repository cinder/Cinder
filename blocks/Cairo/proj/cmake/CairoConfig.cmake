if( NOT TARGET Cairo )
	
  get_filename_component( CINDER_CAIRO_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../include" ABSOLUTE )
  get_filename_component( CINDER_CAIRO_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
  get_filename_component( CINDER_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE )

	list( APPEND CINDER_CAIRO_SOURCES ${CINDER_CAIRO_SOURCE_PATH}/Cairo.cpp )

	add_library( Cairo ${CINDER_CAIRO_SOURCES} )
	
  if( NOT TARGET cinder )
  	include( "${CINDER_PATH}/proj/cmake/configure.cmake" )
 		find_package( cinder REQUIRED PATHS
 		 	"${CINDER_PATH}/${CINDER_LIB_DIRECTORY}"
  		"$ENV{CINDER_PATH}/${CINDER_LIB_DIRECTORY}" )
  endif()

	get_target_property( TEST_VAR cinder IMPORTED_LOCATION_RELEASE )
	message( STATUS TEST_VAR: "${TEST_VAR}" )
	
	string( TOLOWER "${CINDER_TARGET}" CINDER_TARGET_LOWER )

	target_include_directories( Cairo PUBLIC 
		${CINDER_CAIRO_INCLUDE_PATH}/${CINDER_TARGET_LOWER}/cairo 
		${CINDER_CAIRO_INCLUDE_PATH} 
	)
  target_include_directories( Cairo PRIVATE BEFORE "${CINDER_PATH}/include" )

	target_compile_options( Cairo PUBLIC "-std=c++11" )

	get_filename_component( CAIRO_LIBS_PATH "${CMAKE_CURRENT_LIST_DIR}/../../lib/${CINDER_TARGET_LOWER}" ABSOLUTE )
	target_link_libraries( Cairo 
		PUBLIC ${CAIRO_LIBS_PATH}/libcairo.a 
		PUBLIC ${CAIRO_LIBS_PATH}/libpixman-1.a 
		PUBLIC ${CAIRO_LIBS_PATH}/libpng.a 
		PRIVATE "${CINDER_PATH}/${CINDER_LIB_DIRECTORY}/libcinder.a"
	)

endif()
