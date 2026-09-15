PROJECT(laskin-cli C CXX)

INCLUDE(../cmake/add-dependency.cmake)
INCLUDE(../cmake/enable-all-warnings.cmake)

SET(IC_BUILD_TESTS OFF CACHE BOOL "" FORCE)

ADD_DEPENDENCY(
  isocline
  https://github.com/daanx/isocline.git
  v1.1.0
)

ADD_EXECUTABLE(
  laskin-cli
  ./src/main.cpp
  ./src/repl.cpp
)

ENABLE_ALL_WARNINGS(laskin-cli)

TARGET_COMPILE_FEATURES(
  laskin-cli
  PRIVATE
    cxx_std_20
)

TARGET_LINK_LIBRARIES(
  laskin-cli
  PRIVATE
    laskin
    isocline
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
