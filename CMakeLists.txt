cmake_minimum_required(VERSION 3.10)

project(project-river)
set ( CMAKE_CXX_STANDARD 17 )

set ( source_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/" )
set ( include_dir "${CMAKE_CURRENT_SOURCE_DIR}/include/project-river/" )
set ( imgui_dir "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/" )

set(SOURCES
  ${source_dir}/app.cpp
  ${source_dir}/main.cpp
  ${source_dir}/mat_viewer.cpp
  ${source_dir}/ui.cpp
  ${source_dir}/video_viewer.cpp
  ${source_dir}/optical_flow_viewer.cpp
  ${source_dir}/klt_viewer.cpp
  ${imgui_dir}/imgui.cpp
  ${imgui_dir}/imgui_demo.cpp
  ${imgui_dir}/imgui_draw.cpp
  ${imgui_dir}/imgui_widgets.cpp
  ${imgui_dir}/imgui_tables.cpp
  ${imgui_dir}/imgui_impl_glfw.cpp
  ${imgui_dir}/imgui_impl_opengl3.cpp
  )

find_package ( OpenCV REQUIRED )
include_directories ( ${OpenCV_INCLUDE_DIRS} )
include_directories ( ${include_dir} )

if(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  find_library(OPENGL_LIBRARY OpenGL)
  find_library(IOKIT_LIBRARY IOKit)
  find_library(COREVIDEO_LIBRARY CoreVideo)
  find_package(glfw3 REQUIRED)
  set(EXTRA_LIBS
    ${COCOA_LIBRARY}
    ${OPENGL_LIBRARY}
    ${IOKIT_LIBRARY}
    ${COREVIDEO_LIBRARY}
    ${OpenCV_LIBS}
    glfw
    )
endif(APPLE)

include_directories(${imgui_dir})
add_executable(execute ${SOURCES})
target_link_libraries(execute ${EXTRA_LIBS})

