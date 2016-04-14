cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

message( "Building Cinder for ${CINDER_TARGET}" )

set( CINDER_SRC_DIR 	"${CINDER_ROOT}/src" )
set( CINDER_INC_DIR		"${CINDER_ROOT}/include" )

if( NOT CINDER_MSW )
    add_definitions( -Wfatal-errors )
endif()

list( APPEND CMAKE_MODULE_PATH ${CINDER_CMAKE_DIR} ${CMAKE_CURRENT_LIST_DIR}/modules )

if( NOT CMAKE_BUILD_TYPE )
	message( "CMAKE_BUILD_TYPE not specified, defaulting to Debug" )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		FORCE
	)
endif()

if( CINDER_TARGET_GL )
	string( TOLOWER "${CINDER_TARGET_GL}" CINDER_TARGET_GL_LOWER )
	if( "ogl" STREQUAL "${CINDER_TARGET_GL_LOWER}" ) 
		message( "Building Cinder with Core Profile support." )
		set( CINDER_GL_CORE TRUE )
	elseif( "es2" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es2 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_2 TRUE )
	elseif( "es3" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es3 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3 TRUE )
	elseif( "es31" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es3 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_1 TRUE )
	elseif( "es32" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es3 support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_3_2 TRUE )
	elseif( "es2-rpi" STREQUAL "${CINDER_TARGET_GL_LOWER}" )
		message( "Building Cinder with es2-rpi support." )
		set( CINDER_GL_ES TRUE )
		set( CINDER_GL_ES_2_RPI TRUE )
	else()
		message( STATUS "Unknown target GL : " ${CINDER_TARGET_GL_LOWER} " . Default to Core Profile.")
		set( CINDER_GL_CORE TRUE )
	endif()
else()
		message( STATUS "No target GL has been set. Defaulting to Core Profile.")
		set( CINDER_GL_CORE TRUE )
endif()

list( APPEND CINDER_INCLUDE_USER
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/jsoncpp
	${CINDER_INC_DIR}/tinyexr
	${CINDER_SRC_DIR}/linebreak
	${CINDER_SRC_DIR}/oggvorbis/vorbis
	${CINDER_SRC_DIR}/r8brain
)

list( APPEND CINDER_INCLUDE_SYSTEM
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/oggvorbis
	${CINDER_SRC_DIR}/AntTweakBar
)

# find cross-platform packages

# TODO: this can cause a system include location to appear before cinder/include's,
# we need to ensure that can't happen before using this
#find_package( PNG )

if( PNG_FOUND )
	list( APPEND CINDER_INCLUDE_SYSTEM
		${PNG_INCLUDE_DIRS}
	)
endif()

if( CINDER_FREETYPE_USE_SYSTEM )
#	TODO: finish this, not sure what to do about library linking
#	find_package( Freetype2 REQUIRED )
#	list( APPEND CINDER_INCLUDE_SYSTEM  ${FREETYPE2_INCLUDE_DIRS} )
#	list( APPEND CINDER_LIBS_DEPENDS 	${FREETYPE2_LIBRARIES} )
else()
	# use freetype copy that ships with cinder
	message( "using freetype copy that ships with cinder" )
	list( APPEND CINDER_INCLUDE_SYSTEM
		${CINDER_INC_DIR}/freetype
	)
	list( APPEND CINDER_DEFINES "-DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE"  )
endif()

# declare whether AntTweakBar is available (isn't on mobile devices)
if( CINDER_GL_ES )
	set( CINDER_ANTTWEAKBAR_ENABLED FALSE )
else()
	set( CINDER_ANTTWEAKBAR_ENABLED TRUE )
endif()

message( "CINDER_ANTTWEAKBAR_ENABLED: ${CINDER_ANTTWEAKBAR_ENABLED}" )
