PROJECT(laskin)

INCLUDE(FetchContent)

FetchContent_Declare(
  ordered-map
  GIT_REPOSITORY
    https://github.com/Tessil/ordered-map.git
  GIT_TAG
    v1.2.0
)
FetchContent_Declare(
  peelo-chrono
  GIT_REPOSITORY
    https://github.com/peelonet/peelo-chrono.git
  GIT_TAG
    v1.0.0
)
FetchContent_Declare(
  peelo-number
  GIT_REPOSITORY
    https://github.com/peelonet/peelo-number.git
  GIT_TAG
    v1.0.0
)
FetchContent_Declare(
  peelo-unicode
  GIT_REPOSITORY
    https://github.com/peelonet/peelo-unicode.git
  GIT_TAG
    v2.1.0
)
FetchContent_MakeAvailable(
  ordered-map
  peelo-chrono
  peelo-number
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

IF(MSVC)
  TARGET_COMPILE_OPTIONS(
    laskin
    PRIVATE
      /W4 /WX
  )
ELSE()
  TARGET_COMPILE_OPTIONS(
    laskin
    PRIVATE
      -Wall -Werror
  )
ENDIF()

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
