#!/bin/bash
set -e

# Packages
sudo dnf -y upgrade
sudo dnf -y install \
  libXcursor-devel \
  libXrandr-devel \
  libXinerama-devel \
  libXi-devel \
  mesa-libGL-devel \
  libglvnd-devel \
  zlib-devel \
  expat-devel \
  freetype-devel \
  uuid-devel \
  libsndfile-devel \
  pulseaudio-libs-devel \
  alsa-lib-devel \
  libcurl-devel \
  gstreamer1-devel \
  gstreamer1-plugins-bad-free-devel \
  gstreamer1-plugins-base-devel \
  gstreamer1-libav \
  boost-devel \
  mpg123-devel