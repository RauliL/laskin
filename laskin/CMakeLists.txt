PROJECT(laskin)

INCLUDE(../cmake/add-dependency.cmake)
INCLUDE(../cmake/enable-all-warnings.cmake)

ADD_HEADER_ONLY_DEPENDENCY(
  ordered-map
  https://github.com/Tessil/ordered-map.git
  v1.2.0
  tsl::ordered_map
  include
)
ADD_DEPENDENCY(
  peelo-chrono
  https://github.com/peelonet/peelo-chrono.git
  v2.0.0
)
ADD_DEPENDENCY(
  peelo-number
  https://github.com/peelonet/peelo-number.git
  v6.0.0
)
ADD_DEPENDENCY(
  peelo-unicode
  https://github.com/peelonet/peelo-unicode.git
  v2.2.0
)

SET(
  LASKIN_SOURCES
  ./src/ast.cpp
  ./src/chrono.cpp
  ./src/context.cpp
  ./src/error.cpp
  ./src/parser.cpp
  ./src/position.cpp
  ./src/quote.cpp
  ./src/record.cpp
  ./src/utils.cpp
  ./src/value.cpp
  ./src/vector.cpp
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

IF(LASKIN_ENABLE_DYNAMIC_LIBRARIES)
  IF(EMSCRIPTEN)
    MESSAGE(
      FATAL_ERROR
      "Dynamic library support is not available in WebAssembly builds."
    )
  ENDIF()
  LIST(APPEND LASKIN_SOURCES ./src/library.cpp)
ENDIF()

IF(LASKIN_ENABLE_DYNAMIC_LIBRARIES)
  ADD_LIBRARY(laskin SHARED ${LASKIN_SOURCES})
ELSE()
  ADD_LIBRARY(laskin ${LASKIN_SOURCES})
ENDIF()

TARGET_INCLUDE_DIRECTORIES(
  laskin
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${MPFR_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(
  laskin
  PUBLIC
    tsl::ordered_map
    PeeloChrono
    PeeloNumber
    PeeloUnicode
  PRIVATE
    ${MPFR_LIBRARIES}
)

IF(LASKIN_ENABLE_DYNAMIC_LIBRARIES)
  ADD_DEPENDENCY(
    dylib
    https://github.com/martin-olivier/dylib.git
    v3.0.1
  )
  TARGET_COMPILE_DEFINITIONS(
    laskin
    PUBLIC
      LASKIN_ENABLE_DYNAMIC_LIBRARIES
  )
  TARGET_LINK_LIBRARIES(
    laskin
    PRIVATE
      dylib
  )
ENDIF()

ENABLE_ALL_WARNINGS(laskin)

TARGET_COMPILE_FEATURES(
  laskin
  PRIVATE
    cxx_std_20
)

IF(EMSCRIPTEN)
  TARGET_COMPILE_OPTIONS(
    laskin
    PRIVATE
      -fwasm-exceptions
  )
  TARGET_LINK_OPTIONS(
    laskin
    PUBLIC
      -fwasm-exceptions
  )
ENDIF()

IF(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  INSTALL(
    TARGETS
      laskin
    RUNTIME DESTINATION
      bin
  )
  INSTALL(
    DIRECTORY
      ./include/laskin
    DESTINATION
      include
    FILES_MATCHING PATTERN
      "*.hpp"
  )
ENDIF()
