cmake_minimum_required(VERSION 3.25)
project(LoggerProgram)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(LoggerProgram main.cpp Logger.cpp FileHandler.cpp)