cmake_minimum_required (VERSION 3.10)

# add_definitions ( -std=c++17 )
set ( CMAKE_CXX_STANDARD 17 )

project ( project-river )

# set ( CMAKE_BUILD_TYPE Debug )
# if ( UNIX )
#     set ( OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../opencv/opencv_build/" )
# endif()
set ( Qt5_DIR "/opt/homebrew/Cellar/qt@5/5.15.7/lib/cmake/Qt5/")
set ( source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/" )
set ( optical_flow "${CMAKE_CURRENT_SOURCE_DIR}/include/project-river/" )

set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)

find_package ( OpenCV REQUIRED )
# include_directories ( ${OpenCV_DIR} )
include_directories ( ${OpenCV_INCLUDE_DIRS} )
include_directories ( ${optical_flow} )
find_package ( Qt5 COMPONENTS Widgets REQUIRED )
# qt_standard_project_setup()
qt5_wrap_cpp(execute_SRC ${optical_flow}/mainwindow.h)

add_executable ( execute ${source_dir}/main.cpp ${source_dir}/mainwindow.cpp ${execute_SRC} )
target_link_libraries ( execute ${OpenCV_LIBS} Qt5::Widgets )

# set_target_properties(execute PROPERTIES
#     WIN32_EXECUTABLE ON
#     MACOSX_BUNDLE ON
# )
