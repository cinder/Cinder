#!/bin/bash
if [ $# == 0 ] ; then
    echo "Usage: ./dearimgui_update.sh SOURCE DEST"
    exit 1;
fi

dearimgui_path=$1
libcinder_path=$2
cd $dearimgui_path
cp *.h $libcinder_path/include/imgui; cp *.cpp $libcinder_path/src/imgui
cp --parents misc/cpp/*.h $libcinder_path/include/imgui; cp --parents misc/cpp/*.cpp $libcinder_path/src/imgui
cp --parents misc/freetype/*.h $libcinder_path/include/imgui; cp --parents misc/freetype/*.cpp $libcinder_path/src/imgui
cp --parents examples/imgui_impl_opengl3.h $libcinder_path/include/imgui; cp --parents examples/imgui_impl_opengl3.cpp $libcinder_path/src/imgui