

set( OpenCV_DIR "C:/Users/shane/Documents/GitHub/opencv/static-build" )
set(OpenCV_STATIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-p=${CMAKE_BINARY_DIR},-config=")
find_package( OpenCV REQUIRED)
find_package (Threads)

file(GLOB SRC  "*.cpp")
add_library (videoPlayer ${SRC})




find_package (Eigen3 3.3 REQUIRED NO_MODULE)
target_link_libraries( videoPlayer ${CMAKE_THREAD_LIBS_INIT} Eigen3::Eigen)



if( OpenCV_FOUND )

	include_directories( ${OpenCV_INCLUDE_DIRS} )

	link_directories( ${OpenCV_LIB_DIR})
	target_link_libraries(videoPlayer  ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})


endif()




target_include_directories (videoPlayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
