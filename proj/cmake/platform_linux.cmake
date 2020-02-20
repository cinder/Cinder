cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( CMAKE_VERBOSE_MAKEFILE ON )

set( CINDER_PLATFORM "Posix" )

# When CINDER_HEADLESS is set, ${SRC_SET_GLFW} will *not* be compiled.
list( APPEND SRC_SET_GLFW
	${CINDER_SRC_DIR}/glfw/src/context.c
	${CINDER_SRC_DIR}/glfw/src/init.c
	${CINDER_SRC_DIR}/glfw/src/input.c
	${CINDER_SRC_DIR}/glfw/src/monitor.c
	${CINDER_SRC_DIR}/glfw/src/window.c
	${CINDER_SRC_DIR}/glfw/src/glx_context.c
	${CINDER_SRC_DIR}/glfw/src/egl_context.c
	${CINDER_SRC_DIR}/glfw/src/x11_init.c
	${CINDER_SRC_DIR}/glfw/src/x11_monitor.c
	${CINDER_SRC_DIR}/glfw/src/x11_window.c
	${CINDER_SRC_DIR}/glfw/src/xkb_unicode.c
	${CINDER_SRC_DIR}/glfw/src/linux_joystick.c
	${CINDER_SRC_DIR}/glfw/src/posix_time.c
	${CINDER_SRC_DIR}/glfw/src/posix_tls.c
	${CINDER_SRC_DIR}/glfw/src/vulkan.c
)

list( APPEND SRC_SET_CINDER_APP_LINUX
	${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
)

if( NOT CINDER_DISABLE_AUDIO )
	list( APPEND SRC_SET_CINDER_AUDIO_LINUX
	#	${CINDER_SRC_DIR}/cinder/audio/linux/ContextJack.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/ContextPulseAudio.cpp
	#	${CINDER_SRC_DIR}/cinder/audio/linux/DeviceManagerJack.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/DeviceManagerPulseAudio.cpp
		${CINDER_SRC_DIR}/cinder/audio/linux/FileAudioLoader.cpp
	)

	list( APPEND SRC_SET_CINDER_AUDIO_DSP
		${CINDER_SRC_DIR}/cinder/audio/dsp/ooura/fftsg.cpp
		${CINDER_SRC_DIR}/cinder/audio/dsp/ConverterR8brain.cpp
	)
endif()

if( NOT CINDER_DISABLE_VIDEO )
	list( APPEND SRC_SET_CINDER_VIDEO_LINUX
		${CINDER_SRC_DIR}/cinder/linux/GstPlayer.cpp
		${CINDER_SRC_DIR}/cinder/linux/Movie.cpp
	)
endif()

# Curl
list( APPEND SRC_SET_CINDER_LINUX ${CINDER_SRC_DIR}/cinder/UrlImplCurl.cpp )

# Relevant source files depending on target GL and if we running headless.
if( NOT CINDER_HEADLESS ) # Desktop ogl, es2, es3, RPi
	if( CINDER_GL_ES )
		list( APPEND SRC_SET_CINDER_LINUX
			${CINDER_SRC_DIR}/glad/glad_es.c
		)
	else()
		list( APPEND SRC_SET_CINDER_LINUX
			${CINDER_SRC_DIR}/glad/glad_glx.c
		)
	endif()

	list( APPEND SRC_SET_CINDER_LINUX
		${SRC_SET_GLFW}
	)
	list( APPEND SRC_SET_CINDER_APP_LINUX
		${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxGlfw.cpp
		${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxGlfw.cpp
		${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxGlfw.cpp
	)
else() # Headless egl, osmesa
	list( APPEND SRC_SET_CINDER_LINUX
		${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxHeadless.cpp
		${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxHeadless.cpp
		${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxHeadless.cpp
	)
	if( CINDER_GL_ES )
		list( APPEND SRC_SET_CINDER_LINUX
			${CINDER_SRC_DIR}/glad/glad_es.c
		)
	endif()
endif()

list( APPEND CINDER_SRC_FILES
	${SRC_SET_CINDER_LINUX}
	${SRC_SET_CINDER_APP_LINUX}
	${SRC_SET_CINDER_AUDIO_LINUX}
	${SRC_SET_CINDER_AUDIO_DSP}
	${SRC_SET_CINDER_VIDEO_LINUX}
)

# Relevant libs and include dirs depending on target platform and target GL.
if( CINDER_GL_CORE )
	if( NOT CINDER_HEADLESS_GL_OSMESA )
		find_package( OpenGL REQUIRED )
		list( APPEND CINDER_LIBS_DEPENDS ${OPENGL_LIBRARIES} )
		list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${OPENGL_INCLUDE_DIR} )
		find_package( X11 REQUIRED )
		list( APPEND CINDER_LIBS_DEPENDS ${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
		list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${X11_INCLUDE_DIR} )
		if( CINDER_HEADLESS_GL_EGL ) # Headless through EGL
			list( APPEND CINDER_LIBS_DEPENDS EGL )
		endif()
	else()
		find_package( X11 REQUIRED )
		list( APPEND CINDER_LIBS_DEPENDS ${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
		list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${X11_INCLUDE_DIR} )
		find_package( OSMesa REQUIRED )
		list( APPEND CINDER_LIBS_DEPENDS ${OSMESA_LIBRARIES} ${OSMESA_GL_LIBRARIES} )
		list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${OSMESA_INCLUDE_DIR} )
	endif()
elseif( CINDER_GL_ES )
	find_package( X11 REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${X11_INCLUDE_DIR} )
	list( APPEND CINDER_LIBS_DEPENDS EGL GLESv2 )
endif()

# Common libs for Linux.
# ZLib
find_package( ZLIB REQUIRED )
list( APPEND CINDER_LIBS_DEPENDS ${ZLIB_LIBRARIES} )
list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${ZLIB_INCLUDE_DIR} )
# Curl
find_package( CURL REQUIRED )
list( APPEND CINDER_LIBS_DEPENDS ${CURL_LIBRARIES} )
list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${CURL_INCLUDE_DIR} )
# FontConfig
find_package( FontConfig REQUIRED )
list( APPEND CINDER_LIBS_DEPENDS ${FONTCONFIG_LIBRARIES} )
list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${FONTGONFIG_INCLUDE_DIRS} )
if( NOT CINDER_DISABLE_AUDIO )
	# PulseAudio
	find_package( PulseAudio REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${PULSEAUDIO_LIBRARY} )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${PULSEAUDIO_INCLUDE_DIR} )
	# mpg123
	find_package( MPG123 REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${MPG123_LIBRARY} )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${MPG123_INCLUDE_DIR} )
	# sndfile
	find_package( SNDFILE REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${SNDFILE_LIBRARY} )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${SNDFILE_INCLUDE_DIR} )
endif()
# GStreamer and its dependencies.
if( NOT CINDER_DISABLE_VIDEO )
	# Glib
	find_package( Glib REQUIRED COMPONENTS gobject )
	list( APPEND CINDER_LIBS_DEPENDS ${GLIB_GOBJECT_LIBRARIES} ${GLIB_LIBRARIES} )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${GLIB_INCLUDE_DIRS} )
	# GStreamer
	find_package( GStreamer REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS
		${GSTREAMER_LIBRARIES}
		${GSTREAMER_BASE_LIBRARIES}
		${GSTREAMER_APP_LIBRARIES}
		${GSTREAMER_VIDEO_LIBRARIES}
	)
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
		${GSTREAMER_INCLUDE_DIRS}
		${GSTREAMER_BASE_INCLUDE_DIRS}
		${GSTREAMER_APP_INCLUDE_DIRS}
		${GSTREAMER_VIDEO_INCLUDE_DIRS}
	)
	# If we have gst-gl available add it.
	if( GSTREAMER_GL_INCLUDE_DIRS AND GSTREAMER_GL_LIBRARIES )
		list( APPEND CINDER_LIBS_DEPENDS ${GSTREAMER_GL_LIBRARIES} )
		list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${GSTREAMER_GL_INCLUDE_DIRS} )
	endif()
