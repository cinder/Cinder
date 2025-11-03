cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CINDER_PATH}/${CINDER_LIB_DIRECTORY} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CINDER_PATH}/${CINDER_LIB_DIRECTORY} )

ci_log_v( "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )

# The type is based on the value of the BUILD_SHARED_LIBS variable.
# When OFF ( default value ) Cinder will be built as a static lib
# and when ON as a shared library.
# See https://cmake.org/cmake/help/v3.0/command/add_library.html for more info.
add_library(
	cinder
    ${CINDER_SRC_FILES}
)

target_include_directories( cinder BEFORE INTERFACE ${CINDER_INCLUDE_USER_INTERFACE} )
target_include_directories( cinder SYSTEM BEFORE INTERFACE ${CINDER_INCLUDE_SYSTEM_INTERFACE} )

target_include_directories( cinder BEFORE PRIVATE ${CINDER_INCLUDE_USER_PRIVATE} )
target_include_directories( cinder SYSTEM BEFORE PRIVATE ${CINDER_INCLUDE_SYSTEM_PRIVATE} )

target_link_libraries( cinder PUBLIC ${CINDER_LIBS_DEPENDS}  )

target_compile_definitions( cinder PUBLIC ${CINDER_DEFINES} )

# Visual Studio and Xcode generators adds a ${CMAKE_BUILD_TYPE} to the ARCHIVE 
# and LIBRARY directories. Override the directories so, ${CMAKE_BUILD_TYPE} doesn't double up.
if( CINDER_MSW )
	set( PLATFORM_TOOLSET "$(PlatformToolset)" )
	if( NOT ( "${CMAKE_GENERATOR}" MATCHES "Visual Studio.+" ) )
		# Assume Visual Studio 2019
        set( PLATFORM_TOOLSET "v142" )
		if( MSVC_VERSION LESS 1920 ) # Visual Studio 2015
            set( PLATFORM_TOOLSET "v140" )
        elseif( MSVC_VERSION LESS 1900 ) # Visual Studio 2013
            set( PLATFORM_TOOLSET "v120" )
        elseif( MSVC_VERSION LESS 1800 ) 
			message( FATAL_ERROR "Unsupported MSVC version: ${MSVC_VERSION}" )
		endif()
	endif()

    set( OUTPUT_DIRECTORY_BASE "${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}" )
    set_target_properties( cinder PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG			"${OUTPUT_DIRECTORY_BASE}/Debug/${PLATFORM_TOOLSET}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE		"${OUTPUT_DIRECTORY_BASE}/Release/${PLATFORM_TOOLSET}"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL		"${OUTPUT_DIRECTORY_BASE}/MinSizeRel/${PLATFORM_TOOLSET}"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO	"${OUTPUT_DIRECTORY_BASE}/RelWithDebInfo/${PLATFORM_TOOLSET}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG			"${OUTPUT_DIRECTORY_BASE}/Debug/${PLATFORM_TOOLSET}|"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE		"${OUTPUT_DIRECTORY_BASE}/Release/${PLATFORM_TOOLSET}"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL		"${OUTPUT_DIRECTORY_BASE}/MinSizeRel/${PLATFORM_TOOLSET}"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO	"${OUTPUT_DIRECTORY_BASE}/RelWithDebInfo/${PLATFORM_TOOLSET}"
        STATIC_LIBRARY_FLAGS_DEBUG				"${CINDER_STATIC_LIBS_FLAGS_DEBUG} ${CINDER_STATIC_LIBS_DEPENDS_DEBUG}" 
        STATIC_LIBRARY_FLAGS_RELEASE			"${CINDER_STATIC_LIBS_FLAGS_RELEASE} ${CINDER_STATIC_LIBS_DEPENDS_RELEASE}"
        STATIC_LIBRARY_FLAGS_MINSIZEREL			"${CINDER_STATIC_LIBS_FLAGS_RELEASE} ${CINDER_STATIC_LIBS_DEPENDS_RELEASE}"
        STATIC_LIBRARY_FLAGS_RELWITHDEBINFO		"${CINDER_STATIC_LIBS_FLAGS_RELEASE} ${CINDER_STATIC_LIBS_DEPENDS_RELEASE}" 
    )    
elseif( CINDER_MAC )
    set( OUTPUT_DIRECTORY_BASE "${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}" )
    set_target_properties( cinder PROPERTIES 
		ARCHIVE_OUTPUT_DIRECTORY_DEBUG			"${OUTPUT_DIRECTORY_BASE}/Debug"
		ARCHIVE_OUTPUT_DIRECTORY_RELEASE		"${OUTPUT_DIRECTORY_BASE}/Release"
		ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL		"${OUTPUT_DIRECTORY_BASE}/MinSizeRel"
		ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO	"${OUTPUT_DIRECTORY_BASE}/RelWithDebInfo"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG			"${OUTPUT_DIRECTORY_BASE}/Debug"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE		"${OUTPUT_DIRECTORY_BASE}/Release"
		LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL		"${OUTPUT_DIRECTORY_BASE}/MinSizeRel"
		LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO	"${OUTPUT_DIRECTORY_BASE}/RelWithDebInfo"
		STATIC_LIBRARY_FLAGS					"${CINDER_STATIC_LIBS_DEPENDS}" 
	)
elseif( CINDER_COCOA_TOUCH )
elseif( CINDER_LINUX )
endif()

# Enforce the minimum C++ standard Cinder requires.
if( CINDER_MSW AND MSVC )
    if( MSVC_VERSION LESS 1920 )
        message( FATAL_ERROR "Cinder requires Visual Studio 2019 (MSVC 19.20) or newer." )
    endif()
endif()

# Determine C++ standard for Cinder (default 20, allow user override)
if( CMAKE_CXX_STANDARD )
    set( CINDER_CXX_STANDARD ${CMAKE_CXX_STANDARD} )
else()
    set( CINDER_CXX_STANDARD 20 )
endif()

# Validate minimum
if( CINDER_CXX_STANDARD LESS 20 )
    message( FATAL_ERROR "Cinder requires C++20 or later. CMAKE_CXX_STANDARD is set to ${CINDER_CXX_STANDARD}" )
endif()

# Set C++ standard for cinder target
target_compile_features( cinder PUBLIC cxx_std_${CINDER_CXX_STANDARD} )

# Determine CXX_EXTENSIONS: default OFF (prevents "namespace linux" issue)
# Only enable if user explicitly sets CMAKE_CXX_EXTENSIONS=ON
if( DEFINED CMAKE_CXX_EXTENSIONS )
    set( CINDER_CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS} )
else()
    set( CINDER_CXX_EXTENSIONS OFF )
endif()

set_target_properties( cinder PROPERTIES
    CXX_STANDARD ${CINDER_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ${CINDER_CXX_EXTENSIONS}
)

# This file will contain all dependencies, includes, definition, compiler flags and so on..
export( TARGETS cinder FILE ${PROJECT_BINARY_DIR}/${CINDER_LIB_DIRECTORY}/cinderTargets.cmake )

# And this command will generate a file on the ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
# that applications have to pull in order to link successfully with Cinder and its dependencies.
# This specific cinderConfig.cmake file will just hold a path to the above mention
# cinderTargets.cmake file which holds the actual info.
# CINDER_CXX_STANDARD and CINDER_CXX_EXTENSIONS will be substituted into the template
configure_file( ${CMAKE_CURRENT_LIST_DIR}/modules/cinderConfig.buildtree.cmake.in
    "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/cinderConfig.cmake"
)
