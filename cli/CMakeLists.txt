CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(laskin-cli C CXX)

ADD_EXECUTABLE(
  laskin-cli
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/repl.cpp
)

TARGET_COMPILE_OPTIONS(
  laskin-cli
  PRIVATE
    -Wall -Werror
)

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
