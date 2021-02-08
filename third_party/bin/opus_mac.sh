#!bash
mkdir ./output
rm -rf ./output/opus-mac/*
mkdir ./output/opus-mac
mkdir ./output/opus-mac/libs

cd output/opus-mac
cmake ../../opus-cmake -DPLATFORM_TARGET=osx
cmake --build .