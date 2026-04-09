CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
PROJECT(laskin2cpp CXX)

ADD_EXECUTABLE(
  laskin2cpp
  ./src/instruction.cpp
  ./src/main.cpp
  ./src/program.cpp
  ./src/transpile.cpp
  ./src/writer.cpp
)

IF(MSVC)
  TARGET_COMPILE_OPTIONS(
    laskin2cpp
    PRIVATE
      /W4 /WX
  )
ELSE()
  TARGET_COMPILE_OPTIONS(
    laskin2cpp
    PRIVATE
      -Wall -Werror
  )
ENDIF()

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
