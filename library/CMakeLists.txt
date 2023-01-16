set( OpenCV_DIR "C:/Users/shane/Documents/GitHub/opencv/static-build" )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_CXX_STANDARD 20)
find_package( OpenCV REQUIRED)
find_library(FFTW_LIBRARY fftw3)
find_package (Threads)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
file(GLOB_RECURSE SRC  "*.cpp")
add_library (videoPlayer ${SRC})

if( OpenCV_FOUND )
	include_directories( ${OpenCV_INCLUDE_DIRS} )
	link_directories( ${OpenCV_LIB_DIR})
	target_link_libraries(videoPlayer  ${OpenCV_LIBS} ${FFTW_LIBRARY} ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT})

endif()

target_include_directories (videoPlayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})