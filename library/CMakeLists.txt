set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(CURL REQUIRED)
find_library(FFTW_LIBRARY fftw3)
find_package(Eigen3 REQUIRED)
find_library(spdlog_LIBRARY spdlog)




file(GLOB_RECURSE SRC  "*.cpp")
add_library(videoPlayer ${SRC})

if( OpenCV_FOUND )
	include_directories( ${OpenCV_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} )
	target_link_libraries(videoPlayer PRIVATE ${OpenCV_LIBS} ${FFTW_LIBRARY} ${CURL_LIBRARIES} ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT} )

endif()

target_include_directories (videoPlayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})