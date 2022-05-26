cmake_minimum_required(VERSION 2.8)
project( l083 )
find_package( OpenCV REQUIRED )
add_executable( l083 l083.cpp )
target_link_libraries( l083 ${OpenCV_LIBS} )

