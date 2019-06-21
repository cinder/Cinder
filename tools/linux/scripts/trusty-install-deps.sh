#!/bin/bash
set -e

# Packages
sudo apt-get update
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
  gstreamer1.0-plugins-bad \
  libboost-filesystem-dev

# mpg123
wget https://sourceforge.net/projects/mpg123/files/mpg123/1.22.4/mpg123-1.22.4.tar.bz2/download -O mpg123-1.22.4.tar.bz2
tar -xvf mpg123-1.22.4.tar.bz2 && \
  cd mpg123-1.22.4 && \
  ./configure --prefix=/opt/local && \
  make && \
  sudo make install
