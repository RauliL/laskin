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
FIND_PATH(
  READLINE_INCLUDE_DIR
  NAMES readline/readline.h
  DOC "The directory containing readline include files."
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
FIND_LIBRARY(
  READLINE_LIBRARIES
  NAMES readline
  DOC "Path to the readline library."
)
IF(NOT GMP_LIBRARIES OR NOT GMPXX_LIBRARIES)
  MESSAGE(FATAL_ERROR "Could not find GMPXX library.")
ENDIF()
IF(NOT READLINE_LIBRARIES)
  MESSAGE(FATAL_ERROR "Could not find readline library.")
ENDIF()

INCLUDE_DIRECTORIES(
  ${GMP_INCLUDE_DIR}
  ${GMPXX_INCLUDE_DIR}
  ${READLINE_INCLUDE_DIR}
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
  src/number.cpp
  src/parser.cpp
  src/quote.cpp
  src/repl.cpp
  src/unit.cpp
  src/value.cpp
  src/api/boolean.cpp
  src/api/number.cpp
  src/api/quote.cpp
  src/api/string.cpp
  src/api/utils.cpp
  src/api/vector.cpp
  src/operator/add.cpp
  src/operator/compare.cpp
  src/operator/divide.cpp
  src/operator/equals.cpp
  src/operator/multiply.cpp
  src/operator/substract.cpp
)

ADD_EXECUTABLE(
  laskin
  ${COMMON_SOURCES}
)

TARGET_LINK_LIBRARIES(
  laskin
  ${GMP_LIBRARIES}
  ${GMPXX_LIBRARIES}
  ${READLINE_LIBRARIES}
)

INSTALL(
  TARGETS
    laskin
  RUNTIME DESTINATION
    bin
)
