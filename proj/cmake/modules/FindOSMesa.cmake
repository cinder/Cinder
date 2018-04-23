# Try to find OSMesa. Once done this will define
#
# OSMESA_FOUND			- true if OSMesa has been found
# OSMESA_INCLUDE_DIR	- where the GL/osmesa.h can be found
# OSMESA_LIBRARIES		- Link this to use OSMesa

find_path( OSMESA_INCLUDE_DIR NAMES GL/osmesa.h PATHS  "${OSMESA_ROOT}/include" "$ENV{OSMESA_ROOT}/include" "/usr/local/include" "/usr/include" NO_DEFAULT_PATH )
find_library( OSMESA_LIBRARIES NAMES OSMesa PATHS  "${OSMESA_ROOT}/lib" "$ENV{OSMESA_ROOT}/lib" "/usr/local/lib" "/usr/lib/x86_64-linux-gnu" PATH_SUFFIXES gallium NO_DEFAULT_PATH )
find_library( OSMESA_GL_LIBRARIES Names GL PATHS "${OSMESA_ROOT}/lib" "$ENV{OSMESA_ROOT}/lib" "/usr/local/lib" "/usr/lib/x86_64-linux-gnu" PATH_SUFFIXES gallium NO_DEFAULT_PATH )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( OSMesa DEFAULT_MSG OSMESA_LIBRARIES OSMESA_INCLUDE_DIR OSMESA_GL_LIBRARIES )

mark_as_advanced( OSMESA_INCLUDE_DIR OSMESA_LIBRARY OSMESA_GL_LIBRARIES )
