cmake_minimum_required(VERSION 3.0)
project(udpbox)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(udpbox test/udpbox.cpp)
target_link_libraries(udpbox pthread)
