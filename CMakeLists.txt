CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(laskin CXX)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/peelocpp-unicode/include
)

SET(
  COMMON_SOURCES
  ext/peelocpp-unicode/src/test.cpp
  ext/peelocpp-unicode/src/utf8.cpp
  src/ast.cpp
  src/context.cpp
  src/error.cpp
  src/main.cpp
  src/parser.cpp
  src/quote.cpp
  src/value.cpp
  src/api/quote.cpp
  src/api/utils.cpp
)

ADD_EXECUTABLE(
  laskin
  ${COMMON_SOURCES}
)

INSTALL(
  TARGETS
    laskin
  RUNTIME DESTINATION
    bin
)
