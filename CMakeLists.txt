cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Beatspeak VERSION 1.0.1)


include_directories(include)
add_subdirectory(library)
add_subdirectory (source)






