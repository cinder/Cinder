#!/bin/bash
set -e

# CMake
sudo add-apt-repository -y ppa:george-edison55/cmake-3.x && \
  sudo apt-get -y update && \
  sudo apt-get -y install cmake

# Clang
sudo apt-add-repository -y "deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.7 main" && \
  wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|sudo apt-key add - && \
  sudo apt-get -y update && \
  sudo apt-get install -y \
    clang-3.7 \
    lldb-3.7

# Packages
sudo apt-get -y install \
  libxcursor-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxi-dev \
  libgl1-mesa-dev \
  libgles2-mesa-dev \
  zlib1g-dev \
  libfontconfig1-dev \
  libsndfile1 \
  libsndfile1-dev \
  libpulse-dev \
  libasound2-dev \
  libcurl4-gnutls-dev \
  libgstreamer1.0-dev \
  libgstreamer-plugins-bad1.0-dev \
  libgstreamer-plugins-base1.0-dev \
  gstreamer1.0-libav \
  gstreamer1.0-alsa \
  gstreamer1.0-pulseaudio \
  gstreamer1.0-plugins-bad

# mpg123
wget https://sourceforge.net/projects/mpg123/files/mpg123/1.22.4/mpg123-1.22.4.tar.bz2/download -O mpg123-1.22.4.tar.bz2
tar -xvf mpg123-1.22.4.tar.bz2 && \
  cd mpg123-1.22.4 && \
  ./configure --prefix=/opt/local && \
  make && \
  sudo make install
