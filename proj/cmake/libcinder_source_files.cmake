# This file is used to specify sources that become part of the libcinder binary, organized according to namespace or folders.

# ----------------------------------------------------------------------------------------------------------------------
# cinder
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_CINDER
	${CINDER_SRC_DIR}/cinder/Area.cpp
	${CINDER_SRC_DIR}/cinder/Area.cpp
	${CINDER_SRC_DIR}/cinder/BandedMatrix.cpp
	${CINDER_SRC_DIR}/cinder/Base64.cpp
	${CINDER_SRC_DIR}/cinder/BSpline.cpp
	${CINDER_SRC_DIR}/cinder/BSplineFit.cpp
	${CINDER_SRC_DIR}/cinder/Buffer.cpp
	${CINDER_SRC_DIR}/cinder/Camera.cpp
	${CINDER_SRC_DIR}/cinder/CameraUi.cpp
	${CINDER_SRC_DIR}/cinder/Channel.cpp
	${CINDER_SRC_DIR}/cinder/CinderAssert.cpp
	${CINDER_SRC_DIR}/cinder/CinderMath.cpp
	${CINDER_SRC_DIR}/cinder/Clipboard.cpp
	${CINDER_SRC_DIR}/cinder/Color.cpp
	${CINDER_SRC_DIR}/cinder/DataSource.cpp
	${CINDER_SRC_DIR}/cinder/DataTarget.cpp
	${CINDER_SRC_DIR}/cinder/Display.cpp
	${CINDER_SRC_DIR}/cinder/Exception.cpp
	${CINDER_SRC_DIR}/cinder/Filesystem.cpp
	${CINDER_SRC_DIR}/cinder/FileWatcher.cpp
	${CINDER_SRC_DIR}/cinder/Font.cpp
	${CINDER_SRC_DIR}/cinder/Frustum.cpp
	${CINDER_SRC_DIR}/cinder/GeomIo.cpp
	${CINDER_SRC_DIR}/cinder/ImageFileTinyExr.cpp
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
)

if( ( NOT CINDER_LINUX ) AND ( NOT CINDER_ANDROID ) )
	list( APPEND SRC_SET_CINDER
		${CINDER_SRC_DIR}/cinder/Capture.cpp
	)
endif()
if( ( NOT CINDER_COCOA_TOUCH ) AND ( NOT CINDER_ANDROID ) )
	list( APPEND SRC_SET_CINDER
		${CINDER_SRC_DIR}/cinder/Serial.cpp
	)
endif()

list( APPEND CINDER_SRC_FILES   ${SRC_SET_CINDER} )
source_group( "cinder" FILES    ${SRC_SET_CINDER} )

# ----------------------------------------------------------------------------------------------------------------------
# cinder::app
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_CINDER_APP
	${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
	${CINDER_SRC_DIR}/cinder/app/KeyEvent.cpp
	${CINDER_SRC_DIR}/cinder/app/Platform.cpp
	${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
	${CINDER_SRC_DIR}/cinder/app/Window.cpp
)

list( APPEND CINDER_SRC_FILES       ${SRC_SET_CINDER_APP} )
source_group( "cinder\\app" FILES   ${SRC_SET_CINDER_APP} )

# ----------------------------------------------------------------------------------------------------------------------
# cinder::audio
# ----------------------------------------------------------------------------------------------------------------------

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_CINDER_AUDIO
		${CINDER_SRC_DIR}/cinder/audio/ChannelRouterNode.cpp
		${CINDER_SRC_DIR}/cinder/audio/Context.cpp
		${CINDER_SRC_DIR}/cinder/audio/DelayNode.cpp
		${CINDER_SRC_DIR}/cinder/audio/Device.cpp
		${CINDER_SRC_DIR}/cinder/audio/FileOggVorbis.cpp
		${CINDER_SRC_DIR}/cinder/audio/FilterNode.cpp
		${CINDER_SRC_DIR}/cinder/audio/GenNode.cpp
		${CINDER_SRC_DIR}/cinder/audio/InputNode.cpp
		${CINDER_SRC_DIR}/cinder/audio/Node.cpp
		${CINDER_SRC_DIR}/cinder/audio/NodeMath.cpp
		${CINDER_SRC_DIR}/cinder/audio/MonitorNode.cpp
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
	)

	list( APPEND SRC_SET_CINDER_AUDIO_DSP
		${CINDER_SRC_DIR}/cinder/audio/dsp/Biquad.cpp
		${CINDER_SRC_DIR}/cinder/audio/dsp/Converter.cpp
		${CINDER_SRC_DIR}/cinder/audio/dsp/Dsp.cpp
		${CINDER_SRC_DIR}/cinder/audio/dsp/Fft.cpp
	)

	list( APPEND CINDER_SRC_FILES           ${SRC_SET_CINDER_AUDIO} )
	source_group( "cinder\\audio" FILES     ${SRC_SET_CINDER_AUDIO} )

	list( APPEND CINDER_SRC_FILES           	${SRC_SET_CINDER_AUDIO_DSP} )
	source_group( "cinder\\audio\\dsp" FILES    ${SRC_SET_CINDER_AUDIO_DSP} )
