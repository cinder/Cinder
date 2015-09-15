cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )

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


set( CINDER_TOOLCHAIN_CLANG true )

if( CINDER_TOOLCHAIN_CLANG )
    set( CMAKE_C_COMPILER	"clang" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER	"clang++" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_AR           "llvm-ar"		CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       "llvm-link" 	CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           "llvm-nm " 		CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       "llvm-ranlib"	CACHE FILEPATH "" FORCE )
    #set( CMAKE_STRIP        ""				CACHE FILEPATH "" FORCE )

    set(CMAKE_C_FLAGS_INIT					"-Wall -std=c99" CACHE STRING "" FORCE )
    set(CMAKE_C_FLAGS_DEBUG_INIT			"-g" CACHE STRING "" FORCE )
    set(CMAKE_C_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" CACHE STRING "" FORCE )
    set(CMAKE_C_FLAGS_RELEASE_INIT			"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" CACHE STRING "" FORCE )
    set(CMAKE_C_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set(CMAKE_CXX_FLAGS_INIT				"-Wall" CACHE STRING "" FORCE )
    set(CMAKE_CXX_FLAGS_DEBUG_INIT			"-g" CACHE STRING "" FORCE )
    set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" CACHE STRING "" FORCE ) 
    set(CMAKE_CXX_FLAGS_RELEASE_INIT		"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" CACHE STRING "" FORCE )
    set(CMAKE_CCC_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )
endif()

# Needs to come ater compilers are set
project( cinder_linux )

# Output path
set( LIBRARY_OUTPUT_PATH "${CINDER_DIR}/lib/linux/${CINDER_ARCH}" )

message( "" )
message( "${BoldGreen}CINDER_ARCH: ${CINDER_ARCH}${ColorReset}" )
message( "" )
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
#message( "${BoldGreen}CMAKE_STRIP       : ${CMAKE_STRIP}${ColorReset}" ) 
#message( "" )
#message( "${BoldGreen}CMAKE_SYSROOT     : ${CMAKE_SYSROOT}${ColorReset}" ) 
message( "" )

include_directories( 
    ${CINDER_INC_DIR} 
    ${CINDER_INC_DIR}/freetype
    ${CINDER_INC_DIR}/glfw
    ${CINDER_INC_DIR}/jsoncpp 
    ${CINDER_SRC_DIR}/linebreak 
    ${CINDER_SRC_DIR}/libtess2 
    ${CINDER_SRC_DIR}/AntTweakBar
    ${BOOST_INC_DIR}  
)

set( C_FLAGS "-fvisibility=default -DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE -D_UNIX" )
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -Os -ffast-math ${C_FLAGS}" )

# Disable these warnings, many of which are coming from Boost
set( CXX_DISABLE_WARNINGS "-Wno-reorder -Wno-unused-private-field -Wno-unused-local-typedef" )
set( CXX_FLAGS "-stdlib=libc++ -std=c++11 -fvisibility=default -DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE -D_UNIX ${CXX_DISABLE_WARNINGS}" )
set( CMAKE_CXX_FLAGS_DEBUG    "${CXX_FLAGS} -g -fexceptions -frtti" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -Os -fexceptions -frtti -ffast-math" )

set( CINDER_C_SRC_FILES
    ${CINDER_SRC_DIR}/glfw/src/context.c
    ${CINDER_SRC_DIR}/glfw/src/init.c
    ${CINDER_SRC_DIR}/glfw/src/input.c
    ${CINDER_SRC_DIR}/glfw/src/monitor.c
    ${CINDER_SRC_DIR}/glfw/src/window.c

    ${CINDER_SRC_DIR}/glfw/src/window.c
    ${CINDER_SRC_DIR}/glfw/src/glx_context.c
    ${CINDER_SRC_DIR}/glfw/src/x11_init.c
    ${CINDER_SRC_DIR}/glfw/src/x11_monitor.c
    ${CINDER_SRC_DIR}/glfw/src/x11_window.c
    ${CINDER_SRC_DIR}/glfw/src/xkb_unicode.c
    ${CINDER_SRC_DIR}/glfw/src/linux_joystick.c
    ${CINDER_SRC_DIR}/glfw/src/posix_time.c
    ${CINDER_SRC_DIR}/glfw/src/posix_tls.c

    ${CINDER_SRC_DIR}/glload/gl_load.c
    ${CINDER_SRC_DIR}/glload/glx_load.c

    ${CINDER_SRC_DIR}/linebreak/linebreak.c
    ${CINDER_SRC_DIR}/linebreak/linebreakdata.c
    ${CINDER_SRC_DIR}/linebreak/linebreakdef.c

    ${CINDER_SRC_DIR}/libtess2/bucketalloc.c
    ${CINDER_SRC_DIR}/libtess2/dict.c
    ${CINDER_SRC_DIR}/libtess2/geom.c
    ${CINDER_SRC_DIR}/libtess2/mesh.c
    ${CINDER_SRC_DIR}/libtess2/priorityq.c
    ${CINDER_SRC_DIR}/libtess2/sweep.c
    ${CINDER_SRC_DIR}/libtess2/tess.c

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

    ${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxGeneric.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxGeneric.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxGeneric.cpp

    ${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
    ${CINDER_SRC_DIR}/cinder/app/KeyEvent.cpp
    ${CINDER_SRC_DIR}/cinder/app/Platform.cpp
    ${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
    ${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
    ${CINDER_SRC_DIR}/cinder/app/Window.cpp

    ${CINDER_SRC_DIR}/cinder/gl/draw.cpp
    ${CINDER_SRC_DIR}/cinder/gl/scoped.cpp
    ${CINDER_SRC_DIR}/cinder/gl/wrapper.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Batch.cpp
    ${CINDER_SRC_DIR}/cinder/gl/BufferObj.cpp
    ${CINDER_SRC_DIR}/cinder/gl/BufferTexture.cpp
    ${CINDER_SRC_DIR}/cinder/gl/ConstantConversions.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Context.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Environment.cpp
    ${CINDER_SRC_DIR}/cinder/gl/EnvironmentCore.cpp
    ${CINDER_SRC_DIR}/cinder/gl/EnvironmentEs.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Fbo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/GlslProg.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Pbo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Query.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Shader.cpp
    ${CINDER_SRC_DIR}/cinder/gl/ShaderPreprocessor.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Sync.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Texture.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TextureFont.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TextureFormatParsers.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TransformFeedbackObj.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Ubo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Vao.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VaoImplCore.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VaoImplEs.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VaoImplSoftware.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Vbo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VboMesh.cpp

    ${CINDER_SRC_DIR}/glload/gl_load_cpp.cpp
    ${CINDER_SRC_DIR}/glload/glx_load_cpp.cpp

    ${CINDER_SRC_DIR}/cinder/ip/Blend.cpp
    ${CINDER_SRC_DIR}/cinder/ip/EdgeDetect.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Fill.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Flip.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Grayscale.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Hdr.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Premultiply.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Resize.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Threshold.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Trim.cpp

    ${CINDER_SRC_DIR}/cinder/svg/Svg.cpp

    ${CINDER_SRC_DIR}/cinder/Area.cpp
    ${CINDER_SRC_DIR}/cinder/BSpline.cpp
    ${CINDER_SRC_DIR}/cinder/BSplineFit.cpp
    ${CINDER_SRC_DIR}/cinder/Buffer.cpp
    ${CINDER_SRC_DIR}/cinder/Camera.cpp
    ${CINDER_SRC_DIR}/cinder/CameraUi.cpp
    #${CINDER_SRC_DIR}/cinder/Capture.cpp
    ${CINDER_SRC_DIR}/cinder/Channel.cpp
    ${CINDER_SRC_DIR}/cinder/CinderAssert.cpp
    ${CINDER_SRC_DIR}/cinder/CinderMath.cpp
    ${CINDER_SRC_DIR}/cinder/Color.cpp
    ${CINDER_SRC_DIR}/cinder/ConvexHull.cpp
    ${CINDER_SRC_DIR}/cinder/DataSource.cpp
    ${CINDER_SRC_DIR}/cinder/DataTarget.cpp
    ${CINDER_SRC_DIR}/cinder/Display.cpp
    ${CINDER_SRC_DIR}/cinder/Exception.cpp
    ${CINDER_SRC_DIR}/cinder/Font.cpp
    ${CINDER_SRC_DIR}/cinder/Frustum.cpp
    ${CINDER_SRC_DIR}/cinder/GeomIo.cpp
    ${CINDER_SRC_DIR}/cinder/ImageIo.cpp
    ${CINDER_SRC_DIR}/cinder/ImageSourceFileRadiance.cpp
    ${CINDER_SRC_DIR}/cinder/ImageSourceFileStbImage.cpp
    ${CINDER_SRC_DIR}/cinder/ImageTargetFileStbImage.cpp
    ${CINDER_SRC_DIR}/cinder/Json.cpp
    ${CINDER_SRC_DIR}/cinder/Log.cpp
    ${CINDER_SRC_DIR}/cinder/Matrix.cpp
    ${CINDER_SRC_DIR}/cinder/ObjLoader.cpp
    ${CINDER_SRC_DIR}/cinder/Path2d.cpp
    ${CINDER_SRC_DIR}/cinder/Perlin.cpp
    ${CINDER_SRC_DIR}/cinder/Plane.cpp
    ${CINDER_SRC_DIR}/cinder/PolyLine.cpp
    ${CINDER_SRC_DIR}/cinder/Rand.cpp
    ${CINDER_SRC_DIR}/cinder/Ray.cpp
    ${CINDER_SRC_DIR}/cinder/Rect.cpp
    ${CINDER_SRC_DIR}/cinder/Shape2d.cpp
    ${CINDER_SRC_DIR}/cinder/Signals.cpp
    ${CINDER_SRC_DIR}/cinder/Sphere.cpp
    ${CINDER_SRC_DIR}/cinder/Stream.cpp
    ${CINDER_SRC_DIR}/cinder/Surface.cpp
    ${CINDER_SRC_DIR}/cinder/System.cpp
    ${CINDER_SRC_DIR}/cinder/Text.cpp
    ${CINDER_SRC_DIR}/cinder/Timeline.cpp
    ${CINDER_SRC_DIR}/cinder/TimelineItem.cpp
    ${CINDER_SRC_DIR}/cinder/Timer.cpp
    ${CINDER_SRC_DIR}/cinder/Triangulate.cpp
    ${CINDER_SRC_DIR}/cinder/TriMesh.cpp
    ${CINDER_SRC_DIR}/cinder/Tween.cpp
    ${CINDER_SRC_DIR}/cinder/Unicode.cpp
    ${CINDER_SRC_DIR}/cinder/Url.cpp
    ${CINDER_SRC_DIR}/cinder/Utilities.cpp
    ${CINDER_SRC_DIR}/cinder/Xml.cpp

    ${CINDER_SRC_DIR}/jsoncpp/jsoncpp.cpp

    ${CINDER_SRC_DIR}/cinder/params/Params.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/LoadOGL.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/LoadOGLCore.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/TwBar.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/TwColors.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/TwFonts.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/TwOpenGL.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/TwOpenGLCore.cpp
    ${CINDER_SRC_DIR}/AntTweakBar/TwMgr.cpp
)

set( CMAKE_DEBUG_POSTFIX "_d" )
add_library( cinder STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )
