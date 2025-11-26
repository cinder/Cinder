cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

set( CINDER_PLATFORM "MSW" )

list( APPEND SRC_SET_MSW
	${CINDER_SRC_DIR}/cinder/CaptureImplDirectShow.cpp
	${CINDER_SRC_DIR}/cinder/msw/CinderMsw.cpp
	${CINDER_SRC_DIR}/cinder/msw/CinderMswGdiPlus.cpp
	${CINDER_SRC_DIR}/cinder/msw/StackWalker.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileWic.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileWic.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplWinInet.cpp
	${CINDER_SRC_DIR}/glad/glad_wgl.c
)

list( APPEND SRC_SET_APP_MSW
	# TODO: should these two files be added to "cinder\\app" group?
	${CINDER_SRC_DIR}/cinder/app/AppScreenSaver.cpp
	#${CINDER_SRC_DIR}/cinder/app/RendererDx.cpp
	# D3D11/D3D12 renderers not included in default build - add to your project if needed
	#${CINDER_SRC_DIR}/cinder/app/RendererD3d11.cpp
	#${CINDER_SRC_DIR}/cinder/app/RendererD3d12.cpp

	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMswBasic.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMswScreenSaver.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/PlatformMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImpl2dGdi.cpp
	#${CINDER_SRC_DIR}/cinder/app/msw/RendererImplDx.cpp

	# D3D11 renderer
	${CINDER_SRC_DIR}/cinder/app/RendererD3d11.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImplD3d11.cpp

	# D3D12 renderer
	${CINDER_SRC_DIR}/cinder/app/RendererD3d12.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImplD3d12.cpp
)

# ANGLE or native OpenGL renderer
if( CINDER_GL_ANGLE )
	list( APPEND SRC_SET_APP_MSW ${CINDER_SRC_DIR}/cinder/app/msw/RendererImplGlAngle.cpp )
else()
	list( APPEND SRC_SET_APP_MSW ${CINDER_SRC_DIR}/cinder/app/msw/RendererImplGlMsw.cpp )
endif()

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_AUDIO_MSW
		${CINDER_SRC_DIR}/cinder/audio/msw/ContextWasapi.cpp
		${CINDER_SRC_DIR}/cinder/audio/msw/DeviceManagerWasapi.cpp
		${CINDER_SRC_DIR}/cinder/audio/msw/FileMediaFoundation.cpp
		${CINDER_SRC_DIR}/cinder/audio/msw/MswUtil.cpp
	)

	list( APPEND SRC_SET_CINDER_AUDIO_DSP
		${CINDER_SRC_DIR}/cinder/audio/dsp/ooura/fftsg.cpp
		${CINDER_SRC_DIR}/cinder/audio/dsp/ConverterR8brain.cpp
	)
endif()

if( NOT CINDER_DISABLE_VIDEO )
	# Option for using GStreamer under MSW.
	option( CINDER_MSW_USE_GSTREAMER "Use GStreamer for video playback." OFF )

	if( CINDER_MSW_USE_GSTREAMER )
		set( GST_ROOT $ENV{GSTREAMER_1_0_ROOT_X86_64} )
		if( GST_ROOT )
			list( APPEND CINDER_LIBS_DEPENDS
						${GST_ROOT}/lib/gstreamer-1.0.lib
						${GST_ROOT}/lib/gstapp-1.0.lib
						${GST_ROOT}/lib/gstvideo-1.0.lib
						${GST_ROOT}/lib/gstbase-1.0.lib
						${GST_ROOT}/lib/gstnet-1.0.lib
						${GST_ROOT}/lib/gstaudio-1.0.lib
						${GST_ROOT}/lib/gstgl-1.0.lib
						${GST_ROOT}/lib/gobject-2.0.lib
						${GST_ROOT}/lib/gmodule-2.0.lib
						${GST_ROOT}/lib/gthread-2.0.lib
						${GST_ROOT}/lib/glib-2.0.lib
						${GST_ROOT}/lib/gio-2.0.lib )

			list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
						${GST_ROOT}/include
						${GST_ROOT}/include/gstreamer-1.0
						${GST_ROOT}/include/glib-2.0
						${GST_ROOT}/lib/gstreamer-1.0/include
						${GST_ROOT}/lib/glib-2.0/include
						${CINDER_INC_DIR}/cinder/linux )

			list( APPEND CINDER_SRC_FILES
						${CINDER_SRC_DIR}/cinder/linux/GstPlayer.cpp
						${CINDER_SRC_DIR}/cinder/linux/Movie.cpp )

			list( APPEND CINDER_DEFINES CINDER_MSW_USE_GSTREAMER )
		else()
			message( WARNING "Requested GStreamer video playback support for MSW but no suitable GStreamer installation found. Make sure that GStreamer is installed properly and GSTREAMER_1_0_ROOT_X86_64 is defined in your env variables. " )
		endif()
	else()
		# Default: Use MediaFoundation for video playback on Windows
		list( APPEND SRC_SET_VIDEO_MSW
			${CINDER_SRC_DIR}/cinder/qtime/QuickTimeImplMsw.cpp
			${CINDER_SRC_DIR}/cinder/qtime/mf/MediaEnginePlayer.cpp
			${CINDER_SRC_DIR}/cinder/qtime/mf/WICRenderPath.cpp
		)

		# MovieGl and DXGIRenderPath use WGL_NV_DX_interop which requires native OpenGL
		if( NOT CINDER_GL_ANGLE )
			list( APPEND SRC_SET_VIDEO_MSW
				${CINDER_SRC_DIR}/cinder/qtime/QuickTimeGlImplMsw.cpp
				${CINDER_SRC_DIR}/cinder/qtime/mf/DXGIRenderPath.cpp
			)
		endif()

		list( APPEND CINDER_SRC_FILES ${SRC_SET_VIDEO_MSW} )
	endif()
endif()

list( APPEND CINDER_SRC_FILES
	${SRC_SET_MSW}
	${SRC_SET_APP_MSW}
	${SRC_SET_AUDIO_MSW}
	${SRC_SET_CINDER_AUDIO_DSP}
)

source_group( "cinder\\msw"			FILES ${SRC_SET_MSW} )
source_group( "cinder\\app\\msw"	FILES ${SRC_SET_APP_MSW} )
source_group( "cinder\\audio\\msw"	FILES ${SRC_SET_AUDIO_MSW} )
source_group( "cinder\\audio\\dsp"	FILES ${SRC_SET_CINDER_AUDIO_DSP} )
source_group( "cinder\\video\\msw"	FILES ${SRC_SET_VIDEO_MSW} )

list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
	${CINDER_INC_DIR}/msw/png
	${CINDER_INC_DIR}/msw/zlib
	${CINDER_INC_DIR}/msw
)

# ANGLE includes
if( CINDER_GL_ANGLE )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${CINDER_INC_DIR}/ANGLE )
endif()

# NOTE: UNICODE and _UNICODE forces generator to use Unicode instead of MultiByte
list( APPEND CINDER_DEFINES "_LIB;UNICODE;_UNICODE;NOMINMAX;_WIN32_WINNT=0x0A00;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS" )

# ANGLE define
if( CINDER_GL_ANGLE )
	list( APPEND CINDER_DEFINES "CINDER_GL_ANGLE" )
endif()

if( MSVC )
	# Set runtime library - DLL builds use dynamic CRT (/MD), static builds use static CRT (/MT)
	if( NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY )
		if( BUILD_SHARED_LIBS )
			set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" )
		else()
			set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )
		endif()
	endif()

	# Force synchronous PDB writes
	add_compile_options( /FS )
	# Force multiprocess compilation
	add_compile_options( /MP )
	# Static library flags
	set( CINDER_STATIC_LIBS_FLAGS_DEBUG		"/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCPMT" )

	# Platform libraries - base set without GL
	set( MSW_PLATFORM_LIBS_BASE "Ws2_32.lib wldap32.lib shlwapi.lib wmvcore.lib Strmiids.lib Msimg32.lib d3d11.lib d3d12.lib dxgi.lib d3dcompiler.lib" )

	# GL libraries - either ANGLE or native OpenGL
	if( CINDER_GL_ANGLE )
		set( MSW_GL_LIBS "libEGL.lib libGLESv2.lib" )
		# ANGLE libraries are in lib/msw/<arch>
		link_directories( "${CINDER_PATH}/lib/msw/${CINDER_ARCH}" )
	else()
		set( MSW_GL_LIBS "OpenGL32.lib" )
	endif()

	set( MSW_PLATFORM_LIBS "${MSW_PLATFORM_LIBS_BASE} ${MSW_GL_LIBS}" )

	set( MSW_SUBFOLDER "${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}" )
	# Static library debug depends
	set( CINDER_STATIC_LIBS_DEPENDS_DEBUG	"${MSW_PLATFORM_LIBS}" )
	# Static library release depends
	set( CINDER_STATIC_LIBS_DEPENDS_RELEASE "${MSW_PLATFORM_LIBS}" )
endif()
