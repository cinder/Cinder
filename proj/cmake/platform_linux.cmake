cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CMAKE_VERBOSE_MAKEFILE ON )

set( CINDER_PLATFORM "Posix" )

list( APPEND SRC_SET_GLFW 
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

list( APPEND SRC_SET_CINDER_APP_LINUX 
	${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
)

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

list( APPEND SRC_SET_CINDER_VIDEO_LINUX
    ${CINDER_SRC_DIR}/cinder/linux/GstPlayer.cpp
    ${CINDER_SRC_DIR}/cinder/linux/Movie.cpp
)

# Relevant source files depending on target GL.
if( NOT CINDER_GL_ES_2_RPI )
	if( CINDER_GL_ES )
		list( APPEND SRC_SET_GLFW 
			${CINDER_SRC_DIR}/glfw/src/egl_context.c
		)
		list( APPEND SRC_SET_CINDER_LINUX
			${CINDER_SRC_DIR}/cinder/linux/gl_es_load.cpp
		)
	else()
		list( APPEND SRC_SET_GLFW 
			${CINDER_SRC_DIR}/glfw/src/glx_context.c
		)
		list( APPEND SRC_SET_CINDER_LINUX
			${CINDER_SRC_DIR}/glload/glx_load.c
			${CINDER_SRC_DIR}/glload/glx_load_cpp.cpp
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
else()
	list( APPEND SRC_SET_CINDER_LINUX
		${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxEgl.cpp
		${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxEgl.cpp
		${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxEgl.cpp
		${CINDER_SRC_DIR}/cinder/linux/gl_es_load.cpp
	)
endif()

list( APPEND CINDER_SRC_FILES
	${SRC_SET_CINDER_LINUX}
	${SRC_SET_CINDER_APP_LINUX}
	${SRC_SET_CINDER_AUDIO_LINUX}
	${SRC_SET_CINDER_AUDIO_DSP}
	${SRC_SET_CINDER_VIDEO_LINUX}
)

# Relevant libs and include dirs depending on target platform and target GL.
if( NOT CINDER_GL_ES ) # desktop
	find_package( OpenGL REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${OPENGL_LIBRARIES} )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${OPENGL_INCLUDE_DIR} )
	find_package( X11 REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${X11_INCLUDE_DIR} )
elseif( CINDER_GL_ES AND NOT CINDER_GL_ES_2_RPI ) # No X for the rpi.
	find_package( X11 REQUIRED )
	list( APPEND CINDER_LIBS_DEPENDS ${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${X11_INCLUDE_DIR} )
	list( APPEND CINDER_LIBS_DEPENDS EGL GLESv2 )
else() # rpi specific
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
		/opt/vc/include
		/opt/vc/include/interface/vmcs_host/linux/
		/opt/vc/include/interface/vcos/pthreads
	)
	list( APPEND CINDER_LIBS_DEPENDS 
		/opt/vc/lib/libEGL.so
		/opt/vc/lib/libGLESv2.so
		/opt/vc/lib/libbcm_host.so
	)
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
# GStreamer and its dependencies.
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

# Boost
if( CINDER_BOOST_USE_SYSTEM )
	find_package( Boost 1.54 REQUIRED COMPONENTS system filesystem )
	list( APPEND CINDER_LIBS_DEPENDS ${Boost_LIBRARIES} )
	list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE ${Boost_INCLUDE_DIRS} )
else()
	set( LINUX_LIB_DIRECTORY "${CINDER_PATH}/lib/linux/${CINDER_ARCH}/" )
	list( APPEND CINDER_LIBS_DEPENDS
		${LINUX_LIB_DIRECTORY}/libboost_system.a
		${LINUX_LIB_DIRECTORY}/libboost_filesystem.a
	)
endif()

# Defaults... dl and pthread
list( APPEND CINDER_LIBS_DEPENDS dl pthread )

source_group( "cinder\\linux"           FILES ${SRC_SET_CINDER_LINUX} )
source_group( "cinder\\app\\linux"      FILES ${SRC_SET_CINDER_APP_LINUX} )

list( APPEND CINDER_INCLUDE_USER_PRIVATE
	${CINDER_INC_DIR}/glfw
)

# Cinder GL defines depending on target GL.
if( CINDER_GL_ES AND NOT CINDER_GL_ES_2_RPI ) # es2, es3, es31, es32
	list( APPEND GLFW_FLAGS "-D_GLFW_X11 -D_GLFW_EGL -D_GLFW_USE_GLESV2" )
	if( CINDER_GL_ES_2 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_2" )
	elseif( CINDER_GL_ES_3 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3" )
	elseif( CINDER_GL_ES_3_1 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3_1" )
	elseif( CINDER_GL_ES_3_2 )
		list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_3_2" )
	endif()
elseif( NOT CINDER_GL_ES ) # Core Profile
	list( APPEND GLFW_FLAGS "-D_GLFW_X11 -D_GLFW_GLX -D_GLFW_USE_OPENGL" )
else() # Rpi
	list( APPEND CINDER_DEFINES "-DCINDER_GL_ES_2" "-DCINDER_LINUX_EGL_ONLY" )
endif()

list( APPEND CINDER_DEFINES "-D_UNIX" ${GLFW_FLAGS}  )

if( NOT CINDER_BOOST_USE_SYSTEM )
	list( APPEND CINDER_DEFINES "-D_GLIBCXX_USE_CXX11_ABI=0" )
endif()
