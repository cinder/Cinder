#!/bin/bash

lower_case=$(echo "$1" | tr '[:upper:]' '[:lower:]')

if [ -z $1 ]; then 
	echo Need to provide platform. Possible platforms are linux, macosx, ios. Exiting!
	exit 
fi 

WITH_PANGO=false
PANGO_DIR=""
if [ -z $2 ]; then
  echo Building without Glib functionality.
  else
    if [ -z "${GLIB_LIBS:?false}" ]; then
      echo "Chose with-pango but GLIB flags not present. Use Cinder-Pango to get Glib in the correct place. Exiting!"
      exit
    fi
    export glib_CFLAGS=$GLIB_CFLAGS
    export glib_LIBS=$GLIB_LIBS
  if [ -z $3  ]; then
    echo "Error: Need Pango lib location. Exiting!"
    exit
  fi
  PANGO_DIR=${3}
  if [ ! -d ${PANGO_DIR} ]; then
    echo "Can't find pango at...${PANGO_DIR}. Exiting!"
    exit
  fi
  echo "Configured to run with Pango"
  WITH_PANGO=true
fi



#########################
## create prefix dirs
#########################

PREFIX_BASE_DiR=`pwd`/tmp

PREFIX_LIBZ=${PREFIX_BASE_DiR}/libz_install
rm -rf $PREFIX_LIBZ
mkdir -p $PREFIX_LIBZ

PREFIX_LIBPNG=${PREFIX_BASE_DiR}/libpng_install
rm -rf $PREFIX_LIBPNG
mkdir -p $PREFIX_LIBPNG 

PREFIX_PIXMAN=${PREFIX_BASE_DiR}/pixman_install
rm -rf $PREFIX_PIXMAN 
mkdir -p $PREFIX_PIXMAN 

PREFIX_CAIRO=${PREFIX_BASE_DiR}/cairo_install
rm -rf $PREFIX_CAIRO
mkdir -p $PREFIX_CAIRO

#########################
## create final path
#########################

FINAL_PATH=`pwd`/..
if [ $WITH_PANGO = true ]; then
  FINAL_PATH=${PANGO_DIR}
fi

FINAL_LIB_PATH=${FINAL_PATH}/lib/${lower_case}
if [ $WITH_PANGO = false ]; then
  rm -rf ${FINAL_LIB_PATH}
  mkdir -p ${FINAL_LIB_PATH}
fi

FINAL_INCLUDE_PATH=${FINAL_PATH}/include/${lower_case}
if [ $WITH_PANGO = false ]; then
  rm -rf ${FINAL_INCLUDE_PATH}
  mkdir -p ${FINAL_INCLUDE_PATH}
fi

export PATH="${PREFIX_LIBPNG}/bin:${PREFIX_PIXMAN}/bin:$PATH" 

########################
## building ios
#########################

buildIos() 
{
  buildLibPng $HOST
  buildPixman $HOST

	OPTIONS="--enable-quartz=yes --enable-quartz-font=yes --enable-quartz-image=yes --enable-fc=no --without-x --disable-xlib --disable-xlib-xrender --disable-xcb --disable-xlib-xcb --disable-xcb-shm --enable-ft --disable-full-testing" 

	buildCairo "${OPTIONS}" "${HOST}"
}

#########################
## building osx
#########################

buildOSX() 
{
	echo Setting up OSX environment...
	
  # On osx, i want to make sure the zlib version, we use
if [ $WITH_PANGO = false ]; then
  downloadZlib
	buildZlib
fi
	buildLibPng
	buildPixman 
	
	OPTIONS="--enable-quartz=yes --enable-quartz-surface=yes --enable-quartz-image=yes --without-x --disable-xlib --disable-xlib-xrender --disable-xcb --disable-xlib-xcb --disable-xcb-shm --enable-ft --disable-full-testing" 
  
	buildCairo "${OPTIONS}" 
}

#########################
## building linux
#########################

buildLinux() 
{
	echo Setting up Linux environment...
	
	buildLibPng
	buildPixman 
	
	OPTIONS="--with-x --enable-xlib=yes --enable-xlib-xrenderer=yes --enable-fc=yes --enable-ft=yes --disable-full-testing" 
	buildCairo "${OPTIONS}" 
}

#########################
## downloading libs
#########################

