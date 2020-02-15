cmake_minimum_required(VERSION 2.8.3)
project(alignment)



## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package (OpenCV 3 REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})


add_executable(align align.cpp)

target_link_libraries(align
    ${OpenCV_LIBRARIES}
    )

