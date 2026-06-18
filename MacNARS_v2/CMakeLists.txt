cmake_minimum_required(VERSION 3.15)
project(MacNARS_v2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(MacNARS_v2 src/main.cpp)
