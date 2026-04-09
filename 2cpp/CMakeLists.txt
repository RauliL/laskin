CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(laskin2cpp CXX)

INCLUDE(../cmake/enable-all-warnings.cmake)

ADD_EXECUTABLE(
  laskin2cpp
  ./src/instruction.cpp
  ./src/main.cpp
  ./src/program.cpp
  ./src/transpile.cpp
  ./src/writer.cpp
)

ENABLE_ALL_WARNINGS(laskin2cpp)

TARGET_COMPILE_FEATURES(
  laskin2cpp
  PRIVATE
    cxx_std_17
)

TARGET_LINK_LIBRARIES(
  laskin2cpp
  laskin
)

INSTALL(
  TARGETS
    laskin2cpp
  RUNTIME DESTINATION
    bin
)
