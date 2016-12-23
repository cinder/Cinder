# Copyright (c) 2013 Martin Felis <martin@fysx.org>
# License: Public Domain (Unlicense: http://unlicense.org/)
#
# Try to find SNDFILE. Sets the following variables:
#   SNDFILE_FOUND
#   SNDFILE_INCLUDE_DIR
#   SNDFILE_LIBRARY

set( SNDFILE_FOUND false )

set( SNDFILE_INCLUDE_DIRS /opt/local/include /usr/local/include /usr/include )
set( SNDFILE_LIBRARY_DIRS /opt/local/lib /usr/local/lib /usr/lib )

set( SNDFILE_LIB_SUFFIXES lib x86_64-linux-gnu arm-linux-gnueabihf aarch64-linux-gnu )

find_path( SNDFILE_INCLUDE_DIR NAMES "sndfile.h" PATHS ${SNDFILE_INCLUDE_DIRS} PATH_SUFFIXES include NO_DEFAULT_PATH )
find_library( SNDFILE_LIBRARY  NAMES "sndfile"   PATHS ${SNDFILE_LIBRARY_DIRS} PATH_SUFFIXES ${SNDFILE_LIB_SUFFIXES} NO_DEFAULT_PATH )

mark_as_advanced( SNDFILE_INCLUDE_DIR SNDFILE_LIBRARY )

if( SNDFILE_LIBRARY AND EXISTS "${SNDFILE_LIBRARY}" )
	get_filename_component( SNDFILE_SO_PATH "${SNDFILE_LIBRARY}" REALPATH )

    string( REGEX REPLACE "^.*/libsndfile\\.so\\.([0-9]+).*$" "\\1" SNDFILE_VERSION_MAJOR "${SNDFILE_SO_PATH}" )
    string( REGEX REPLACE "^.*/libsndfile\\.so\\.[0-9]+\\.([0-9]+).*$" "\\1" SNDFILE_VERSION_MINOR  "${SNDFILE_SO_PATH}" )
    string( REGEX REPLACE "^.*/libsndfile\\.so\\.[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" SNDFILE_VERSION_PATCH "${SNDFILE_SO_PATH}" )
    set( SNDFILE_VERSION_STRING "${SNDFILE_VERSION_MAJOR}.${SNDFILE_VERSION_MINOR}.${SNDFILE_VERSION_PATCH}" )

    set( SNDFILE_MAJOR_VERSION "${SNDFILE_VERSION_MAJOR}" )
    set( SNDFILE_MINOR_VERSION "${SNDFILE_VERSION_MINOR}" )
    set( SNDFILE_PATCH_VERSION "${SNDFILE_VERSION_PATCH}" )
endif()

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	SNDFILE	
	REQUIRED_VARS SNDFILE_LIBRARY SNDFILE_INCLUDE_DIR 
	VERSION_VAR SNDFILE_VERSION_STRING
)

if( SNDFILE_FOUND )
	message( STATUS "  using SNDFILE headers at: ${SNDFILE_INCLUDE_DIR}" )
endif()
