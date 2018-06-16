CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(laskin CXX)

SET(CMAKE_CXX_STANDARD 17)

FIND_PATH(
  GMP_INCLUDE_DIR
  NAMES gmp.h
  DOC "The directory containing GMP include files."
)
FIND_PATH(
  GMPXX_INCLUDE_DIR
  NAMES gmpxx.h
  DOC "The directory containing GMPXX include files."
)
FIND_LIBRARY(
  GMP_LIBRARIES
  NAMES gmp libgmp
  DOC "Path to the GMP library."
)
FIND_LIBRARY(
  GMPXX_LIBRARIES
  NAMES gmpxx libgmpxx
  DOC "Path to the GMPXX library."
)
IF(NOT GMP_LIBRARIES OR NOT GMPXX_LIBRARIES)
  MESSAGE(FATAL_ERROR "Could not find GMPXX library.")
ENDIF()

INCLUDE_DIRECTORIES(
  ${GMP_INCLUDE_DIR}
  ${GMPXX_INCLUDE_DIR}
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

TARGET_LINK_LIBRARIES(
  laskin
  ${GMP_LIBRARIES}
  ${GMPXX_LIBRARIES}
)

INSTALL(
  TARGETS
    laskin
  RUNTIME DESTINATION
    bin
)
