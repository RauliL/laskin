CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(laskin-web CXX)

IF(NOT EMSCRIPTEN)
  MESSAGE(FATAL_ERROR "web/ requires an Emscripten toolchain (use emcmake).")
ENDIF()

INCLUDE(../cmake/enable-all-warnings.cmake)

ADD_EXECUTABLE(
  laskin-web
  ./src/bindings.cpp
)

TARGET_LINK_LIBRARIES(
  laskin-web
  PRIVATE
    laskin
)

ENABLE_ALL_WARNINGS(laskin-web)

TARGET_COMPILE_FEATURES(
  laskin-web
  PRIVATE
    cxx_std_17
)

TARGET_COMPILE_OPTIONS(
  laskin-web
  PRIVATE
    -fwasm-exceptions
)

TARGET_LINK_OPTIONS(
  laskin-web
  PRIVATE
    --bind
    -fwasm-exceptions
    -sMODULARIZE=1
    -sEXPORT_NAME=createLaskinModule
    -sEXPORT_ES6=1
    -sENVIRONMENT=web,node,worker
    -sALLOW_MEMORY_GROWTH=1
)

SET_TARGET_PROPERTIES(
  laskin-web
  PROPERTIES
    OUTPUT_NAME laskin
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist"
)

# Emscripten emits both .js and .wasm next to the output name.
ADD_CUSTOM_COMMAND(
  TARGET laskin-web
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/package.json"
      "${CMAKE_CURRENT_SOURCE_DIR}/index.js"
      "${CMAKE_CURRENT_SOURCE_DIR}/index.d.ts"
      "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
      "${CMAKE_CURRENT_SOURCE_DIR}/dist/"
  COMMENT
    "Copying npm package metadata into web/dist"
)
