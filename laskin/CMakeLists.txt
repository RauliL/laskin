PROJECT(laskin)

INCLUDE(FetchContent)
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindMPFR.cmake)

FetchContent_Declare(
  ordered-map
  PeeloUnicode
  GIT_REPOSITORY
    https://github.com/Tessil/ordered-map.git
  GIT_TAG
    v1.1.0
)
FetchContent_Declare(
  peelo-chrono
  PeeloUnicode
  GIT_REPOSITORY
    https://github.com/peelonet/peelo-chrono.git
  GIT_TAG
    0.3.0
)
FetchContent_Declare(
  peelo-unicode
  PeeloUnicode
  GIT_REPOSITORY
    https://github.com/peelonet/peelo-unicode.git
  GIT_TAG
    v2.1.0
)
FetchContent_MakeAvailable(
  ordered-map
  peelo-chrono
  peelo-unicode
)

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
)

TARGET_LINK_LIBRARIES(
  laskin
  tsl::ordered_map
  PeeloChrono
  PeeloUnicode
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
