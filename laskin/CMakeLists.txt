PROJECT(laskin)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindMPFR.cmake)

ADD_LIBRARY(
  laskin
  ./src/ast.cpp
  ./src/chrono.cpp
  ./src/context.cpp
  ./src/error.cpp
  ./src/number.cpp
  ./src/parser.cpp
  ./src/quote.cpp
  ./src/unit.cpp
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
  ./src/operator/multiply.cpp
  ./src/operator/substract.cpp
)

TARGET_INCLUDE_DIRECTORIES(
  laskin
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/ordered-map/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/peelo-chrono/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ext/peelo-unicode/include
)

TARGET_LINK_LIBRARIES(
  laskin
  ${MPFR_LIBRARIES}
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
