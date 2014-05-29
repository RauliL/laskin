CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(laskin CXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

ADD_EXECUTABLE(
    laskin
    src/main.cpp
    src/token.cpp
    src/value.cpp
)
