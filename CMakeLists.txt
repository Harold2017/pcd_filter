cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project(pcd_filter CXX)

find_package(Open3D REQUIRED)
include_directories(${Open3D_INCLUDE_DIRS})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Subdirectories
add_subdirectory(src/x3 EXCLUDE_FROM_ALL)

add_executable(pcd_filter pcd_filter.cpp)
target_link_libraries(pcd_filter matheval::x3 ${Open3D_LIBRARIES})
