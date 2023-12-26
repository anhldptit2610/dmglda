cmake_minimum_required(VERSION 3.27.7)
project(dmglda)

include(CTest)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(dmglda)
add_subdirectory(testing)