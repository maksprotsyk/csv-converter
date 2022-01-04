cmake_minimum_required(VERSION 3.15)
project(hourscounter)

set(CMAKE_CXX_STANDARD 14)

add_executable(hourscounter main.cpp)

find_package(Boost REQUIRED)


include_directories(include)
include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(hourscounter ${Boost_LIBRARIES})