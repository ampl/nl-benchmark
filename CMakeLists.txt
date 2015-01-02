cmake_minimum_required(VERSION 3.0)

# Set the path to CMake modules.
set(MP_CMAKE_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mp/support/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MP_CMAKE_MODULE_DIR})

include(init)

project(MP_BENCHMARK)

add_subdirectory(mp)
add_executable(nl-benchmark nl-benchmark.cc)
target_link_libraries(nl-benchmark asl)
add_custom_target(run-benchmark COMMAND ${CMAKE_SOURCE_DIR}/nl-benchmark.py)