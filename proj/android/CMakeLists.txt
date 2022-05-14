cmake_minimum_required( VERSION 3.10.2 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

# Suppress compiler checks. Why? CMake seems to be inconsistent
# with how it checks compilers on different platforms.
set( CMAKE_C_COMPILER_WORKS   1 )
set( CMAKE_CXX_COMPILER_WORKS 1 )

project( cinder_android )

# Output path
set( LIBRARY_OUTPUT_PATH ${CINDER_DIR}/lib/android/${NDK_PLATFORM_OUT}/${NKD_ARCH_OUT} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH} )

include_directories( 
    ${CINDER_INC_DIR} 
    ${CINDER_INC_DIR}/freetype
    ${CINDER_INC_DIR}/jsoncpp
    ${CINDER_INC_DIR}/oggvorbis
    ${CINDER_SRC_DIR}/linebreak
    ${CINDER_SRC_DIR}/libtess2
    ${CINDER_SRC_DIR}/oggvorbis/vorbis
    ${CINDER_SRC_DIR}/r8brain
    ${CINDER_INC_DIR}/tinyexr
)

# Arch flags
if( "armeabi" STREQUAL "${NDK_ARCH}" )
    if( NDK_TOOLCHAIN_CLANG )
        set( ARCH_FLAGS "-target armv5te-none-linux-androideabi -march=armv5te -mtune=xscale -msoft-float -fpic -fstack-protector-strong" )
    else()
  		set( ARCH_FLAGS "-march=armv5te -mtune=xscale -msoft-float -mthumb" )
	endif()
elseif( "armeabi-v7a" STREQUAL "${NDK_ARCH}" )
    if( NDK_TOOLCHAIN_CLANG )
        set( ARCH_FLAGS "-target armv7-none-linux-androideab -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -fstack-protector -fpic" )
    else()
        set( ARCH_FLAGS "-march=armv7-a -mfpu=neon -mthumb" )
    endif()
elseif( "arm64-v8a" STREQUAL "${NDK_ARCH}" )
	if( NDK_TOOLCHAIN_CLANG )
		set( ARCH_FLAGS "-target aarch64-none-linux-android -fstack-protector-strong -fpic" )
	endif()
elseif( "x86" STREQUAL "${NDK_ARCH}" )
	if( NDK_TOOLCHAIN_CLANG )
		set( ARCH_FLAGS "-target i686-none-linux-android -fstack-protector-strong -fPIC" )
	endif()
elseif( "x86_64" STREQUAL "${NDK_ARCH}" )
	if( NDK_TOOLCHAIN_CLANG )
		set( ARCH_FLAGS "-target x86_64-none-linux-android -fstack-protector-strong -fPIC" )
	endif()
elseif( "mips" STREQUAL "${NDK_ARCH}" )
	if( NDK_TOOLCHAIN_CLANG )
		set( ARCH_FLAGS "-target mipsel-none-linux-android -fpic -fno-strict-aliasing -finline-functions -fmessage-length=0" )
	endif()
elseif( "mips64" STREQUAL "${NDK_ARCH}" )
	if( NDK_TOOLCHAIN_CLANG )
		set( ARCH_FLAGS "-target mips64el-none-linux-android -fpic -fno-strict-aliasing -finline-functions -fmessage-length=0" )
	endif()
endif()

# C_FLAGS
if( NDK_TOOLCHAIN_CLANG )
    set( C_FLAGS "-DANDROID -DFT2_BUILD_LIBRARY ${ARCH_FLAGS} -fvisibility=default" )
else()
    set( C_FLAGS "-DFT2_BUILD_LIBRARY ${ARCH_FLAGS} -fvisibility=default" )
endif()
if( NDK_GLES2 )
    set( C_FLAGS "-DCINDER_GL_ES_2 ${C_FLAGS}")
elseif( NDK_GLES31 )
    set( C_FLAGS "-DCINDER_GL_ES_3_1 ${C_FLAGS}")
endif()
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -Os -ffast-math ${C_FLAGS}" )

# CXX_FLAGS
if( NDK_TOOLCHAIN_CLANG )
    set( CXX_FLAGS "-gcc-toolchain ${NDK_ROOT}/toolchains/arm-linux-androideabi-4.9/prebuilt/${NDK_TOOLCHAIN_HOST} ${ARCH_FLAGS} -fvisibility=default -std=c++11 -stdlib=libstdc++ -ffunction-sections -funwind-tables -Wno-invalid-command-line-argument -Wno-unused-command-line-argument -no-canonical-prefixes -fomit-frame-pointer -DANDROID -DFT2_BUILD_LIBRARY" )
	include_directories(
		${NDK_ROOT}/sources/cxx-stl/llvm-libc++/include
        ${NDK_ROOT}/sources/cxx-stl/llvm-libc++/libs/${NDK_ARCH}/include
	)

if( NDK_GLES2 )
    set( CXX_FLAGS "-DCINDER_GL_ES_2 ${CXX_FLAGS}")
elseif( NDK_GLES31 )
    set( CXX_FLAGS "-DCINDER_GL_ES_3_1 ${CXX_FLAGS}")
