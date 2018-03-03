#cmake_minimum_required(VERSION 3.7)
project(ospray_renderer)

set(CMAKE_CXX_STANDARD 11)
set(VTK_DIR /home/cad/下载/VTK-8.1.0)

find_package(VTK REQUIRED)
set(ospray_DIR /home/cad/下载/ospray-1.4.3.x86_64.linux/lib/cmake/ospray-1.4.3)
find_package(ospray REQUIRED)
include(${VTK_USE_FILE})

set(SOURCE_FILES src/main.cpp
        src/VolumeReader.cpp
        src/VolumeReader.h
        src/lodepng.cpp
        src/lodepng.h
	src/BMPWriter.cpp
        src/BMPWriter.h
        src/ParamReader.cpp
        src/ParamReader.h
        src/ComputationTimer.cpp)

set(CMAKE_C_FLAGS_DEBUG -g -D_DEBUG)
include_directories(include)
include_directories(${OSPRAY_INCLUDE_DIR})
link_libraries(${OSPRAY_LIBRARIES} ${VTK_LIBRARIES})
add_executable(renderer ${SOURCE_FILES})