downloadZlib()
{
	echo Downloading zlib...
	curl http://zlib.net/zlib-1.2.8.tar.gz -o zlib.tar.gz > /dev/null
	tar -xf zlib.tar.gz
	mv zlib-* zlib
	rm zlib.tar.gz
	echo Finished Downloading zlib...
}

downloadLibPng() 
{
	echo Downloading libpng...
	curl ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/libpng-1.6.26.tar.gz -o libpng.tar.gz > /dev/null
	tar -xf libpng.tar.gz 
	mv libpng-* libpng
	rm libpng.tar.gz 
	echo Finished downloading libpng...
}

downloadLibPixman() 
{
	echo Downloading pixman...
	curl https://www.cairographics.org/releases/pixman-0.34.0.tar.gz -o pixman.tar.gz > /dev/null
	tar -xf pixman.tar.gz
	mv pixman-* pixman 
	rm pixman.tar.gz 
	echo Finished downloading pixman...
}

downloadLibCairo() 
{
	echo Downloading cairo...
	curl https://www.cairographics.org/releases/cairo-1.14.6.tar.xz -o cairo.tar.xz > /dev/null
	tar -xf cairo.tar.xz 
	mv cairo-* cairo 
	rm cairo.tar.xz 
  if [ "${lower_case}" = "ios" ]; then
    echo In the harfbuzz change...
    # Many of these edits are based on this patch which hasn't been merged in...
    # https://bugs.freedesktop.org/show_bug.cgi?id=97895 
    export LC_CTYPE=C 
    export LANG=C
    sed -i -e 's=ApplicationServices/ApplicationServices.h=CoreGraphics/CoreGraphics.h=' cairo/configure
    sed -i -e 's=-framework ApplicationServices=-framework CoreGraphics=' cairo/configure
    sed -i -e 's=#include <ApplicationServices/ApplicationServices.h>=#include <CoreGraphics/CoreGraphics.h>\
#include <CoreText/CoreText.h>=' cairo/src/cairo-quartz.h
    gtac cairo/src/cairo-quartz.h | sed '/cairo_quartz_font_face_create_for_atsu_font_id (ATSUFontID font_id);/{N;d;N;d;N;d;}' | gtac > cairo/src/temp.h; mv cairo/src/temp.h cairo/src/cairo-quartz.h
    sed -i -e 's|static ATSFontRef (\*FMGetATSFontRefFromFontPtr) (FMFont iFont) = NULL;||' cairo/src/cairo-quartz-font.c
    sed -i -e 's|FMGetATSFontRefFromFontPtr = dlsym(RTLD_DEFAULT, "FMGetATSFontRefFromFont");||' cairo/src/cairo-quartz-font.c
    # This is dirty and inoptimal but couldn't figure out another way to do it.
    for i in $(seq 1 46); do sed -i -e '$d' cairo/src/cairo-quartz-font.c; done;
    sed -i -e 's|#include <Carbon/Carbon.h>||' cairo/src/cairo-quartz-image.h
  fi
  echo Finished downloading cairo...
}

#########################
## building libs
#########################

buildZlib()
{
 	cd zlib
	echo "Building zlib, and installing $1"
	
	./configure --prefix=${PREFIX_LIBZ}

	make -j 6
	make install
	make clean

	cd ..	
}