endif()

# ----------------------------------------------------------------------------------------------------------------------
# cinder::gl
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_CINDER_GL
	${CINDER_SRC_DIR}/cinder/gl/Batch.cpp
	${CINDER_SRC_DIR}/cinder/gl/BufferObj.cpp
	${CINDER_SRC_DIR}/cinder/gl/BufferTexture.cpp
	${CINDER_SRC_DIR}/cinder/gl/ConstantConversions.cpp
	${CINDER_SRC_DIR}/cinder/gl/Context.cpp
	${CINDER_SRC_DIR}/cinder/gl/draw.cpp
	${CINDER_SRC_DIR}/cinder/gl/Environment.cpp
	${CINDER_SRC_DIR}/cinder/gl/EnvironmentCore.cpp
	${CINDER_SRC_DIR}/cinder/gl/EnvironmentEs.cpp
	${CINDER_SRC_DIR}/cinder/gl/Fbo.cpp
	${CINDER_SRC_DIR}/cinder/gl/GlslProg.cpp
	${CINDER_SRC_DIR}/cinder/gl/Pbo.cpp
	${CINDER_SRC_DIR}/cinder/gl/Query.cpp
	${CINDER_SRC_DIR}/cinder/gl/scoped.cpp
	${CINDER_SRC_DIR}/cinder/gl/Sampler.cpp
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
	${CINDER_SRC_DIR}/cinder/gl/wrapper.cpp
	${CINDER_SRC_DIR}/cinder/gl/nv/Multicast.cpp
)

list( APPEND CINDER_SRC_FILES       ${SRC_SET_CINDER_GL} )
source_group( "cinder\\gl" FILES    ${SRC_SET_CINDER_GL} )

# ----------------------------------------------------------------------------------------------------------------------
# cinder::ip
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_CINDER_IP
	${CINDER_SRC_DIR}/cinder/ip/Blend.cpp
	${CINDER_SRC_DIR}/cinder/ip/Blur.cpp
	${CINDER_SRC_DIR}/cinder/ip/Checkerboard.cpp
	${CINDER_SRC_DIR}/cinder/ip/Fill.cpp
	${CINDER_SRC_DIR}/cinder/ip/Grayscale.cpp
	${CINDER_SRC_DIR}/cinder/ip/Premultiply.cpp
	${CINDER_SRC_DIR}/cinder/ip/Threshold.cpp
	${CINDER_SRC_DIR}/cinder/ip/EdgeDetect.cpp
	${CINDER_SRC_DIR}/cinder/ip/Flip.cpp
	${CINDER_SRC_DIR}/cinder/ip/Hdr.cpp
	${CINDER_SRC_DIR}/cinder/ip/Resize.cpp
	${CINDER_SRC_DIR}/cinder/ip/Trim.cpp
)

list( APPEND CINDER_SRC_FILES       ${SRC_SET_CINDER_IP} )
source_group( "cinder\\ip" FILES    ${SRC_SET_CINDER_IP} )

