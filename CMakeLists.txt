CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(laskin C CXX)

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

SET(
  COMMON_SOURCES
  src/ast.cpp
  src/chrono.cpp
  src/context.cpp
  src/error.cpp
  src/main.cpp
  src/number.cpp
  src/parser.cpp
  src/quote.cpp
  src/repl.cpp
  src/unit.cpp
  src/utils.cpp
  src/value.cpp
  src/api/boolean.cpp
  src/api/date.cpp
  src/api/month.cpp
  src/api/number.cpp
  src/api/quote.cpp
  src/api/string.cpp
  src/api/time.cpp
  src/api/utils.cpp
  src/api/vector.cpp
  src/api/weekday.cpp
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

TARGET_COMPILE_OPTIONS(
  laskin
  PRIVATE
    -Wall -Werror
)

TARGET_COMPILE_FEATURES(
  laskin
  PRIVATE
    cxx_std_17
)

TARGET_INCLUDE_DIRECTORIES(
  laskin
  PRIVATE
    ${GMP_INCLUDE_DIR}
    ${GMPXX_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/peelo-chrono/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/peelo-prompt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/peelo-unicode/include
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
