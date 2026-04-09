CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(laskin-cli C CXX)

INCLUDE(../cmake/enable-all-warnings.cmake)

ADD_EXECUTABLE(
  laskin-cli
  ./src/main.cpp
  ./src/repl.cpp
)

ENABLE_ALL_WARNINGS(laskin-cli)

TARGET_COMPILE_FEATURES(
  laskin-cli
  PRIVATE
    cxx_std_17
)

TARGET_LINK_LIBRARIES(
  laskin-cli
  laskin
)

SET_TARGET_PROPERTIES(
  laskin-cli
  PROPERTIES
    OUTPUT_NAME laskin
)

INSTALL(
  TARGETS
    laskin-cli
  RUNTIME DESTINATION
    bin
)
