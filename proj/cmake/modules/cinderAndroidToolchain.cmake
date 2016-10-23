message( STATUS "Configuring Android toolcahin for Cinder" )

# Force some known options
set( CINDER_GL_ES true )

# NDK path
if( NOT CINDER_ANDROID_NDK_DIR )
	# Look at environment variable NDK_DIR
	if( $ENV{NDK_DIR} )
		set( CINDER_ANDROID_NDK_DIR "$ENV{NDK_DIR}" )
	endif()
endif()
# Make sure there's an NDK dir specified
if( NOT CINDER_ANDROID_NDK_DIR )
	message( FATAL_ERROR "NDK directory not found, use -DCINDER_ANDROID_NDK_DIR=<path> or environment variable NDK_DIR to specify" )
endif()
# Make sure the specified NDK dir exists
if( NOT EXISTS "${CINDER_ANDROID_NDK_DIR}" )
	message( FATAL_ERROR "Specified NDK directory does not exist: ${CINDER_ANDROID_NDK_DIR}" )
endif()

# NDK platform - defaults to platform 24
if( NOT CINDER_ANDROID_NDK_PLATFORM )
	set( CINDER_ANDROID_NDK_PLATFORM 24 )
endif()
# Check for minimum platform
if( ${CINDER_ANDROID_NDK_PLATFORM} LESS 24 )
	message( FATAL_ERROR "Cinder requires platform 24 or later" )
endif()

# NDK architecture - defaults to ARMV8
if( NOT CINDER_ANDROID_NDK_ARCH )
	set( CINDER_ANDROID_NDK_ARCH "arm64-v8a" )
endif()
# Validate architecture
string( TOLOWER "${CINDER_ANDROID_NDK_ARCH}" CINDER_ANDROID_NDK_ARCH_LOWER )
if( NOT "${CINDER_ANDROID_NDK_ARCH_LOWER}" MATCHES "^(armeabi|armeabi-v7a|arm64-v8a|x86|x86_64|mips|mips64)$" )
	message( FATAL_ERROR "Unknown NDK architecture: ${CINDER_ANDROID_NDK_ARCH}, supported architectures: armeabi,armeabi-v7a,arm64-v8a,x86,x86_64,mips,mips64" )
endif()

# NDK host
if( APPLE )
    set( CINDER_ANDROID_NDK_HOST "darwin-x86_64" )
elseif( UNIX )
	set( CINDER_ANDROID_NDK_HOST "linux-x86_64" )
elseif( WIN32 )
	set( CINDER_ANDROID_NDK_HOST "windows-x86_64" )
	set( EXE_SUFFIX ".exe" )
else()
	message( FATAL_ERROR "Unkonwn NDK host system" )
endif()

# Print relevant information
message( STATUS "Using NDK at ${CINDER_ANDROID_NDK_DIR}" )
message( STATUS "Using NDK platform ${CINDER_ANDROID_NDK_PLATFORM}" )
message( STATUS "Using NDK architecture ${CINDER_ANDROID_NDK_ARCH_LOWER}" )
message( STATUS "Using NDK host ${CINDER_ANDROID_NDK_HOST}" )

