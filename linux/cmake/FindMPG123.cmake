# Copyright (c) 2013 Martin Felis <martin@fysx.org>
# License: Public Domain (Unlicense: http://unlicense.org/)
#
# Try to find MPG123. Sets the following variables:
#   MPG123_FOUND
#   MPG123_INCLUDE_DIR
#   MPG123_LIBRARY

set( MPG123_FOUND false )

set( MPG123_INCLUDE_DIRS /usr/local/include /usr/include )

set( MPG123_LIBRARY_DIRS /usr/lib /usr/local/lib )

find_path( MPG123_INCLUDE_DIR NAMES mpg123.h PATHS ${MPG123_INCLUDE_DIRS} PATH_SUFFIXES include )
find_library( MPG123_LIBRARY  NAMES mpg123   PATHS ${MPG123_LIBRARY_DIRS} PATH_SUFFIXES lib )

mark_as_advanced( MPG123_INCLUDE_DIR MPG123_LIBRARY )

if( MPG123_INCLUDE_DIR AND EXISTS "${MPG123_INCLUDE_DIR}/mpg123.h" )
    file( STRINGS "${MPG123_INCLUDE_DIR}/mpg123.h" MPG123_H REGEX "libmpg123: MPEG Audio Decoder library \\(version [^\"]*\\)$" )

    string( REGEX REPLACE "^.*\\(version ([0-9]+).*\\)$" "\\1" MPG123_VERSION_MAJOR "${MPG123_H}" )
    string( REGEX REPLACE "^.*\\(version [0-9]+\\.([0-9]+).*\\)$" "\\1" MPG123_VERSION_MINOR  "${MPG123_H}" )
    string( REGEX REPLACE "^.*\\(version [0-9]+\\.[0-9]+\\.([0-9]+).*\\)$" "\\1" MPG123_VERSION_PATCH "${MPG123_H}" )
    set( MPG123_VERSION_STRING "${MPG123_VERSION_MAJOR}.${MPG123_VERSION_MINOR}.${MPG123_VERSION_PATCH}" )

    set( MPG123_MAJOR_VERSION "${MPG123_VERSION_MAJOR}" )
    set( MPG123_MINOR_VERSION "${MPG123_VERSION_MINOR}" )
    set( MPG123_PATCH_VERSION "${MPG123_VERSION_PATCH}" )
endif()

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	MPG123 
	REQUIRED_VARS MPG123_LIBRARY MPG123_INCLUDE_DIR 
	VERSION_VAR MPG123_VERSION_STRING
)

if( MPG123_FOUND )
endif()