buildLibPng()
{
	cd libpng
	echo "Building libpng, and installing $1"
 	PREFIX=$PREFIX_LIBPNG
	HOST=$1
	echo "Passed in $HOST"
	if [ -z "$HOST" ]; then 
		./configure --disable-shared --prefix=${PREFIX}
	else	
		echo Building with cross-compile
		./configure --host=${HOST} --disable-shared --prefix=${PREFIX} 
	fi

  make -j 6
	make install
	make clean
 	
	cp -r ${PREFIX}/include/* ${FINAL_INCLUDE_PATH}
	cp ${PREFIX}/lib/*.a ${FINAL_LIB_PATH}

	cd ..	
}

buildPixman()
{
	cd pixman
	echo "Building pixman, and installing $1"
	PREFIX=$PREFIX_PIXMAN
	HOST=$1
 	if [ -z "$HOST" ]; then 
		./configure --disable-shared --prefix=${PREFIX}
	else	
		./configure --host=${HOST} --disable-shared --prefix=${PREFIX} 
	fi

  sed -i -e 's/region_test_OBJECTS = region-test.$(OBJEXT)/region_test_OBJECTS = region-test.$(OBJEXT) utils.$(OBJEXT)/g' test/Makefile
  sed -i -e 's/scaling_helpers_test_OBJECTS = scaling-helpers-test.$(OBJEXT)/scaling_helpers_test_OBJECTS = scaling-helpers-test.$(OBJEXT) utils.$(OBJEXT)/g' test/Makefile

  make -j 6
	make install
	make clean
 	
	cp -r ${PREFIX}/include/* ${FINAL_INCLUDE_PATH}
	cp ${PREFIX}/lib/*.a ${FINAL_LIB_PATH}

	cd ..	
}

buildCairo()
{
	cd cairo

	echo "Building cairo, and installing ${PREFIX_CAIRO}, with options ${1}"
 	PREFIX=$PREFIX_CAIRO
	OPTIONS=$1
	HOST=$2
	if [ -z "$HOST" ]; then 
		./configure --disable-shared --enable-static --prefix=${PREFIX} ${OPTIONS}
	else	
		./configure --host=${HOST} --disable-shared --enable-static --prefix=${PREFIX} $OPTIONS
	fi
  
	make -j 6
	make install
	make clean
 		
	cp -r ${PREFIX}/include/* ${FINAL_INCLUDE_PATH}
	cp ${PREFIX}/lib/libcairo.a ${FINAL_LIB_PATH}

	cd ..	
}

#########################
## echo the flags
#########################

echoFlags()
{
	echo "==================================================================="
	echo "Environment for ${lower_case}..."
	echo -e "\t CXX:      ${CXX}"
	echo -e "\t CC:       ${CC}"
	echo -e "\t CFLAGS:   ${CFLAGS}"
	echo -e "\t CXXFLAGS: ${CXXFLAGS}"
	echo -e "\t LDFLAGS:  ${LDFLAGS}"
	echo "==================================================================="
}

#########################
## program
#########################

# Create working directory
rm -rf tmp 
mkdir tmp 
cd tmp

downloadLibPng
downloadLibPixman
downloadLibCairo

## Set up flags used by the builds
export PNG_CFLAGS="-I${FINAL_INCLUDE_PATH}" 
export PNG_LIBS="-L${FINAL_LIB_PATH} -lpng -lz" 
export png_LIBS="-L${FINAL_LIB_PATH} -lpng"
export png_CFLAGS="-I${FINAL_INCLUDE_PATH}" 
export pixman_CFLAGS="-I${FINAL_INCLUDE_PATH}/pixman-1"
export pixman_LIBS="-L${FINAL_LIB_PATH} -lpixman-1"
export PKG_CONFIG_PATH="${PREFIX_LIBPNG}/lib/pkgconfig:${PREFIX_PIXMAN}/lib/pkgconfig"

echo "Building cairo for {$lower_case}"
if [ "${lower_case}" = "mac" ] || [ "${lower_case}" = "macosx" ]; # Destop OSX path
then
  export CXX="$(xcrun -find -sdk macosx clang++) -Wno-enum-conversion"
  export CC="$(xcrun -find -sdk macosx clang) -Wno-enum-conversion"
  export CFLAGS="-O3 -pthread ${CFLAGS}"
  export CXXFLAGS="-O3 -pthread ${CXXFLAGS}"
  export LDFLAGS="-stdlib=libc++ -framework CoreText -framework CoreFoundation -framework CoreGraphics  ${LDFLAGS}"

	##################################
	## we use cinder to link freetype
	##################################

	CINDER_DIR=`pwd`/../../../..
	CINDER_LIB_DIR=${CINDER_DIR}/lib/${lower_case}/Release
	CINDER_FREETYPE_INCLUDE_PATH=${CINDER_DIR}/include/

  if [ ! -f "${CINDER_LIB_DIR}/libcinder.a" ]; then
    echo "Need to build release version of cinder to run this install. Cairo needs Freetype. Exiting!"
    exit
  fi

	export FREETYPE_LIBS="-L${CINDER_LIB_DIR} -lcinder"
	export FREETYPE_CFLAGS="-I${CINDER_FREETYPE_INCLUDE_PATH}/freetype -I${CINDER_FREETYPE_INCLUDE_PATH}"

	echoFlags
	buildOSX
elif [ "${lower_case}" = "linux" ]; # Linux path
then
  export CXX="clang++ -Wno-enum-conversion"
  export CC="clang -Wno-enum-conversion"
  export CFLAGS="-O3 -pthread ${CFLAGS}"
  export CXXFLAGS="-O3 -pthread ${CXXFLAGS}"
  export LDFLAGS="-stdlib=libc++  ${LDFLAGS}"
	
	##################################
	## we use cinder to link freetype
	##################################

	CINDER_DIR=`pwd`/../../../.. 
	CINDER_LIB_DIR=${CINDER_DIR}/lib/${lower_case}/x86_64/ogl/Release
	CINDER_FREETYPE_INCLUDE_PATH=${CINDER_DIR}/include/

  if [ ! -f "${CINDER_LIB_DIR}/libcinder.a" ]; then
    echo "Need to build release version of cinder to run this install. Cairo needs Freetype. Exiting!"
    exit
  fi

	export FREETYPE_LIBS="-L${CINDER_LIB_DIR} -lcinder"
	export FREETYPE_CFLAGS="-I${CINDER_FREETYPE_INCLUDE_PATH}/freetype -I${CINDER_FREETYPE_INCLUDE_PATH}"
	
	echoFlags
	buildLinux
elif [ "${lower_case}" = "ios" ]; # iOS path
then
  ARCH="arm64"
  HOST="arm-apple-darwin"
  export IOS_PLATFORM="iPhoneOS"
  export IOS_PLATFORM_DEVELOPER="$(xcode-select --print-path)/Platforms/${IOS_PLATFORM}.platform/Developer"
  export IOS_SDK="${IOS_PLATFORM_DEVELOPER}/SDKs/$(ls ${IOS_PLATFORM_DEVELOPER}/SDKs | sort -r | head -n1)"
  echo $IOS_SDK
  export XCODE_DEVELOPER=`xcode-select --print-path`
  export CXX="$(xcrun -find -sdk iphoneos clang++) -Wno-enum-conversion"
  export CC="$(xcrun -find -sdk iphoneos clang) -Wno-enum-conversion"

  export CPPFLAGS="-isysroot ${IOS_SDK} -I${IOS_SDK}/usr/include -arch ${ARCH} -mios-version-min=8.0"
  export CFLAGS="-O3 -pthread ${CFLAGS}"
  #export PIXMAN_CFLAGS_i386="${CFLAGS} -DPIXMAN_NO_TLS"
  #export PIXMAN_CXXFLAGS_i386="${CXXFLAGS} -DPIXMAN_NO_TLS"
  export CXXFLAGS="-O3 -pthread ${CXXFLAGS} -isysroot ${IOS_SDK} -I${IOS_SDK}/usr/include -I${INCLUDEDIR}/pixman-1 -arch ${ARCH} -mios-version-min=    8.0"

  export LDFLAGS="-stdlib=libc++ -isysroot ${IOS_SDK} -L${FINAL_LIB_PATH} -L${IOS_SDK}/usr/lib -arch ${ARCH} -mios-version-min=8.0 -framework CoreText -framework CoreFoundation -framework CoreGraphics  ${LDFLAGS}"
  export PNG_LIBS="-L${IOS_SDK}/usr/lib ${PNG_LIBS}"

	##################################
	## we use cinder to link freetype
	##################################

	CINDER_DIR=`pwd`/../../../..
	CINDER_LIB_DIR=${CINDER_DIR}/lib/${lower_case}/Release
	CINDER_FREETYPE_INCLUDE_PATH=${CINDER_DIR}/include/
	
  if [ ! -f "${CINDER_LIB_DIR}/libcinder.a" ]; then
    echo "Need to build release version of cinder to run this install. Cairo needs Freetype. Exiting!"
    exit
  fi
	
  export FREETYPE_LIBS="-L${CINDER_LIB_DIR} -lcinder"
	export FREETYPE_CFLAGS="-I${CINDER_FREETYPE_INCLUDE_PATH}/freetype -I${CINDER_FREETYPE_INCLUDE_PATH}"
	
	echoFlags
	buildIos
else
	echo "Unkown selection: ${1}"
	echo "usage: ./install.sh [platform]"
	echo "accepted platforms are macosx, linux, ios"
	exit 1
fi

