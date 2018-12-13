cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

# Suppress compiler checks. Why? CMake seems to be inconsistent
# with how it checks compilers on different platforms.
set( CMAKE_C_COMPILER_WORKS   1 )
set( CMAKE_CXX_COMPILER_WORKS 1 )

# Assume Debug if a build type isn't specified
if( "" STREQUAL "${CMAKE_BUILD_TYPE}" )
	set( CMAKE_BUILD_TYPE "Debug" CACHE FILEPATH "" FORCE )
endif()

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE )
set( CINDER_INC_DIR ${CINDER_DIR}/include )
set( CINDER_SRC_DIR ${CINDER_DIR}/src )
set( CINDER_LIB_DIR ${CINDER_DIR}/lib/emscripten )

### Needs to come atfer compilers are set
project( cinder_emscripten )

### Force some Emscripten CMake variables. CMake seems to be really particular
### about when these vars show up. They need to come after project.
set( CMAKE_AR 							"emcc" CACHE FILEPATH "" FORCE)
set( CMAKE_STATIC_LIBRARY_SUFFIX 		".bc" )
set( CMAKE_C_CREATE_STATIC_LIBRARY 		"<CMAKE_AR> -o <TARGET> <LINK_FLAGS> <OBJECTS>" )
set( CMAKE_CXX_CREATE_STATIC_LIBRARY 	"<CMAKE_AR> -o <TARGET> <LINK_FLAGS> <OBJECTS>" )

set( CINDER_GL_ES_PLATFORM	"es3" )
set( CINDER_GL_ES_FLAGS 	"-DCINDER_GL_ES_3 -s USE_WEBGL2=1 -s FULL_ES3=1" )

# FreeType Flags
set( CINDER_FREETYPE_FLAGS "-DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE" )

# C Flags
set( C_FLAGS "-fvisibility=default -D_UNIX ${CINDER_GL_ES_FLAGS} ${CINDER_FREETYPE_FLAGS} -s USE_GLFW=3 -s DISABLE_EXCEPTION_CATCHING=0" )
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g -O0" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -Os" )