endif()

# Defaults... dl and pthread
list( APPEND CINDER_LIBS_DEPENDS dl pthread )

# Workaround for gcc bug on versions > 5.3.1 when building as a shared lib.
if( CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.3.1 AND BUILD_SHARED_LIBS )
	list( APPEND CINDER_LIBS_DEPENDS gcc )
endif()

source_group( "cinder\\linux"			FILES ${SRC_SET_CINDER_LINUX} )
source_group( "cinder\\app\\linux"		FILES ${SRC_SET_CINDER_APP_LINUX} )

list( APPEND CINDER_INCLUDE_USER_PRIVATE
	${CINDER_INC_DIR}/glfw
)

# Cinder GL defines depending on target GL.
if( CINDER_GL_CORE )
	list( APPEND CINDER_DEFINES "-DCINDER_GL_CORE" )
elseif( CINDER_GL_ES )
	list( APPEND CINDER_DEFINES "-DCINDER_GL_ES" )
	
	if( CINDER_GL_ES_2 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_2" )
	elseif( CINDER_GL_ES_3 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3" )
	elseif( CINDER_GL_ES_3_1 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3_1" )
	elseif( CINDER_GL_ES_3_2 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3_2" )
	elseif( CINDER_GL_ES_3_RPI )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3" "-DCINDER_GL_ES_3_RPI" )
	endif()
endif()

# Set appropriate defines when running headless.
if( CINDER_HEADLESS )
	if( CINDER_HEADLESS_GL_EGL )
		list( APPEND CINDER_DEFINES "-DCINDER_LINUX_EGL_ONLY -DCINDER_HEADLESS -DCINDER_HEADLESS_GL_EGL" )
	elseif( CINDER_HEADLESS_GL_OSMESA )
		list( APPEND CINDER_DEFINES "-DCINDER_HEADLESS -DCINDER_HEADLESS_GL_OSMESA" )
	endif()
else() # If not headless we need X.
	list( APPEND GLFW_FLAGS "-D_GLFW_X11" )
endif()

list( APPEND CINDER_DEFINES "-D_UNIX" ${GLFW_FLAGS}  )
