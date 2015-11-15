if( NOT CINDER_LINUX_EGL_RPI2 )
	list( APPEND CINDER_C_SRC_FILES
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
	)
	
	if( CINDER_GL_ES )
		list( APPEND CINDER_C_SRC_FILES
			${CINDER_SRC_DIR}/glfw/src/egl_context.c
		)
	else()
		list( APPEND CINDER_C_SRC_FILES
			${CINDER_SRC_DIR}/glfw/src/glx_context.c
			${CINDER_SRC_DIR}/glload/gl_load.c
			${CINDER_SRC_DIR}/glload/glx_load.c
		)
	endif()
endif()

list( APPEND CINDER_C_SRC_FILES
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

if( CINDER_LINUX_EGL_RPI2 )
	list( APPEND CINDER_CXX_SRC_FILES
   		${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxEgl.cpp
    	${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxEgl.cpp
    	${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxEgl.cpp
	)
else()
	if( NOT CINDER_GL_ES )
		list( APPEND CINDER_CXX_SRC_FILES
			${CINDER_SRC_DIR}/glload/gl_load_cpp.cpp
			${CINDER_SRC_DIR}/glload/glx_load_cpp.cpp
		)
	endif()

	list( APPEND CINDER_CXX_SRC_FILES
   		${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxGlfw.cpp
    	${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxGlfw.cpp
    	${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxGlfw.cpp
	)
endif()

if( CINDER_GL_ES )
	list( APPEND CINDER_CXX_SRC_FILES
   		${CINDER_SRC_DIR}/cinder/linux/gl_es_load.cpp
	)
endif()

list( APPEND CINDER_CXX_SRC_FILES
    ${CINDER_SRC_DIR}/cinder/linux/CinderLinux.cpp

    ${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
    ${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp

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
    ${CINDER_SRC_DIR}/cinder/UrlImplCurl.cpp
    ${CINDER_SRC_DIR}/cinder/Utilities.cpp
    ${CINDER_SRC_DIR}/cinder/Xml.cpp

    ${CINDER_SRC_DIR}/jsoncpp/jsoncpp.cpp
)

# Audio - keep this in its own blcok for now
#if( NOT CINDER_GL_ES )
	list( APPEND CINDER_C_SRC_FILES
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

	list( APPEND CINDER_CXX_SRC_FILES
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

		${CINDER_SRC_DIR}/cinder/audio/linux/ContextJack.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/DeviceManagerJack.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/ContextPulseAudio.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/DeviceManagerPulseAudio.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/FileAudioLoader.cpp

		${CINDER_SRC_DIR}/r8brain/r8bbase.cpp
	)
#endif()

if( NOT CINDER_GL_ES )
	list( APPEND CINDER_CXX_SRC_FILES
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
endif()
