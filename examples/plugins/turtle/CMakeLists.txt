PROJECT(laskin-turtle-plugin)

FIND_PACKAGE(SDL2 REQUIRED)

ADD_LIBRARY(laskin-turtle-plugin SHARED turtle.cpp)

TARGET_LINK_LIBRARIES(
  laskin-turtle-plugin
  PRIVATE
    laskin
    SDL2::SDL2
)

TARGET_COMPILE_FEATURES(
  laskin-turtle-plugin
  PRIVATE
    cxx_std_20
)

SET_TARGET_PROPERTIES(
  laskin-turtle-plugin
  PROPERTIES
    OUTPUT_NAME
      laskin-turtle-plugin
    PREFIX
      ""
    BUILD_RPATH
      "$ORIGIN/../../../laskin"
)
