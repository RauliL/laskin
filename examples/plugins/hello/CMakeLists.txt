PROJECT(laskin-hello-plugin)

ADD_LIBRARY(laskin-hello-plugin SHARED hello.cpp)

TARGET_LINK_LIBRARIES(
  laskin-hello-plugin
  PRIVATE
    laskin
)

TARGET_COMPILE_FEATURES(
  laskin-hello-plugin
  PRIVATE
    cxx_std_20
)

SET_TARGET_PROPERTIES(
  laskin-hello-plugin
  PROPERTIES
    OUTPUT_NAME
      laskin-hello-plugin
    PREFIX
      ""
    BUILD_RPATH
      "$ORIGIN/../../../laskin"
)