# ----------------------------------------------------------------------------------------------------------------------
# cinder::svg
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_CINDER_SVG
	${CINDER_SRC_DIR}/cinder/svg/Svg.cpp
)

list( APPEND CINDER_SRC_FILES       ${SRC_SET_CINDER_SVG} )
source_group( "cinder\\svg" FILES   ${SRC_SET_CINDER_SVG} )

# ----------------------------------------------------------------------------------------------------------------------
# tinyexr
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_TINYEXR
	${CINDER_SRC_DIR}/tinyexr/tinyexr.cc
)

list( APPEND CINDER_SRC_FILES               ${SRC_SET_TINYEXR} )
source_group( "thirdparty\\tinyexr" FILES   ${SRC_SET_TINYEXR} )

# ----------------------------------------------------------------------------------------------------------------------
# glad
# ----------------------------------------------------------------------------------------------------------------------

if( NOT CINDER_GL_ES )

list( APPEND SRC_SET_GLAD
	${CINDER_SRC_DIR}/glad/glad.c
)

list( APPEND CINDER_SRC_FILES               ${SRC_SET_GLAD} )
source_group( "thirdparty\\glad" FILES      ${SRC_SET_GLAD} )

endif()

# ----------------------------------------------------------------------------------------------------------------------
# jsoncpp
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_JSONCPP
	${CINDER_SRC_DIR}/jsoncpp/jsoncpp.cpp
)

list( APPEND CINDER_SRC_FILES               ${SRC_SET_JSONCPP} )
source_group( "thirdparty\\jsoncpp" FILES   ${SRC_SET_JSONCPP} )

# ----------------------------------------------------------------------------------------------------------------------
# linebreak
# ----------------------------------------------------------------------------------------------------------------------

list( APPEND SRC_SET_LINEBREAK
	${CINDER_SRC_DIR}/linebreak/linebreak.c
	${CINDER_SRC_DIR}/linebreak/linebreakdata.c
	${CINDER_SRC_DIR}/linebreak/linebreakdef.c
)

list( APPEND CINDER_SRC_FILES                 ${SRC_SET_LINEBREAK} )
source_group( "thirdparty\\linebreak" FILES   ${SRC_SET_LINEBREAK} )

# ----------------------------------------------------------------------------------------------------------------------
# libtess
# ----------------------------------------------------------------------------------------------------------------------

# libtess
list( APPEND SRC_SET_LIBTESS
	${CINDER_SRC_DIR}/libtess2/bucketalloc.c
	${CINDER_SRC_DIR}/libtess2/dict.c
	${CINDER_SRC_DIR}/libtess2/geom.c
	${CINDER_SRC_DIR}/libtess2/mesh.c
	${CINDER_SRC_DIR}/libtess2/priorityq.c
	${CINDER_SRC_DIR}/libtess2/sweep.c
	${CINDER_SRC_DIR}/libtess2/tess.c
)

list( APPEND CINDER_SRC_FILES               ${SRC_SET_LIBTESS} )
source_group( "thirdparty\\libtess" FILES   ${SRC_SET_LIBTESS} )

# ----------------------------------------------------------------------------------------------------------------------
# libpng
# ----------------------------------------------------------------------------------------------------------------------

if( PNG_FOUND )
	list( APPEND SRC_SET_TINYEXR
		${CINDER_SRC_DIR}/cinder/ImageSourcePng.cpp
	)

	list( APPEND CINDER_SRC_FILES ${SRC_SET_TINYEXR} )
	source_group( "cinder" FILES ${SRC_SET_CINDER_APP} )
endif()

# ----------------------------------------------------------------------------------------------------------------------
# cinder::params + AntTweakBar
# ----------------------------------------------------------------------------------------------------------------------

