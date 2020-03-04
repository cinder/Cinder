cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

ci_log_v( "Building Cinder for ${CINDER_TARGET}" )

set( CINDER_SRC_DIR 	"${CINDER_PATH}/src" )
set( CINDER_INC_DIR		"${CINDER_PATH}/include" )

if( NOT CINDER_MSW )
	add_definitions( -Wfatal-errors )
endif()

list( APPEND CMAKE_MODULE_PATH ${CINDER_CMAKE_DIR} ${CMAKE_CURRENT_LIST_DIR}/modules )

# *_INTERFACE includes get added to targets that depend on cinder.
list( APPEND CINDER_INCLUDE_USER_INTERFACE
	${CINDER_INC_DIR}
)

list( APPEND CINDER_INCLUDE_SYSTEM_INTERFACE
	${CINDER_INC_DIR}
)

# *_PRIVATE includes are used by cinder internally, user apps explicitly add these as needed.
list( APPEND CINDER_INCLUDE_USER_PRIVATE
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/jsoncpp
	${CINDER_INC_DIR}/tinyexr
	${CINDER_INC_DIR}/imgui
	${CINDER_SRC_DIR}/linebreak
	${CINDER_SRC_DIR}/oggvorbis/vorbis
	${CINDER_SRC_DIR}/r8brain
)

if( CINDER_HEADLESS_GL_EGL )
	list( APPEND CINDER_INCLUDE_USER_PRIVATE ${CINDER_INC_DIR}/EGL-Registry )
endif()

list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/oggvorbis
	${CINDER_SRC_DIR}/AntTweakBar
)

# find cross-platform packages

# TODO: this can cause a system include location to appear before cinder/include's,
# we need to ensure that can't happen before using this
#find_package( PNG )

if( PNG_FOUND )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
		${PNG_INCLUDE_DIRS}
	)
endif()

if( CINDER_FREETYPE_USE_SYSTEM )
	#	TODO: finish this, not sure what to do about library linking
	#	find_package( Freetype2 REQUIRED )
	#	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE  ${FREETYPE2_INCLUDE_DIRS} )
	#	list( APPEND CINDER_LIBS_DEPENDS 	${FREETYPE2_LIBRARIES} )
else()
	# use freetype copy that ships with cinder
	ci_log_v( "using freetype copy that ships with cinder" )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
		${CINDER_INC_DIR}/freetype
	)
	list( APPEND CINDER_DEFINES "-DFT2_BUILD_LIBRARY;-DFT_DEBUG_LEVEL_TRACE" )
endif()

# declare whether AntTweakBar is available (isn't on mobile devices)
if( CINDER_GL_ES OR CINDER_DISABLE_ANTTWEAKBAR )
	set( CINDER_ANTTWEAKBAR_ENABLED FALSE )
else()
	set( CINDER_ANTTWEAKBAR_ENABLED TRUE )
endif()

if( CINDER_DISABLE_IMGUI )
	set( CINDER_IMGUI_ENABLED FALSE )
else()
	set( CINDER_IMGUI_ENABLED TRUE )
endif()
