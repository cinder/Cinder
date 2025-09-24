# FindCairo.cmake script for locating Cairo library
# Defines CAIRO_FOUND, CAIRO_CFLAGS, CAIRO_INCLUDE_DIRS, CAIRO_LIBRARIES
# Copyright (C) 2007, 2010, Pino Toscano, <pino@kde.org>
# Redistribution allowed under BSD license terms

if(CAIRO_INCLUDE_DIRS AND CAIRO_LIBRARIES)
  set(CAIRO_FOUND TRUE)
else()
  if(NOT WIN32)
    find_package(PkgConfig REQUIRED)
    if(Cairo_FIND_VERSION_COUNT GREATER 0)
      set(_cairo_version_cmp ">=${Cairo_FIND_VERSION}")
    endif()
    pkg_check_modules(_pc_cairo cairo${_cairo_version_cmp})
    if(_pc_cairo_FOUND)
      set(CAIRO_FOUND TRUE)
    endif()
  else()
    set(CAIRO_FOUND TRUE)
  endif()

  if(CAIRO_FOUND)
    set(CAIRO_FOUND FALSE)

    find_library(CAIRO_LIBRARY cairo HINTS ${_pc_cairo_LIBRARY_DIRS})
    set(CAIRO_LIBRARIES "${CAIRO_LIBRARY}")

    find_path(CAIRO_INCLUDE_DIR cairo.h
              HINTS ${_pc_cairo_INCLUDE_DIRS}
              PATH_SUFFIXES cairo)
    set(CAIRO_INCLUDE_DIRS "${CAIRO_INCLUDE_DIR};${_pc_cairo_INCLUDE_DIRS}")

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Cairo DEFAULT_MSG CAIRO_LIBRARIES CAIRO_INCLUDE_DIRS)
  endif()
endif()

mark_as_advanced(CAIRO_CFLAGS CAIRO_INCLUDE_DIRS CAIRO_LIBRARIES)