if( CINDER_ANTTWEAKBAR_ENABLED )

	list( APPEND SRC_SET_CINDER_PARAMS
		${CINDER_SRC_DIR}/cinder/params/Params.cpp
	)

	list( APPEND SRC_SET_ANTTWEAKBAR
		${CINDER_SRC_DIR}/AntTweakBar/TwColors.cpp
		${CINDER_SRC_DIR}/AntTweakBar/TwFonts.cpp
		${CINDER_SRC_DIR}/AntTweakBar/LoadOGL.cpp
		${CINDER_SRC_DIR}/AntTweakBar/LoadOGLCore.cpp
		${CINDER_SRC_DIR}/AntTweakBar/TwBar.cpp
		${CINDER_SRC_DIR}/AntTweakBar/TwMgr.cpp
		${CINDER_SRC_DIR}/AntTweakBar/TwOpenGL.cpp
		${CINDER_SRC_DIR}/AntTweakBar/TwOpenGLCore.cpp
		${CINDER_SRC_DIR}/AntTweakBar/TwPrecomp.cpp
	)

	list( APPEND CINDER_SRC_FILES
		${SRC_SET_CINDER_PARAMS}
		${SRC_SET_ANTTWEAKBAR}
	)

	source_group( "cinder\\params"	            FILES ${SRC_SET_CINDER_PARAMS} )
	source_group( "thirdparty\\AntTweakBar"	    FILES ${SRC_SET_ANTTWEAKBAR} )

endif()

if( CINDER_IMGUI_ENABLED )
	list( APPEND SRC_SET_CINDER_IMGUI ${CINDER_SRC_DIR}/cinder/CinderImGui.cpp )
	list( APPEND SRC_SET_IMGUI
		${CINDER_SRC_DIR}/imgui/imgui.cpp
		${CINDER_SRC_DIR}/imgui/imgui_demo.cpp
		${CINDER_SRC_DIR}/imgui/imgui_draw.cpp
		${CINDER_SRC_DIR}/imgui/imgui_freetype.cpp
		${CINDER_SRC_DIR}/imgui/imgui_impl_opengl3.cpp
		${CINDER_SRC_DIR}/imgui/imgui_stdlib.cpp
		${CINDER_SRC_DIR}/imgui/imgui_widgets.cpp
	)

	list( APPEND CINDER_SRC_FILES
		${SRC_SET_CINDER_IMGUI}
		${SRC_SET_IMGUI}
	)
	source_group( "cinder"					FILES ${SRC_SET_CINDER_IMGUI} )
	source_group( "thirdparty\\imgui"	    FILES ${SRC_SET_IMGUI} )
endif()

# ----------------------------------------------------------------------------------------------------------------------
# FreeType
# ----------------------------------------------------------------------------------------------------------------------

if( NOT CINDER_FREETYPE_USE_SYSTEM )
	list( APPEND SRC_SET_FREETYPE
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

	list( APPEND CINDER_SRC_FILES               ${SRC_SET_FREETYPE}	)
	source_group( "thirdparty\\freetype" FILES  ${SRC_SET_FREETYPE} )

endif() # ! CINDER_FREETYPE_USE_SYSTEM

list( APPEND SRC_SET_ZLIB
	${CINDER_SRC_DIR}/zlib/adler32.c
	${CINDER_SRC_DIR}/zlib/compress.c
	${CINDER_SRC_DIR}/zlib/crc32.c
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
	${CINDER_SRC_DIR}/zlib/zutil.c
)

list( APPEND CINDER_SRC_FILES           ${SRC_SET_ZLIB}	)
source_group( "thirdparty\\zlib" FILES  ${SRC_SET_ZLIB} )

# ----------------------------------------------------------------------------------------------------------------------
# r8brain
# ----------------------------------------------------------------------------------------------------------------------

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_R8BRAIN
		${CINDER_SRC_DIR}/r8brain/r8bbase.cpp
	)

	list( APPEND CINDER_SRC_FILES               ${SRC_SET_R8BRAIN}	)
	source_group( "thirdparty\\r8brain" FILES   ${SRC_SET_R8BRAIN} )
endif()

# ----------------------------------------------------------------------------------------------------------------------
# oggvorbis
# ----------------------------------------------------------------------------------------------------------------------

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_OGGVORBIS
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
	)

	list( APPEND CINDER_SRC_FILES                   ${SRC_SET_OGGVORBIS} )
	source_group( "thirdparty\\oggvorbis" FILES     ${SRC_SET_OGGVORBIS} )
endif()
