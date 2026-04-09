CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(laskin)

INCLUDE(../cmake/add-dependency.cmake)
INCLUDE(../cmake/enable-all-warnings.cmake)

ADD_DEPENDENCY(
  ordered-map
  https://github.com/Tessil/ordered-map.git
  v1.2.0
)
ADD_DEPENDENCY(
  peelo-chrono
  https://github.com/peelonet/peelo-chrono.git
  v1.0.0
)
ADD_DEPENDENCY(
  peelo-number
  https://github.com/peelonet/peelo-number.git
  v2.0.0
)
ADD_DEPENDENCY(
  peelo-unicode
  https://github.com/peelonet/peelo-unicode.git
  v2.1.0
)

ADD_LIBRARY(
  laskin
  ./src/ast.cpp
  ./src/chrono.cpp
  ./src/context.cpp
  ./src/error.cpp
  ./src/parser.cpp
  ./src/position.cpp
  ./src/quote.cpp
  ./src/utils.cpp
  ./src/value.cpp
  ./src/api/boolean.cpp
  ./src/api/date.cpp
  ./src/api/month.cpp
  ./src/api/number.cpp
  ./src/api/quote.cpp
  ./src/api/record.cpp
  ./src/api/string.cpp
  ./src/api/time.cpp
  ./src/api/utils.cpp
  ./src/api/vector.cpp
  ./src/api/weekday.cpp
  ./src/operator/add.cpp
  ./src/operator/compare.cpp
  ./src/operator/divide.cpp
  ./src/operator/equals.cpp
  ./src/operator/modulo.cpp
  ./src/operator/multiply.cpp
  ./src/operator/substract.cpp
)

TARGET_INCLUDE_DIRECTORIES(
  laskin
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${MPFR_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(
  laskin
  tsl::ordered_map
  PeeloChrono
  PeeloNumber
  PeeloUnicode
  ${MPFR_LIBRARIES}
)

ENABLE_ALL_WARNINGS(laskin)

TARGET_COMPILE_FEATURES(
  laskin
  PRIVATE
    cxx_std_17
)

IF(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  INSTALL(
    TARGETS
      laskin
    RUNTIME DESTINATION
      bin
  )
  INSTALL(
    DIRECTORY
      ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION
      include
    FILES_MATCHING PATTERN
      "*.hpp"
  )
ENDIF()
