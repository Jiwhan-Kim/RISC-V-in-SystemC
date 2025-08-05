cmake_minimum_required(VERSION 3.1)
project(test)

set (CMAKE_PREFIX_PATH /home/kimjw/systemC/systemc-3.0.1)

include_directories(${CMAKE_PREFIX_PATH}/include)

find_library(systemc systemc ${CMAKE_PREFIX_PATH}/lib-linux64)
link_directories(${CMAKE_PREFIX_PATH}/lib-linux64)

set(CMAKE_CXX_STANDARD 17) # C++11...

set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...

set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(test systemc)
