#!/bin/sh
#
# Run from $(CINDER_DIR)/tools/android/scripts:
#     sh build_boost.sh
#

CINDER_LIB="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"/../../../lib
cd ../../../../boost

echo "Boost strapping for filesystem,system"
sh bootstrap.sh --with-libraries=filesystem,system

########################################################################
########################################################################
########################################################################

PLATFORM=19

BOOST_ARCH=armeabi
ARCH=armeabi
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=armeabi_v7a
ARCH=armeabi-v7a
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=x86
ARCH=x86
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=mips
ARCH=mips
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

########################################################################
########################################################################
########################################################################

PLATFORM=21

BOOST_ARCH=armeabi
ARCH=armeabi
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=armeabi_v7a
ARCH=armeabi-v7a
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=x86
ARCH=x86
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=mips
ARCH=mips
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=arm64_v8a
ARCH=arm64-v8a
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=x86_64
ARCH=x86_64
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

BOOST_ARCH=mips64
ARCH=mips64
echo ">>> Building android-${PLATFORM}/${ARCH}: ./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}"
./b2 -a link=static debug-symbols=off optimization=space toolset=gcc-android_${PLATFORM}_${BOOST_ARCH} --stagedir=stage-${PLATFORM}-${BOOST_ARCH}
echo ">>> cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}"
mkdir -p ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}
cp stage-${PLATFORM}-${BOOST_ARCH}/lib/*.a ${CINDER_LIB}/android/android-${PLATFORM}/${ARCH}

