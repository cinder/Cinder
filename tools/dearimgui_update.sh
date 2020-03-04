#!/bin/bash
if [ $# == 0 ] ; then
    echo "Usage: ./dearimgui_update.sh SOURCE DEST"
    exit 1;
fi

dearimgui_path=$1
libcinder_path=$2

target_inc=$libcinder_path/include/imgui
target_src=$libcinder_path/src/imgui

cd $dearimgui_path
cp -v *.h $target_inc; cp -v *.cpp $target_src
cp -v misc/cpp/*.h $target_inc; cp -v misc/cpp/*.cpp $target_src
cp -v misc/freetype/*.h $target_inc; cp -v misc/freetype/*.cpp $target_src
cp -v examples/imgui_impl_opengl3.h $target_inc; cp -v examples/imgui_impl_opengl3.cpp $target_src
