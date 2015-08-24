cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset    "${Esc}[m"     )
  set(ColorBold     "${Esc}[1m"    )
  set(Red           "${Esc}[31m"   )
  set(Green         "${Esc}[32m"   )
  set(Yellow        "${Esc}[33m"   )
  set(Blue          "${Esc}[34m"   )
  set(Magenta       "${Esc}[35m"   )
  set(Cyan          "${Esc}[36m"   )
  set(White         "${Esc}[37m"   )
  set(BoldRed       "${Esc}[1;31m" )
  set(BoldGreen     "${Esc}[1;32m" )
  set(BoldYellow    "${Esc}[1;33m" )
  set(BoldBlue      "${Esc}[1;34m" )
  set(BoldMagenta   "${Esc}[1;35m" )
  set(BoldCyan      "${Esc}[1;36m" )
  set(BoldWhite     "${Esc}[1;37m" )
endif()

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE )
set( CINDER_INC_DIR ${CINDER_DIR}/include )
set( CINDER_SRC_DIR ${CINDER_DIR}/src )
set( BOOST_INC_DIR ${CINDER_DIR}/boost )

project( cinder_android )

set( CINDER_TOOLCHAIN_CLANG true )

if( CINDER_TOOLCHAIN_CLANG )
    set(CMAKE_TOOLCHAIN_PREFIX "llvm-" ) 
    SET(CMAKE_C_COMPILER                      "/usr/bin/clang-3.5")
    SET(CMAKE_CXX_COMPILER                    "/usr/bin/clang++-3.5")

    SET (CMAKE_C_FLAGS_INIT                   "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG_INIT             "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL_INIT        "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE_INIT           "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT    "-O2 -g")
    SET(CMAKE_C_FLAGS                         "${CMAKE_C_FLAGS} -fmessage-length=0 ")

    SET (CMAKE_CXX_FLAGS_INIT                 "-Wall")
    SET (CMAKE_CXX_FLAGS_DEBUG_INIT           "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT      "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE_INIT         "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT  "-O2 -g")
    SET (CMAKE_CCC_FLAGS                      "${CMAKE_C_FLAGS} -fmessage-length=0 ")
endif()

message( "${BoldGreen}CINDER_INC_DIR: ${CINDER_INC_DIR}${ColorReset}" )
message( "${BoldGreen}CINDER_SRC_DIR: ${CINDER_SRC_DIR}${ColorReset}" )
message( "" )
message( "${BoldGreen}LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH}${ColorReset}" )
message( "" )
message( "${BoldGreen}CMAKE_C_COMPILER  : ${CMAKE_C_COMPILER}${ColorReset}" )
message( "${BoldGreen}CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}${ColorReset}" )
message( "${BoldGreen}CMAKE_AR          : ${CMAKE_AR}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_LINKER      : ${CMAKE_LINKER}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_NM          : ${CMAKE_NM}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_RANLIB      : ${CMAKE_RANLIB}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_STRIP       : ${CMAKE_STRIP}${ColorReset}" ) 
message( "" )
message( "${BoldGreen}CMAKE_SYSROOT     : ${CMAKE_SYSROOT}${ColorReset}" ) 
message( "" )



include_directories( 
    ${CINDER_INC_DIR} 
    ${CINDER_INC_DIR}/glfw
    ${CINDER_INC_DIR}/jsoncpp 
    ${CINDER_SRC_DIR}/linebreak 
    ${CINDER_SRC_DIR}/libtess2 
    ${BOOST_INC_DIR}  
)

set( C_FLAGS "-fvisibility=default -DFT2_BUILD_LIBRARY" )
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -Os -ffast-math ${C_FLAGS}" )

set( CXX_FLAGS "-stdlib=libc++ -std=c++11 -fvisibility=default -DFT2_BUILD_LIBRARY" )
set( CMAKE_CXX_FLAGS_DEBUG    "${CXX_FLAGS} -g -fexceptions -frtti" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -Os -fexceptions -frtti -ffast-math" )

set( CINDER_C_SRC_FILES
    ${CINDER_SRC_DIR}/glfw/src/context.c
    ${CINDER_SRC_DIR}/glfw/src/init.c
    ${CINDER_SRC_DIR}/glfw/src/input.c
    ${CINDER_SRC_DIR}/glfw/src/monitor.c
    ${CINDER_SRC_DIR}/glfw/src/window.c

    ${CINDER_SRC_DIR}/glfw/src/window.c
    ${CINDER_SRC_DIR}/glfw/src/x11_init.c
    ${CINDER_SRC_DIR}/glfw/src/x11_monitor.c
    ${CINDER_SRC_DIR}/glfw/src/x11_window.c
    ${CINDER_SRC_DIR}/glfw/src/xkb_unicode.c
    ${CINDER_SRC_DIR}/glfw/src/linux_joystick.c
    ${CINDER_SRC_DIR}/glfw/src/posix_time.c
    ${CINDER_SRC_DIR}/glfw/src/posix_tls.c

    ${CINDER_SRC_DIR}/freetype/bdf/bdf.c
    ${CINDER_SRC_DIR}/freetype/cff/cff.c
    ${CINDER_SRC_DIR}/freetype/pcf/pcf.c
    ${CINDER_SRC_DIR}/freetype/pfr/pfr.c
    ${CINDER_SRC_DIR}/freetype/sfnt/sfnt.c
    ${CINDER_SRC_DIR}/freetype/truetype/truetype.c
    ${CINDER_SRC_DIR}/freetype/type1/type1.c
    ${CINDER_SRC_DIR}/freetype/type42/type42.c
    ${CINDER_SRC_DIR}/freetype/winfonts/winfnt.c
    ${CINDER_SRC_DIR}/freetype/base/ftbase.c
    ${CINDER_SRC_DIR}/freetype/base/ftbbox.c
    ${CINDER_SRC_DIR}/freetype/base/ftbdf.c
    ${CINDER_SRC_DIR}/freetype/base/ftbitmap.c
    ${CINDER_SRC_DIR}/freetype/base/ftcid.c
    ${CINDER_SRC_DIR}/freetype/base/ftdebug.c
    ${CINDER_SRC_DIR}/freetype/base/ftfstype.c
    ${CINDER_SRC_DIR}/freetype/base/ftgasp.c
    ${CINDER_SRC_DIR}/freetype/base/ftglyph.c
    ${CINDER_SRC_DIR}/freetype/base/ftgxval.c
    ${CINDER_SRC_DIR}/freetype/base/ftinit.c
    ${CINDER_SRC_DIR}/freetype/base/ftlcdfil.c
    ${CINDER_SRC_DIR}/freetype/base/ftmm.c
    ${CINDER_SRC_DIR}/freetype/base/ftotval.c
    ${CINDER_SRC_DIR}/freetype/base/ftpatent.c
    ${CINDER_SRC_DIR}/freetype/base/ftpfr.c
    ${CINDER_SRC_DIR}/freetype/base/ftstroke.c
    ${CINDER_SRC_DIR}/freetype/base/ftsynth.c
    ${CINDER_SRC_DIR}/freetype/base/ftsystem.c
    ${CINDER_SRC_DIR}/freetype/base/fttype1.c
    ${CINDER_SRC_DIR}/freetype/base/ftwinfnt.c
    ${CINDER_SRC_DIR}/freetype/base/ftxf86.c
    ${CINDER_SRC_DIR}/freetype/raster/raster.c
    ${CINDER_SRC_DIR}/freetype/smooth/smooth.c
    ${CINDER_SRC_DIR}/freetype/autofit/autofit.c
    ${CINDER_SRC_DIR}/freetype/bzip2/ftbzip2.c
    ${CINDER_SRC_DIR}/freetype/cache/ftcache.c
    ${CINDER_SRC_DIR}/freetype/gzip/ftgzip.c
    ${CINDER_SRC_DIR}/freetype/lzw/ftlzw.c
    ${CINDER_SRC_DIR}/freetype/gxvalid/gxvalid.c
    ${CINDER_SRC_DIR}/freetype/otvalid/otvalid.c
    ${CINDER_SRC_DIR}/freetype/psaux/psaux.c
    ${CINDER_SRC_DIR}/freetype/pshinter/pshinter.c
    ${CINDER_SRC_DIR}/freetype/psnames/psnames.c
    ${CINDER_SRC_DIR}/freetype/cid/type1cid.c
    ${CINDER_SRC_DIR}/freetype/bdf/bdflib.c
)

set( CINDER_CXX_SRC_FILES
    ${CINDER_SRC_DIR}/cinder/linux/CinderLinux.cpp

    ${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinux.cpp

    ${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
    ${CINDER_SRC_DIR}/cinder/app/KeyEvent.cpp
    ${CINDER_SRC_DIR}/cinder/app/Platform.cpp
    ${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
    ${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
    ${CINDER_SRC_DIR}/cinder/app/Window.cpp
)


set( CMAKE_DEBUG_POSTFIX "_d" )
if( LINUX_GLES2 )
    add_library( cinder-es2 STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )
else()
    add_library( cinder STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )
endif()