# CPP Flags
set( CXX_FLAGS "-stdlib=libc++ -std=c++11 -fvisibility=default -D_UNIX ${CINDER_GL_ES_FLAGS} ${CINDER_FREETYPE_FLAGS} -s USE_GLFW=3 -s DEMANGLE_SUPPORT=1 -s DISABLE_EXCEPTION_CATCHING=0" )
set( CMAKE_CXX_FLAGS_DEBUG	"${CXX_FLAGS} -g -O0 -fexceptions -frtti" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -Os -fexceptions -frtti" )

list( APPEND CINDER_C_SRC_FILES
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

		#including this causes a multiple symbol error :/ not sure what to do here if needed.
    #${CINDER_SRC_DIR}/freetype/base/ftlcdfil.c
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

	${CINDER_SRC_DIR}/oggvorbis/ogg/bitwise.c
	${CINDER_SRC_DIR}/oggvorbis/ogg/framing.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/analysis.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/bitrate.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/block.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/codebook.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/envelope.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/floor0.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/floor1.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/info.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/lookup.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/lpc.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/lsp.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/mapping0.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/mdct.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/psy.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/registry.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/res0.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/sharedbook.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/smallft.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/synthesis.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/vorbisenc.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/vorbisfile.c
	${CINDER_SRC_DIR}/oggvorbis/vorbis/window.c

	${CINDER_SRC_DIR}/zlib/adler32.c
	${CINDER_SRC_DIR}/zlib/compress.c
	${CINDER_SRC_DIR}/zlib/crc32.c
	${CINDER_SRC_DIR}/zlib/crc32.h
	${CINDER_SRC_DIR}/zlib/deflate.c
	${CINDER_SRC_DIR}/zlib/gzclose.c
	${CINDER_SRC_DIR}/zlib/gzlib.c
	${CINDER_SRC_DIR}/zlib/gzread.c
	${CINDER_SRC_DIR}/zlib/gzwrite.c
	${CINDER_SRC_DIR}/zlib/infback.c
	${CINDER_SRC_DIR}/zlib/inffast.c
	${CINDER_SRC_DIR}/zlib/inflate.c
	${CINDER_SRC_DIR}/zlib/inftrees.c
	${CINDER_SRC_DIR}/zlib/trees.c
	${CINDER_SRC_DIR}/zlib/uncompr.c
	${CINDER_SRC_DIR}/zlib/zconf.h
	${CINDER_SRC_DIR}/zlib/zutil.c
)

list( APPEND CINDER_CXX_SRC_FILES
	${CINDER_SRC_DIR}/cinder/linux/gl_es_load.cpp

	${CINDER_SRC_DIR}/cinder/app/emscripten/AppEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/AppImplEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/PlatformEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/RendererImplGlEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/WindowImplEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/emscripten/EmscriptenVideo.cpp
	#${CINDER_SRC_DIR}/cinder/audio/emscripten/ContextWebAudio.cpp
	${CINDER_SRC_DIR}/cinder/emscripten/globalbindings.cpp
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

	${CINDER_SRC_DIR}/cinder/ip/Blend.cpp
	${CINDER_SRC_DIR}/cinder/ip/Blur.cpp
	${CINDER_SRC_DIR}/cinder/ip/Checkerboard.cpp
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
	${CINDER_SRC_DIR}/cinder/Base64.cpp
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
	#${CINDER_SRC_DIR}/cinder/ConvexHull.cpp
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

	${CINDER_SRC_DIR}/cinder/audio/ChannelRouterNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/Context.cpp
	${CINDER_SRC_DIR}/cinder/audio/DelayNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/Device.cpp
	${CINDER_SRC_DIR}/cinder/audio/FileOggVorbis.cpp
	${CINDER_SRC_DIR}/cinder/audio/FilterNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/GenNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/InputNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/MonitorNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/Node.cpp
	${CINDER_SRC_DIR}/cinder/audio/NodeMath.cpp
	${CINDER_SRC_DIR}/cinder/audio/OutputNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/PanNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/Param.cpp
	${CINDER_SRC_DIR}/cinder/audio/SamplePlayerNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/SampleRecorderNode.cpp
	${CINDER_SRC_DIR}/cinder/audio/Source.cpp
	${CINDER_SRC_DIR}/cinder/audio/Target.cpp
	${CINDER_SRC_DIR}/cinder/audio/Utilities.cpp
	${CINDER_SRC_DIR}/cinder/audio/Voice.cpp
	${CINDER_SRC_DIR}/cinder/audio/WaveTable.cpp

	${CINDER_SRC_DIR}/cinder/audio/dsp/Biquad.cpp
	${CINDER_SRC_DIR}/cinder/audio/dsp/Converter.cpp
	${CINDER_SRC_DIR}/cinder/audio/dsp/ConverterR8brain.cpp
	${CINDER_SRC_DIR}/cinder/audio/dsp/Dsp.cpp
	${CINDER_SRC_DIR}/cinder/audio/dsp/Fft.cpp
	${CINDER_SRC_DIR}/cinder/audio/dsp/ooura/fftsg.cpp

	${CINDER_SRC_DIR}/r8brain/r8bbase.cpp
)


### Include directories
list(APPEND CINDER_INCLUDE_DIRS
	${CINDER_INC_DIR}
	${CINDER_INC_DIR}/freetype
	${CINDER_INC_DIR}/jsoncpp
	${CINDER_INC_DIR}/oggvorbis
	${CINDER_SRC_DIR}/linebreak
	${CINDER_SRC_DIR}/libtess2
	${CINDER_SRC_DIR}/AntTweakBar
	${CINDER_SRC_DIR}/oggvorbis/vorbis
	${CINDER_SRC_DIR}/r8brain
	${CINDER_SRC_DIR}/zlib
	${BOOST_INC_DIR}
	${EMSCRIPTEN_ROOT_PATH}/system/lib/libcxxabi/include
)

### Output path - should output to lib/emscripten
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CINDER_LIB_DIR}" )

### Create our Cinder target.
add_library( cinder STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )

target_include_directories( cinder PUBLIC "$<BUILD_INTERFACE:${CINDER_INCLUDE_DIRS};${CINDER_INCLUDE_DEPENDS}>" )

set_target_properties( cinder PROPERTIES DEBUG_POSTFIX "_d" )
