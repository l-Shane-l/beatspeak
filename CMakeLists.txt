

cmake_minimum_required(VERSION 3.10)
project(Beatspeak VERSION 1.0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Assuming OpenCV and other dependencies are installed and findable by CMake
find_package(OpenCV REQUIRED)
find_library(FFTW_LIBRARY NAMES fftw3)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(spdlog REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Assuming all source files are under 'src/' and headers are under 'include/'
file(GLOB_RECURSE SRC "src/*.cpp")
add_executable(videoPlayer ${SRC})

target_include_directories(videoPlayer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

# Automatically add all subdirectories containing headers
file(GLOB_RECURSE HEADER_LIST LIST_DIRECTORIES true "*.hpp")
foreach(_header IN LISTS HEADER_LIST)
  get_filename_component(_dir ${_header} PATH)
  target_include_directories(videoPlayer PUBLIC ${_dir})
endforeach()

target_link_libraries(videoPlayer
    ${OpenCV_LIBS}
    ${FFTW_LIBRARY}
    ${CURL_LIBRARIES}
    fmt::fmt
    spdlog::spdlog
    Eigen3::Eigen
    ${CMAKE_THREAD_LIBS_INIT}
    # Add any other necessary libraries or dependencies
)

# Optionally, specify a default start-up project for IDEs like Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT videoPlayer)


