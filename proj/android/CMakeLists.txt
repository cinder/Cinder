cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

# Suppress compiler checks. Why? CMake seems to be inconsistent
# with how it checks compilers on different platforms. 
set( CMAKE_C_COMPILER_WORKS   1 )
set( CMAKE_CXX_COMPILER_WORKS 1 )

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset    "${Esc}[m"     )
  set(ColorBold     "${Esc}[1m"    )
  set(Red           "${Esc}[31m"   )
  set(Green         "${Esc}[32m"   )
  set(Yellow        "${Esc}[33m"   )
  set(Blue          "${Esc}[34m"   )
  set(Magenta       "${Esc}[35m"   )
  set(Cyan          "${Esc}[36m"   )
  set(White         "${Esc}[37m"   )
  set(BoldRed       "${Esc}[1;31m" )
  set(BoldGreen     "${Esc}[1;32m" )
  set(BoldYellow    "${Esc}[1;33m" )
  set(BoldBlue      "${Esc}[1;34m" )
  set(BoldMagenta   "${Esc}[1;35m" )
  set(BoldCyan      "${Esc}[1;36m" )
  set(BoldWhite     "${Esc}[1;37m" )
endif()

# Default values
set( DEFAULT_NDK_PLATFORM 21    )
set( DEFAULT_NDK_ARCH     "armeabi-v7a" )

# ------------------------------------------------------------------------
# NDK_ROOT
# ------------------------------------------------------------------------
# If NDK_ROOT as not passed in as parameter - check environment variable.
if( NOT NDK_ROOT )
    if( DEFINED ENV{NDK_ROOT} )
        set( NDK_ROOT $ENV{NDK_ROOT} )
    endif()
endif()

if( "" STREQUAL "${CMAKE_BUILD_TYPE}" )
    set( CMAKE_BUILD_TYPE "Debug" CACHE FILEPATH "" FORCE )
endif()
message( "${BoldGreen}CMake Build Type: ${CMAKE_BUILD_TYPE} ${ColorReset}" )

# See if we can find the NDK located next to the Cinder directory
if( NOT NDK_ROOT )
    # Most recent first
    set( _supported_ndks 
        "android-ndk"
        "android-ndk-r10e"
        "android-ndk-r10d"
        "android-ndk-r9d"
        "android-ndk-r9c"
    )

    set( _curDirStr "${CMAKE_CURRENT_SOURCE_DIR}" )
    # If string begins with "/" - remove the first "/"
    string( SUBSTRING "${_curDirStr}" 0 1 _firstChar )
    if( "/" STREQUAL "${_firstChar}" )
        string( LENGTH "${_curDirStr}" strLen )
        string( SUBSTRING "${_curDirStr}" 1 -1 _curDirStr )
    endif()

    # Create list of path components
    string( REPLACE "/" ";" _curDirList "${_curDirStr}" )
    list( LENGTH _curDirList _numToks )
    # Find Cinder directory
    foreach( val RANGE ${_numToks} )
        #math( EXPR _index "${_numToks} - ${val}" )
        #list( GET _curDirList ${_index} item )

        list( LENGTH _curDirList _listLen )

        string( REPLACE ";" "/" _dirStr "${_curDirList}" )
        if( ${_listLen} GREATER 0 )
            set( _dirStr "/${_dirStr}" )
        endif()

        set( incDir "${_dirStr}/include/cinder/app/android" )
        if( IS_DIRECTORY "${incDir}" )
            set( CINDER_DIR "${_dirStr}" ) 
            break()
        endif()

        if( ${_listLen} GREATER 0 )
            math( EXPR _lastIndex "${_listLen} - 1" )
            list( REMOVE_AT _curDirList ${_lastIndex} )
        endif()
    endforeach()

    if( CINDER_DIR )
        foreach( val in ${_supported_ndks} )
            get_filename_component( ndk_dir "${CINDER_DIR}/../${val}" ABSOLUTE )
            if( IS_DIRECTORY "${ndk_dir}" )
                message( "${BoldGreen}Detected NDK, version: ${val}, at ${ndk_dir}${ColorReset}" )
                message( "" )
                set( NDK_ROOT "${ndk_dir}" )            
                break()
            endif() 
        endforeach()
    endif()
endif()

# Bail if we haven't set NDK_ROOT
if( NOT NDK_ROOT )
    set( _suggestedDir ${CMAKE_CURRENT_SOURCE_DIR}/../../android-ndk )
    get_filename_component( _suggestedDir "${_suggestedDir}" ABSOLUTE )
    message( FATAL_ERROR "${BoldRed}NDK_ROOT not set! Set using env var or -DNDK_ROOT=<path to NDK> or install (r10d or later) to ${_suggestedDir} ${ColorReset}" )
