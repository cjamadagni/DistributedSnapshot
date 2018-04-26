cmake_minimum_required(VERSION 3.9)

project(DistributedSnapshot)

set(CMAKE_CXX_STANDARD 17)

add_executable(client.out client.cpp socket-wrapper.h socket-wrapper.cpp)
add_executable(server.out server.cpp socket-wrapper.h socket-wrapper.cpp)
add_executable(initialize initialize.cpp)
