cmake_minimum_required (VERSION 3.10)

# add_definitions ( -std=c++17 )
set ( CMAKE_CXX_STANDARD 17 )

project ( project-find-circle )

set ( CMAKE_BUILD_TYPE Debug )
# if ( UNIX )
#     set ( OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../opencv/opencv_build/" )
# endif()
set ( source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/" )
# set ( optical_flow "${CMAKE_CURRENT_SOURCE_DIR}/include/project-river/" )

find_package ( OpenCV REQUIRED )
# include_directories ( ${OpenCV_DIR} )
include_directories ( ${OpenCV_INCLUDE_DIRS} )
# include_directories ( ${optical_flow} )

add_executable ( execute ${source_dir}/main.cpp ) # ${source_dir}/hough_transform.cpp )
target_link_libraries ( execute ${OpenCV_LIBS} )
