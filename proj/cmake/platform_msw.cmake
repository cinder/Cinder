cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CINDER_PLATFORM "MSW" )

list( APPEND SRC_SET_MSW
	${CINDER_SRC_DIR}/cinder/CaptureImplDirectShow.cpp
	${CINDER_SRC_DIR}/cinder/msw/CinderMsw.cpp
	${CINDER_SRC_DIR}/cinder/msw/CinderMswGdiPlus.cpp
	${CINDER_SRC_DIR}/cinder/msw/StackWalker.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileWic.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileWic.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplWinInet.cpp
	${CINDER_SRC_DIR}/glload/wgl_load_cpp.cpp
	${CINDER_SRC_DIR}/glload/wgl_load.c
	${CINDER_SRC_DIR}/AntTweakBar/TwDirect3D11.cpp
)

list( APPEND SRC_SET_APP_MSW
	# TODO: should these two files be added to "cinder\\app" group?
	${CINDER_SRC_DIR}/cinder/app/AppScreenSaver.cpp
	#${CINDER_SRC_DIR}/cinder/app/RendererDx.cpp

	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMswBasic.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppImplMswScreenSaver.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/AppMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/PlatformMsw.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImpl2dGdi.cpp
	#${CINDER_SRC_DIR}/cinder/app/msw/RendererImplDx.cpp
	#${CINDER_SRC_DIR}/cinder/app/msw/RendererImplGlAngle.cpp
	${CINDER_SRC_DIR}/cinder/app/msw/RendererImplGlMsw.cpp
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_MSW}
	${SRC_SET_APP_MSW}
)

source_group( "cinder\\msw"       FILES ${SRC_SET_MSW} )
source_group( "cinder\\app\\msw"  FILES ${SRC_SET_APP_MSW} )

list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
    ${CINDER_INC_DIR}/msw/zlib
)

# NOTE: UNICODE and _UNICODE forces generator to use Unicode instead of MultiByte
list( APPEND CINDER_DEFINES "_LIB;UNICODE;_UNICODE;NOMINMAX;_WIN32_WINNT=0x0601;_CRT_SECURE_NO_WARNINGS;_SCL_SECURE_NO_WARNINGS" )

if( MSVC )
	# Override the default /MD with /MT
	foreach( 
		flag_var
		CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO 
		CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO 
	)
		if( ${flag_var} MATCHES "/MD" )
			string( REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}" )
		endif()
	endforeach()
	# Force synchronous PDB writes
	add_compile_options( /FS )
	# Force multiprocess compilation
	add_compile_options( /MP )
    # Static library flags
    set( CINDER_STATIC_LIBS_FLAGS_DEBUG     "/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCPMT" )
    set( CINDER_STATIC_LIBS_FLAGS_RELEASE   "/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCPMT" )
   
    # Platform libraries 
    set( MSW_PLATFORM_LIBS "Ws2_32.lib wldap32.lib shlwapi.lib OpenGL32.lib wmvcore.lib Strmiids.lib Msimg32.lib" )

    set( MSW_SUBFOLDER "${CINDER_PATH}/lib/${CINDER_TARGET_SUBFOLDER}" )
    # Static library debug depends
    set( CINDER_STATIC_LIBS_DEPENDS_DEBUG   "${MSW_PLATFORM_LIBS} ${MSW_SUBFOLDER}/zlib.lib" )
    # Static library release depends
    set( CINDER_STATIC_LIBS_DEPENDS_RELEASE "${MSW_PLATFORM_LIBS} ${MSW_SUBFOLDER}/zlib.lib" )

    # Glob debug boost libs
    file( GLOB MSW_BOOST_LIBS "${MSW_SUBFOLDER}/libboost*-sgd-*.lib" )
    foreach( BOOST_LIB ${MSW_BOOST_LIBS} )
        set( CINDER_STATIC_LIBS_DEPENDS_DEBUG "${CINDER_STATIC_LIBS_DEPENDS_DEBUG} ${BOOST_LIB}" )
    endforeach()

    # Glob release boost libs
    file( GLOB MSW_BOOST_LIBS "${MSW_SUBFOLDER}/libboost*-s-*.lib" )
    foreach( BOOST_LIB ${MSW_BOOST_LIBS} )
        set( CINDER_STATIC_LIBS_DEPENDS_RELEASE "${CINDER_STATIC_LIBS_DEPENDS_RELEASE} ${BOOST_LIB}" )
    endforeach()
endif()