endif()
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# NDK_PLATFORM
# ------------------------------------------------------------------------
if( NOT NDK_PLATFORM )
    set( NDK_PLATFORM ${DEFAULT_NDK_PLATFORM} )
endif()

if( "19" STREQUAL "${NDK_PLATFORM}" )
    set( NDK_PLATFORM_OUT "android-19" )
elseif( "21" STREQUAL "${NDK_PLATFORM}" )
    set( NDK_PLATFORM_OUT "android-21" )
else()
    message( FATAL_ERROR "${BoldRed}Unsupported Platform (API Level) ${NDK_PLATFORM}${ColorReset}" )
endif()

if( NDK_GLES31 AND ( ${NDK_PLATFORM} LESS 21 ) )
	message( FATAL_ERROR "${BoldRed}OpenGL ES 3.1 requires Platform 21 or later${ColorReset}" )
endif()

# ------------------------------------------------------------------------
# NDK_ARCH
# ------------------------------------------------------------------------
if( NOT NDK_ARCH )
    set( NDK_ARCH ${DEFAULT_NDK_ARCH} )
endif()

set( _supported_archs "armeabi" "armeabi-v7a" "arm64-v8a" "x86" "x86_64" "mips" "mips64" )
list( FIND _supported_archs "${NDK_ARCH}" _index )
if( ${_index} LESS 0 )
    message( FATAL_ERROR "${BoldRed}NDK_ARCH specifies unsupported architecture: ${NDK_ARCH}${ColorReset}" )
endif()

# -- log to console --
message( "${BoldBlue}NDK_ROOT     : ${NDK_ROOT}${ColorReset}" )
message( "${BoldBlue}NDK_PLATFORM : ${NDK_PLATFORM}${ColorReset}" )
message( "${BoldBlue}NDK_ARCH     : ${NDK_ARCH}${ColorReset}" )
if( NDK_GLES2 )
    message( "${BoldBlue}NDK_GLES2    : TRUE${ColorReset}" )
    message( "${BoldBlue}NDK_GLES3    : FALSE${ColorReset}" )
    message( "${BoldBlue}NDK_GLES31   : FALSE${ColorReset}" )
elseif( NDK_GLES31 )
    message( "${BoldBlue}NDK_GLES2    : FALSE${ColorReset}" )
    message( "${BoldBlue}NDK_GLES3    : FALSE${ColorReset}" )
    message( "${BoldBlue}NDK_GLES31   : TRUE${ColorReset}" )
else()
    message( "${BoldBlue}NDK_GLES2    : FALSE${ColorReset}" )
    message( "${BoldBlue}NDK_GLES3    : TRUE${ColorReset}" )
    message( "${BoldBlue}NDK_GLES31   : FALSE${ColorReset}" )
endif()
message( "" )

# Toolchain, STL (gnustl, libc++)
if( NOT NDK_TOOLCHAIN )
    set( NDK_TOOLCHAIN "gcc49" )
endif()

if( NOT NDK_STL )
	set( NDK_STL "gnustl" )
endif()
# Switch to gcc49 if on arm64-v8a or x86_64
if( "gcc48" STREQUAL "${NDK_TOOLCHAIN}" )
    if(("arm64-v8a" STREQUAL "${NDK_ARCH}") OR ("x86_64" STREQUAL "${NDK_ARCH}") OR ("mips64" STREQUAL "${NDK_ARCH}"))
        set( NDK_TOOLCHAIN "gcc49" )
    endif()
endif()

# Process platform
set( NDK_PLATFORM_DIR "android-${NDK_PLATFORM}" )

