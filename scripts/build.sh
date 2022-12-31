#!/bin/bash

cd ..
rm -r build
mkdir build
pushd build
conan install .. --build spdlog --build=fmt
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  ..
ninja