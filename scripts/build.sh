#!/bin/bash

cd ..
rm -r build
mkdir build
pushd build
conan install .. --build spdlog --build=fmt
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_PREFIX=llvm -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..

ninja