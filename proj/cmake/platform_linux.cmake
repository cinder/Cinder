cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

set( CINDER_PLATFORM "Posix" )

# Find architecture name.
execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )

set( CINDER_TARGET_SUBFOLDER "linux/${CINDER_ARCH}" )

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

list( APPEND SRC_SET_CINDER_LINUX
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
	${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxGlfw.cpp
	${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxGlfw.cpp
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_CINDER_LINUX}
	${SRC_SET_CINDER_APP_LINUX}
)

source_group( "cinder\\linux"           FILES ${SRC_SET_CINDER_LINUX} )
source_group( "cinder\\app\\linux"      FILES ${SRC_SET_CINDER_APP_LINUX} )

list( APPEND CINDER_INCLUDE_USER
	${CINDER_INC_DIR}/glfw
)

list( APPEND GLFW_FLAGS "-D_GLFW_X11 -D_GLFW_GLX -D_GLFW_USE_OPENGL" )
list( APPEND CINDER_DEFINES "-D_UNIX ${GLFW_FLAGS}"  )

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )
