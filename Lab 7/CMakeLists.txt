cmake_minimum_required(VERSION 2.8)
project( l071 )
find_package( OpenCV REQUIRED )
add_executable( l071 l071.cpp )
target_link_libraries( l071 ${OpenCV_LIBS} )

