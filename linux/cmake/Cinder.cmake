
include(${CMAKE_CURRENT_LIST_DIR}/DebugColours.cmake)

execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )
execute_process( COMMAND which clang OUTPUT_VARIABLE CLANG_PATH )

if( NOT CMAKE_CXX_COMPILER OR NOT CMAKE_C_COMPILER )
    if( NOT "${CLANG_PATH}" STREQUAL "" )
        set( CINDER_TOOLCHAIN_CLANG true )
    endif()

    if( NOT CINDER_TOOLCHAIN_GCC AND CINDER_TOOLCHAIN_CLANG )
		set( CMAKE_C_COMPILER						"clang" 		CACHE FILEPATH "" FORCE )
		set( CMAKE_CXX_COMPILER						"clang++" 		CACHE FILEPATH "" FORCE )
		set( CMAKE_AR          						"llvm-ar"		CACHE FILEPATH "" FORCE )
		set( CMAKE_LINKER       					"llvm-link" 	CACHE FILEPATH "" FORCE )
		set( CMAKE_NM           					"llvm-nm " 		CACHE FILEPATH "" FORCE )
		set( CMAKE_RANLIB       					"llvm-ranlib"	CACHE FILEPATH "" FORCE )

		set( CMAKE_C_FLAGS_INIT						"-Wall -std=c99" 	CACHE STRING "" FORCE )
		set( CMAKE_C_FLAGS_DEBUG_INIT				"-g" 				CACHE STRING "" FORCE )
		set( CMAKE_C_FLAGS_MINSIZEREL_INIT			"-Os -DNDEBUG" 		CACHE STRING "" FORCE )
		set( CMAKE_C_FLAGS_RELEASE_INIT				"-O4 -DNDEBUG" 		CACHE STRING "" FORCE )
		set( CMAKE_C_FLAGS_RELWITHDEBINFO_INIT		"-O2 -g" 			CACHE STRING "" FORCE )
		set( CMAKE_C_FLAGS							"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

		set( CMAKE_CXX_FLAGS_INIT					"-Wall" 		CACHE STRING "" FORCE )
		set( CMAKE_CXX_FLAGS_DEBUG_INIT				"-g" 			CACHE STRING "" FORCE )
		set( CMAKE_CXX_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" 	CACHE STRING "" FORCE ) 
		set( CMAKE_CXX_FLAGS_RELEASE_INIT			"-O4 -DNDEBUG" 	CACHE STRING "" FORCE )
		set( CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" 		CACHE STRING "" FORCE )
		set( CMAKE_CCC_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

        set(STDCXXLIB                          		"-stdlib=libstdc++" )
    else()
		# Keep these versionless
    	set( CMAKE_C_COMPILER						"gcc"	CACHE FILEPATH "" FORCE )
	    set( CMAKE_CXX_COMPILER						"g++" 	CACHE FILEPATH "" FORCE )
		set( CINDER_TOOLCHAIN_GCC 					true 	CACHE BOOL "" FORCE )
		set( CINDER_TOOLCHAIN_CLANG 				false 	CACHE BOOL "" FORCE )
    endif()
endif()

# C++ flags - TODO: Add logic for the case when GCC5's new C++ ABI is desired.
set( CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 ${STDCXXLIB} -std=c++11 -Wno-reorder -Wno-unused-private-field -Wno-unused-local-typedef" )

if( CINDER_TOOLCHAIN_CLANG )
	# Disable these warnings, many of which are coming from Boost - append at end
	set( CXX_DISABLE_WARNINGS "-Wno-reorder -Wno-unused-function -Wno-unused-private-field -Wno-unused-local-typedef -Wno-tautological-compare -Wno-missing-braces" )
elseif( CINDER_TOOLCHAIN_GCC )
	execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
	if( ( "${GCC_VERSION}" VERSION_GREATER 5.0 ) OR ( "${GCC_VERSION}" VERSION_EQUAL 5.0 ) )
		# Disable these warnings, many of which are coming from Boost - append at end
		set( CXX_DISABLE_WARNINGS "-Wno-deprecated-declarations" )
	endif()
endif()

# C++ flags
set( CMAKE_CXX_FLAGS_DEBUG    "${CXX_FLAGS} -g -fexceptions -frtti" 				CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -Os -fexceptions -frtti -ffast-math" 	CACHE STRING "" FORCE )


set( CINDER_LIB_SUFFIX )
set( CINDER_OUT_DIR_PREFIX )

if( CINDER_GL_ES_2 )
	set( CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG}   -DCINDER_GL_ES_2" )
	set( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DCINDER_GL_ES_2" )
    set( CINDER_LIB_SUFFIX -es2 )
    if( CINDER_LINUX_EGL_RPI2 )
        set( CINDER_OUT_DIR_PREFIX es2-rpi2 )
        add_definitions( -DCINDER_LINUX_EGL_ONLY )
    else()
        set( CINDER_OUT_DIR_PREFIX es2 )
    endif()
elseif( CINDER_GL_ES_3 )
	set( CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG}   -DCINDER_GL_ES_3" )
	set( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DCINDER_GL_ES_3" )
    set( CINDER_LIB_SUFFIX -es3 )
    set( CINDER_OUT_DIR_PREFIX es3 )
elseif( CINDER_GL_ES_3_1 )
	set( CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG}   -DCINDER_GL_ES_3_1" )
	set( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DCINDER_GL_ES_3_1" )
    set( CINDER_LIB_SUFFIX -es31 )
    set( CINDER_OUT_DIR_PREFIX es31 )
elseif( CINDER_GL_ES_3_2 )
	set( CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_CXX_FLAGS_DEBUG}   -DCINDER_GL_ES_3_2" )
	set( CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -DCINDER_GL_ES_3_2" )
    set( CINDER_LIB_SUFFIX -es32 )
    set( CINDER_OUT_DIR_PREFIX es32 )
else()
    set( CINDER_LIB_SUFFIX )
    set( CINDER_OUT_DIR_PREFIX ogl )
endif()