# Process arch - find tool chain params
set( NDK_ARCH_DIR "arch-${NDK_ARCH}" )
# >> armeabi <<
if( "armeabi" STREQUAL "${NDK_ARCH}" )
    if( "gcc48" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "arm-linux-androideabi-4.8" )
        set( NDK_TOOLCHAIN_PREFIX "arm-linux-androideabi" )
    elseif( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "arm-linux-androideabi-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "arm-linux-androideabi" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "armeabi" )

# >> armeabi-v7a <<
elseif( "armeabi-v7a" STREQUAL "${NDK_ARCH}" )
    if( "gcc48" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "arm-linux-androideabi-4.8" )
        set( NDK_TOOLCHAIN_PREFIX "arm-linux-androideabi" )
    elseif( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "arm-linux-androideabi-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "arm-linux-androideabi" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "armeabi-v7a" )

# >> arm64-v8a <<
elseif( "arm64-v8a" STREQUAL "${NDK_ARCH}" )
    if( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "aarch64-linux-android-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "aarch64-linux-android" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "arm64-v8a" )

# >> x86 <<
elseif( "x86" STREQUAL "${NDK_ARCH}" )
    if( "gcc48" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "x86-4.8" )
        set( NDK_TOOLCHAIN_PREFIX "i686-linux-android" )
    elseif( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "x86-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "i686-linux-android" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "x86" )

# >> x86_64 <<
elseif( "x86_64" STREQUAL "${NDK_ARCH}" )
    if( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "x86_64-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "x86_64-linux-android" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "x86_64" )

# >> mips <<
elseif( "mips" STREQUAL "${NDK_ARCH}" )
    if( "gcc48" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "mipsel-linux-android-4.8" )
        set( NDK_TOOLCHAIN_PREFIX "mipsel-linux-android" )
    elseif( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "mipsel-linux-android-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "mipsel-linux-android" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "mips" )

# >> mips64 <<
elseif( "mips64" STREQUAL "${NDK_ARCH}" )
    if( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "mips64el-linux-android-4.9" )
        set( NDK_TOOLCHAIN_PREFIX "mips64el-linux-android" )
    elseif( "clang" STREQUAL "${NDK_TOOLCHAIN}" )
        set( NDK_TOOLCHAIN_DIR "llvm-3.6" )
        set( NDK_TOOLCHAIN_PREFIX "" )
        set( NDK_TOOLCHAIN_CLANG TRUE )
    endif()
    # Library out dir
    set( NKD_ARCH_OUT "mips64" )
endif()

if( NOT NDK_TOOLCHAIN_DIR )
    message( FATAL_ERROR "${BoldRed}Toolchain ${NDK_TOOLCHAIN} is not supported on ${NDK_ARCH}${ColorReset}" )
endif()

# STL
if( "gnustl" STREQUAL "${NDK_STL}" )
    if( "gcc48" STREQUAL "${NDK_TOOLCHAIN}" )
        include_directories( 
            ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.8/include 
            ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.8/include/backward
        )
        # Architecture
        if( "armeabi" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include )
        elseif( "armeabi-v7a" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi-v7a/include )
        elseif( "x86" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.8/libs/x86/include )
        elseif( "mips" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.8/libs/mips/include )
        endif()
    elseif( "gcc49" STREQUAL "${NDK_TOOLCHAIN}" )
        include_directories( 
            ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/include 
            ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/include/backward
        )
        # Architecture
        if( "armeabi" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include )
        elseif( "armeabi-v7a" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include )
        elseif( "arm64-v8a" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/arm64-v8a/include )
        elseif( "x86" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/x86/include )
        elseif( "x86_64" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/x86_64/include )
        elseif( "mips" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/mips/include )
        elseif( "mips64" STREQUAL "${NDK_ARCH}" )
            include_directories( ${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/mips64/include )
        endif()
    endif()
elseif( "libc++" STREQUAL "${NDK_STL}" )
    include_directories( 
        ${NDK_ROOT}/sources/cxx-stl/llvm-libc++/libcxx/include
    )
else()
    message( FATAL_ERROR "${BoldRed}Unsupported STL library ${NDK_STL}${ColorReset}" )
endif()

# Toolchain host
if( APPLE )
    set( NDK_TOOLCHAIN_HOST "darwin-x86_64" )
elseif( WINDOWS )
elseif( UNIX )
    set( NDK_TOOLCHAIN_HOST "linux-x86_64" )
endif()

if( NOT NDK_TOOLCHAIN_HOST )
    message( FATAL_ERROR "${BoldRed}Unsupported host platform ${CMAKE_SYSTEM_NAME}{ColorReset}" )
endif()

# Toolchain prefix
set( NKD_TOOLCHAIN_ROOT "${NDK_ROOT}/toolchains/${NDK_TOOLCHAIN_DIR}/prebuilt/${NDK_TOOLCHAIN_HOST}" )
message( "${BoldBlue}NKD_TOOLCHAIN_ROOT : ${NKD_TOOLCHAIN_ROOT}${ColorReset}" )
if( NOT NDK_TOOLCHAIN_CLANG )
    message( "${BoldBlue}NDK_TOOLCHAIN_PREFIX : ${NDK_TOOLCHAIN_PREFIX}${ColorReset}" )
endif()
message( "" )

# sysroot
#if( NOT NDK_TOOLCHAIN_CLANG)
    if(("armeabi" STREQUAL "${NDK_ARCH}") OR ("armeabi-v7a" STREQUAL "${NDK_ARCH}"))
        set( NDK_SYSROOT "${NDK_ROOT}/platforms/${NDK_PLATFORM_DIR}/arch-arm" )
    elseif( "arm64-v8a" STREQUAL "${NDK_ARCH}" )
        set( NDK_SYSROOT "${NDK_ROOT}/platforms/${NDK_PLATFORM_DIR}/arch-arm64" )
    elseif( "x86" STREQUAL "${NDK_ARCH}" )
        set( NDK_SYSROOT "${NDK_ROOT}/platforms/${NDK_PLATFORM_DIR}/arch-x86" )
    elseif( "x86_64" STREQUAL "${NDK_ARCH}" )
        set( NDK_SYSROOT "${NDK_ROOT}/platforms/${NDK_PLATFORM_DIR}/arch-x86_64" )
    elseif( "mips" STREQUAL "${NDK_ARCH}" )
        set( NDK_SYSROOT "${NDK_ROOT}/platforms/${NDK_PLATFORM_DIR}/arch-mips" )
    elseif( "mips64" STREQUAL "${NDK_ARCH}" )
        set( NDK_SYSROOT "${NDK_ROOT}/platforms/${NDK_PLATFORM_DIR}/arch-mips64" )
    endif()
    #
    if( NOT NDK_SYSROOT )
        message( FATAL_ERROR "${BoldRed}Unable to determine SYSROOT for ${NDK_ARCH} for platform android-${NDK_PLATFORM}${ColorReset}" )
    endif()
    #
    message( "${BoldBlue}NDK_SYSROOT : ${NDK_SYSROOT}${ColorReset}" )
    message( "" )

    set( CMAKE_SYSROOT ${NDK_SYSROOT} CACHE FILEPATH "" FORCE )
#endif()

if( APPLE )
    set( CMAKE_OSX_SYSROOT "" CACHE FILEPATH "" FORCE )
endif()

# Compiler, build tool settings
if( NDK_TOOLCHAIN_CLANG )
    set( CMAKE_C_COMPILER   ${NKD_TOOLCHAIN_ROOT}/bin/clang     CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER ${NKD_TOOLCHAIN_ROOT}/bin/clang++   CACHE FILEPATH "" FORCE )
    set( CMAKE_AR           ${NKD_TOOLCHAIN_ROOT}/bin/llvm-ar   CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       ${NKD_TOOLCHAIN_ROOT}/bin/clang++ 	CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           ${NKD_TOOLCHAIN_ROOT}/bin/          CACHE FILEPATH "" FORCE )
    #set( CMAKE_RANLIB       ${NKD_TOOLCHAIN_ROOT}/bin/llvm-ar   CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       ${NDK_ROOT}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-ranlib   CACHE FILEPATH "" FORCE )
    set( CMAKE_STRIP        ${NKD_TOOLCHAIN_ROOT}/bin/ndk-strip CACHE FILEPATH "" FORCE )
else()
    set( CMAKE_C_COMPILER   ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-gcc    CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-g++    CACHE FILEPATH "" FORCE )
    set( CMAKE_AR           ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-ar     CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-ld     CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-nm     CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-ranlib CACHE FILEPATH "" FORCE )
    set( CMAKE_STRIP        ${NKD_TOOLCHAIN_ROOT}/bin/${NDK_TOOLCHAIN_PREFIX}-strip  CACHE FILEPATH "" FORCE )
endif()

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE )
set( CINDER_INC_DIR ${CINDER_DIR}/include )
set( CINDER_SRC_DIR ${CINDER_DIR}/src )
set( BOOST_INC_DIR ${CINDER_DIR}/boost )


message( "${BoldGreen}CINDER_INC_DIR: ${CINDER_INC_DIR}${ColorReset}" )
message( "${BoldGreen}CINDER_SRC_DIR: ${CINDER_SRC_DIR}${ColorReset}" )
message( "" )
message( "${BoldGreen}LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH}${ColorReset}" )
message( "" )
message( "${BoldGreen}CMAKE_C_COMPILER  : ${CMAKE_C_COMPILER}${ColorReset}" )
message( "${BoldGreen}CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}${ColorReset}" )
message( "${BoldGreen}CMAKE_AR          : ${CMAKE_AR}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_LINKER      : ${CMAKE_LINKER}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_NM          : ${CMAKE_NM}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_RANLIB      : ${CMAKE_RANLIB}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_STRIP       : ${CMAKE_STRIP}${ColorReset}" ) 
message( "" )
message( "${BoldGreen}CMAKE_SYSROOT     : ${CMAKE_SYSROOT}${ColorReset}" ) 
message( "" )

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
    ${BOOST_INC_DIR}
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
		${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/include
		${NDK_ROOT}/sources/cxx-stl/gnu-libstdc++/4.9/libs/${NDK_ARCH}/include
	)
else()
    set( CXX_FLAGS "${ARCH_FLAGS} -fvisibility=default -std=c++11 -DFT2_BUILD_LIBRARY" )
endif()
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
