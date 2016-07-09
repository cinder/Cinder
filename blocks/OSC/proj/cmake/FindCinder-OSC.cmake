get_filename_component( OSC_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.." ABSOLUTE )

add_library( Cinder-OSC ${OSC_SOURCE_PATH}/OSC.cpp )

target_include_directories( Cinder-OSC PUBLIC "${BOX2D_SOURCE_PATH}" )
target_include_directories( Cinder-OSC SYSTEM BEFORE PUBLIC "${CINDER_PATH}/include" )
