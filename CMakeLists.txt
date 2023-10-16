cmake_minimum_required(VERSION 3.24)
project(MathTeamEventer)

set(CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

add_executable(MathTeamEventer main.cpp)
