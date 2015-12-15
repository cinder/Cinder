cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

message( "Building Cinder for ${CINDER_PLATFORM}" )

set( CINDER_SRC_DIR 	"${CINDER_ROOT}/src" )
set( CINDER_INC_DIR		"${CINDER_ROOT}/include" )

if( NOT CMAKE_BUILD_TYPE )
	message( "CMAKE_BUILD_TYPE not specified, defaulting to Debug" )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		FORCE
	)
endif()

set( CINDER_INCLUDE_USER
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/json
	${CINDER_INC_DIR}/tinyexr
	${CINDER_SRC_DIR}/linebreak
	${CINDER_SRC_DIR}/oggvorbis
	${CINDER_SRC_DIR}/r8brain
)

set( CINDER_INCLUDE_SYSTEM
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/oggvorbis
	${CINDER_SRC_DIR}/AntTweakBar
)

include_directories( ${CINDER_INCLUDE_USER} )
include_directories( ${CINDER_INCLUDE_SYSTEM} SYSTEM )