# Setup platform and toolchain dirs
set( CINDER_ANDROID_NDK_PLATFORM_DIR 	"${CINDER_ANDROID_NDK_DIR}/platforms/android-${CINDER_ANDROID_NDK_PLATFORM}" )
set( CINDER_ANDROID_NDK_TOOLCHAIN_DIR 	"${CINDER_ANDROID_NDK_DIR}/toolchains" )
set( CINDER_ANDROID_NDK_COMPILER_DIR	"${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/llvm/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
if( ( "armeabi" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" ) OR ( "armeabi-v7a" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" ) )
	set( CINDER_ANDROID_NDK_BINUTIL_DIR "${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/arm-linux-androideabi-4.9/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
	set( CINDER_ANDROID_NDK_BINUTIL_PREFIX "arm-linux-androideabi" )
elseif( "arm64-v8a" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_BINUTIL_DIR "${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/aarch64-linux-android-4.9/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
	set( CINDER_ANDROID_NDK_BINUTIL_PREFIX "aarch64-linux-android" )
elseif( "x86" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_BINUTIL_DIR "${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/x86-4.9/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
	set( CINDER_ANDROID_NDK_BINUTIL_PREFIX "i686-linux-android" )
elseif( "x86_64" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_BINUTIL_DIR "${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/x86_64-4.9/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
	set( CINDER_ANDROID_NDK_BINUTIL_PREFIX "x86_64-linux-android" )
elseif( "mips" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_BINUTIL_DIR "${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/mipsel-linux-android-4.9/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
	set( CINDER_ANDROID_NDK_BINUTIL_PREFIX "mipsel-linux-android" )
elseif( "mips64" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_BINUTIL_DIR "${CINDER_ANDROID_NDK_TOOLCHAIN_DIR}/mips64el-linux-android-4.9/prebuilt/${CINDER_ANDROID_NDK_HOST}" )
	set( CINDER_ANDROID_NDK_BINUTIL_PREFIX "mips64el-linux-android" )
endif()

# Print tool chain dirs
message( STATUS "Using NDK compilers at ${CINDER_ANDROID_NDK_COMPILER_DIR}" )
message( STATUS "Using NDK binutils at ${CINDER_ANDROID_NDK_BINUTIL_DIR}" )

# Setup toolchain
set( CMAKE_C_COMPILER   ${CINDER_ANDROID_NDK_COMPILER_DIR}/bin/clang${EXE_SUFFIX}                                       CACHE FILEPATH "" FORCE )
set( CMAKE_CXX_COMPILER ${CINDER_ANDROID_NDK_COMPILER_DIR}/bin/clang++${EXE_SUFFIX}                                     CACHE FILEPATH "" FORCE )
set( CMAKE_AR           ${CINDER_ANDROID_NDK_BINUTIL_DIR}/bin/${CINDER_ANDROID_NDK_BINUTIL_PREFIX}-ar${EXE_SUFFIX}      CACHE FILEPATH "" FORCE )
set( CMAKE_LINKER       ${CINDER_ANDROID_NDK_BINUTIL_DIR}/bin/${CINDER_ANDROID_NDK_BINUTIL_PREFIX}-ld${EXE_SUFFIX}      CACHE FILEPATH "" FORCE )
set( CMAKE_NM           ${CINDER_ANDROID_NDK_BINUTIL_DIR}/bin/${CINDER_ANDROID_NDK_BINUTIL_PREFIX}-nm${EXE_SUFFIX}      CACHE FILEPATH "" FORCE )
set( CMAKE_RANLIB       ${CINDER_ANDROID_NDK_BINUTIL_DIR}/bin/${CINDER_ANDROID_NDK_BINUTIL_PREFIX}-ranlib${EXE_SUFFIX}  CACHE FILEPATH "" FORCE )
set( CMAKE_STRIP        ${CINDER_ANDROID_NDK_BINUTIL_DIR}/bin/${CINDER_ANDROID_NDK_BINUTIL_PREFIX}-strip${EXE_SUFFIX}   CACHE FILEPATH "" FORCE )

# Suppress compiler checks for Android since building happens on a host system
set( CMAKE_C_COMPILER_WORKS   1 )
set( CMAKE_CXX_COMPILER_WORKS 1 )

# Configure system root
if( ( "armeabi" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" ) OR ( "armeabi-v7a" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" ) )
	set( CINDER_ANDROID_NDK_SYSROOT "${CINDER_ANDROID_NDK_PLATFORM_DIR}/arch-arm" )
elseif( "arm64-v8a" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_SYSROOT "${CINDER_ANDROID_NDK_PLATFORM_DIR}/arch-arm64" )
elseif( "x86" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_SYSROOT "${CINDER_ANDROID_NDK_PLATFORM_DIR}/arch-x86" )
elseif( "x86_64" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_SYSROOT "${CINDER_ANDROID_NDK_PLATFORM_DIR}/arch-x86_64" )
elseif( "mips" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_SYSROOT "${CINDER_ANDROID_NDK_PLATFORM_DIR}/arch-mips" )
elseif( "mips64" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( CINDER_ANDROID_NDK_SYSROOT "${CINDER_ANDROID_NDK_PLATFORM_DIR}/arch-mips64" )
endif()
set( CMAKE_SYSROOT ${CINDER_ANDROID_NDK_SYSROOT} CACHE FILEPATH "" FORCE )

# Print system root dir
message( STATUS "Using NDK system root at ${CMAKE_SYSROOT}" )

# Suppress this on macOS host
if( APPLE )
    set( CMAKE_OSX_SYSROOT "" CACHE FILEPATH "" FORCE )
endif()

# STL include directories 
include_directories( 
	"${CINDER_ANDROID_NDK_DIR}/sources/cxx-stl/gnu-libstdc++/4.9/include" 
	"${CINDER_ANDROID_NDK_DIR}/sources/cxx-stl/gnu-libstdc++/4.9/libs/${CINDER_ANDROID_NDK_ARCH_LOWER}/include" 
)

# Architecture flags
if( "armeabi" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( ARCH_FLAGS "-target armv5te-none-linux-androideabi -march=armv5te -mtune=xscale -msoft-float -fpic -fstack-protector-strong" )
elseif( "armeabi-v7a" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	# Force hard float, Vulkan requires the use the hard float calling convention or the build will break.
	set( ARCH_FLAGS "-target armv7-none-linux-androideabi -march=armv7-a -mfpu=neon -mhard-float -D_NDK_MATH_NO_SOFTFP=1 -fstack-protector -fpic" )
elseif( "arm64-v8a" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( ARCH_FLAGS "-target aarch64-none-linux-android -fstack-protector-strong -fpic" )
elseif( "x86" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( ARCH_FLAGS "-target i686-none-linux-android -fstack-protector-strong -fPIC" )
elseif( "x86_64" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( ARCH_FLAGS "-target x86_64-none-linux-android -fstack-protector-strong -fPIC" )
elseif( "mips" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( ARCH_FLAGS "-target mipsel-none-linux-android -fpic -fno-strict-aliasing -finline-functions -fmessage-length=0" )
elseif( "mips64" STREQUAL "${CINDER_ANDROID_NDK_ARCH_LOWER}" )
	set( ARCH_FLAGS "-target mips64el-none-linux-android -fpic -fno-strict-aliasing -finline-functions -fmessage-length=0" )
endif()

# C_FLAGS
set( C_FLAGS "-DANDROID ${ARCH_FLAGS} -fvisibility=default" )
# CMAKE_FLAGS
set( CMAKE_C_FLAGS_DEBUG   	"-DCINDER_DEBUG ${C_FLAGS} -fPIC -g" CACHE STRING "" FORCE )
set( CMAKE_C_FLAGS_RELEASE 	"${C_FLAGS} -fPIC -Os -ffast-math ${C_FLAGS}" CACHE STRING "" FORCE )

# CXX_FLAGS
set( CXX_FLAGS "-gcc-toolchain ${CINDER_ANDROID_NDK_BINUTIL_DIR} ${ARCH_FLAGS} -fvisibility=default -stdlib=libstdc++ -D_GLIBCXX_HAS_GTHREADS -ffunction-sections -funwind-tables -Wno-invalid-command-line-argument -Wno-unused-command-line-argument -no-canonical-prefixes -fomit-frame-pointer -DANDROID" )
# CMAKE_CXX_FLAGS
set( CMAKE_CXX_FLAGS_DEBUG   "-DCINDER_DEBUG ${CXX_FLAGS} -fPIC -g -rdynamic -fexceptions -frtti" CACHE STRING "" FORCE )
set( CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS} -fPIC -Os -rdynamic -fexceptions -frtti -ffast-math" CACHE STRING "" FORCE )

