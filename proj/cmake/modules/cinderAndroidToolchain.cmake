message( STATUS "Configuring Android toolcahin for Cinder" )

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
elseif( UNIX )
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

# Setup toolchain dirs
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
