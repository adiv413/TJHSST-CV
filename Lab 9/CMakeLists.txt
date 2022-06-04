cmake_minimum_required(VERSION 2.8)
project( l091 )
find_package( OpenCV REQUIRED )
add_executable( l091 l091.cpp )
target_link_libraries( l091 ${OpenCV_LIBS} )