endif()
set( CMAKE_CXX_FLAGS_DEBUG   "${CXX_FLAGS} -g -rdynamic -fexceptions -frtti" )
set( CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS} -Os -rdynamic -fexceptions -frtti -ffast-math" )

set( CINDER_C_SRC_FILES
    ${CINDER_SRC_DIR}/cinder/app/android/android_native_app_glue.c

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
)

set( CINDER_CXX_SRC_FILES
    ${CINDER_SRC_DIR}/glad/glad_es.c

    ${CINDER_SRC_DIR}/cinder/android/libc_helper.cpp
    ${CINDER_SRC_DIR}/cinder/android/AndroidDevLog.cpp
    ${CINDER_SRC_DIR}/cinder/android/CinderAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/android/JniHelper.cpp
    ${CINDER_SRC_DIR}/cinder/android/LogCatStream.cpp
    ${CINDER_SRC_DIR}/cinder/android/hardware/Camera.cpp
    ${CINDER_SRC_DIR}/cinder/android/app/CinderNativeActivity.cpp
    ${CINDER_SRC_DIR}/cinder/android/app/ComponentManager.cpp
    ${CINDER_SRC_DIR}/cinder/android/net/UrlLoader.cpp
    ${CINDER_SRC_DIR}/cinder/android/video/MovieGl.cpp
    ${CINDER_SRC_DIR}/cinder/android/video/VideoPlayer.cpp

    ${CINDER_SRC_DIR}/cinder/app/android/AppAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/AppImplAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/AssetFileSystem.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/EventManagerAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/PlatformAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/Renderer2dAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/RendererGlAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/WindowImplAndroid.cpp

    ${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
    #${CINDER_SRC_DIR}/cinder/app/AppScreenSaver.cpp
    ${CINDER_SRC_DIR}/cinder/app/KeyEvent.cpp
    ${CINDER_SRC_DIR}/cinder/app/Platform.cpp
    ${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
    ${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
    ${CINDER_SRC_DIR}/cinder/app/Window.cpp

    ${CINDER_SRC_DIR}/cinder/audio/Node.cpp
    ${CINDER_SRC_DIR}/cinder/audio/ChannelRouterNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/FilterNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/NodeMath.cpp
    ${CINDER_SRC_DIR}/cinder/audio/SampleRecorderNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/WaveTable.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Context.cpp
    ${CINDER_SRC_DIR}/cinder/audio/GenNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/OutputNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Source.cpp
    ${CINDER_SRC_DIR}/cinder/audio/DelayNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/InputNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/PanNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Target.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Device.cpp
    ${CINDER_SRC_DIR}/cinder/audio/MonitorNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Param.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Utilities.cpp
    ${CINDER_SRC_DIR}/cinder/audio/FileOggVorbis.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Node.cpp
    ${CINDER_SRC_DIR}/cinder/audio/SamplePlayerNode.cpp
    ${CINDER_SRC_DIR}/cinder/audio/Voice.cpp
    ${CINDER_SRC_DIR}/cinder/audio/android/ContextOpenSl.cpp
    ${CINDER_SRC_DIR}/cinder/audio/android/DeviceManagerOpenSl.cpp
    ${CINDER_SRC_DIR}/cinder/audio/dsp/Biquad.cpp
    ${CINDER_SRC_DIR}/cinder/audio/dsp/Converter.cpp
    ${CINDER_SRC_DIR}/cinder/audio/dsp/ConverterR8brain.cpp
    ${CINDER_SRC_DIR}/cinder/audio/dsp/Dsp.cpp
    ${CINDER_SRC_DIR}/cinder/audio/dsp/Fft.cpp
    ${CINDER_SRC_DIR}/cinder/audio/dsp/ooura/fftsg.cpp

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

    ${CINDER_SRC_DIR}/cinder/ip/Blend.cpp
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
    ${CINDER_SRC_DIR}/cinder/Capture.cpp
    ${CINDER_SRC_DIR}/cinder/CaptureImplJni.cpp
    ${CINDER_SRC_DIR}/cinder/Channel.cpp
    ${CINDER_SRC_DIR}/cinder/CinderAssert.cpp
    ${CINDER_SRC_DIR}/cinder/CinderMath.cpp
    ${CINDER_SRC_DIR}/cinder/Color.cpp
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
    ${CINDER_SRC_DIR}/cinder/ImageFileTinyExr.cpp
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
    ${CINDER_SRC_DIR}/cinder/UrlImplJni.cpp
    ${CINDER_SRC_DIR}/cinder/Utilities.cpp
    ${CINDER_SRC_DIR}/cinder/Xml.cpp

    ${CINDER_SRC_DIR}/jsoncpp/jsoncpp.cpp

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

    ${CINDER_SRC_DIR}/tinyexr/tinyexr.cc

	${CINDER_SRC_DIR}/r8brain/r8bbase.cpp

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

    ${CINDER_SRC_DIR}/../blocks/MotionManager/src/cinder/MotionManager.cpp
    ${CINDER_SRC_DIR}/../blocks/MotionManager/src/cinder/MotionImplAndroid.cpp
)

set( CMAKE_DEBUG_POSTFIX "_d" )
if( NDK_GLES2 )
    add_library( cinder-es2 STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )
else()
    add_library( cinder STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )
endif()
	endif()