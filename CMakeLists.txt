CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(laskin CXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

ADD_EXECUTABLE(
    laskin
    src/api-list.cpp
    src/api-numbers.cpp
    src/api-stack.cpp
    src/api-string.cpp
    src/function.cpp
    src/interpreter.cpp
    src/main.cpp
    src/signature.cpp
    src/token.cpp
    src/value.cpp
)
