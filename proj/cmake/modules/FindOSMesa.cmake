# Try to find Mesa off-screen library and include dir.
# Once done this will define
#
# OSMesa_FOUND        - true if OSMesa has been found
# OSMesa_INCLUDE_DIRS - where the GL/osmesa.h can be found
# OSMesa_LIBRARIES    - Link this to use OSMesa
# OSMesa_VERSION      - Version of OSMesa found
# OSMesa::OSMesa      - Imported target

find_path(OSMESA_INCLUDE_DIR
  NAMES GL/osmesa.h
  PATHS "${OSMESA_ROOT}/include"
        "$ENV{OSMESA_ROOT}/include"
        /usr/openwin/share/include
        /opt/graphics/OpenGL/include
  DOC   "OSMesa include directory")
mark_as_advanced(OSMESA_INCLUDE_DIR)

find_library(OSMESA_LIBRARY
  NAMES OSMesa OSMesa16 OSMesa32
  PATHS "${OSMESA_ROOT}/lib"
        "$ENV{OSMESA_ROOT}/lib"
        /opt/graphics/OpenGL/lib
        /usr/openwin/lib
  DOC   "OSMesa library")
mark_as_advanced(OSMESA_LIBRARY)

if (OSMESA_INCLUDE_DIR AND EXISTS "${OSMESA_INCLUDE_DIR}/GL/osmesa.h")
  file(STRINGS "${OSMESA_INCLUDE_DIR}/GL/osmesa.h" _OSMesa_version_lines
    REGEX "OSMESA_[A-Z]+_VERSION")
  string(REGEX REPLACE ".*# *define +OSMESA_MAJOR_VERSION +([0-9]+).*" "\\1" _OSMesa_version_major "${_OSMesa_version_lines}")
  string(REGEX REPLACE ".*# *define +OSMESA_MINOR_VERSION +([0-9]+).*" "\\1" _OSMesa_version_minor "${_OSMesa_version_lines}")
  string(REGEX REPLACE ".*# *define +OSMESA_PATCH_VERSION +([0-9]+).*" "\\1" _OSMesa_version_patch "${_OSMesa_version_lines}")
  set(OSMesa_VERSION "${_OSMesa_version_major}.${_OSMesa_version_minor}.${_OSMesa_version_patch}")
  unset(_OSMesa_version_major)
  unset(_OSMesa_version_minor)
  unset(_OSMesa_version_patch)
  unset(_OSMesa_version_lines)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OSMesa
  REQUIRED_VARS OSMESA_INCLUDE_DIR OSMESA_LIBRARY
  VERSION_VAR OSMesa_VERSION)

if (OSMesa_FOUND)
  set(OSMesa_INCLUDE_DIRS "${OSMESA_INCLUDE_DIR}")
  set(OSMesa_LIBRARIES "${OSMESA_LIBRARY}")

  if (NOT TARGET OSMesa::OSMesa)
    add_library(OSMesa::OSMesa UNKNOWN IMPORTED)
    set_target_properties(OSMesa::OSMesa PROPERTIES
      IMPORTED_LOCATION "${OSMESA_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${OSMESA_INCLUDE_DIR}")
  endif ()
endif ()
